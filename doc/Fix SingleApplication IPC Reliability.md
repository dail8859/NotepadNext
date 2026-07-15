# Fix: SingleApplication IPC Message Delivery Reliability

## Problem

When opening files via Windows Explorer right-click context menu (or any external invocation), NotepadNext intermittently activates its window but fails to open the requested file. The issue is more frequent when many tabs are open.

## Root Cause Analysis

NotepadNext uses [itay-grudev/SingleApplication](https://github.com/itay-grudev/SingleApplication) (v3.5.2) for single-instance management. When a secondary instance launches:

1. The `SingleApplication` constructor performs a handshake with the primary, emitting `instanceStarted` → `bringWindowToForeground()` (window activates)
2. `sendInfoToPrimaryInstance()` then calls `sendMessage(buffer)` to deliver file paths → `receivedMessage` → `openFiles()` (file opens)

These are **two independent IPC operations**. The window activation (step 1) always succeeds because the constructor uses a 1000ms timeout. But `sendMessage()` uses a **default timeout of only 100ms** with **no retry logic**.

### Why 100ms is insufficient

The `sendMessage()` timeout budget is shared across:
- Socket reconnection (`connectToPrimary` with `randomSleep` of 8-18ms)
- Two confirmed frame writes (header + body), each requiring a round-trip ACK from the primary

The primary sends ACKs inside its event loop. When the event loop is blocked by:
- **Session auto-save** (runs every 60 seconds via `autoSaveTimer`, iterates all editors)
- **File-change detection** (synchronous `stat` calls + potential modal `QMessageBox`)
- **UI processing** (large session restore, heavy repaint)

...the ACKs are delayed and the 100ms budget is exhausted. `sendMessage()` returns `false`, the secondary logs a warning and exits silently. The user sees the window come forward but no file opens.

### Evidence from codebase

- Commit `22a9590` ("Add more debugging for singleapplication handling", ref #541) explicitly acknowledges this failure mode
- Commit `e9b5395` added `Qt::QueuedConnection` to work around threading issues in the `receivedMessage` signal
- Commit `bfd5742` fixed a separate IPC bug where relative paths were sent instead of absolute

## Fix

**Commit:** `f73a8ad`
**Files changed:** `src/NotepadNextApplication.cpp`, `src/NotepadNextApplication.h`, `src/main.cpp`

### 1. Increased timeout + retry logic (`NotepadNextApplication.cpp`)

Replaced single `sendMessage(buffer)` (100ms, no retry) with a 3-attempt retry loop at 1000ms per attempt:

```cpp
const int maxAttempts = 3;
const int timeout = 1000; // ms per attempt

for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
    if (sendMessage(buffer, timeout)) {
        qInfo("sendMessage() successful on attempt %d", attempt);
        return true;
    }
    qWarning("sendMessage() attempt %d of %d failed", attempt, maxAttempts);
}

qCritical("Failed to send message to primary instance after %d attempts", maxAttempts);
return false;
```

**Why 3 × 1000ms:**
- Attempt 1: covers session save blocking (~50-200ms)
- Attempt 2: covers modal dialog blocking (user typically clicks within 2s)
- Attempt 3: safety net
- Worst case total: ~3 seconds — imperceptible to user

### 2. Return type change (`NotepadNextApplication.h`)

`sendInfoToPrimaryInstance()` changed from `void` to `bool` to propagate success/failure to `main.cpp`.

### 3. User feedback on failure (`main.cpp`)

When all retry attempts fail, a `QMessageBox::warning` is shown instead of silently exiting:

```cpp
if (!app.sendInfoToPrimaryInstance()) {
    QMessageBox::warning(nullptr,
        QGuiApplication::applicationDisplayName(),
        QObject::tr("Unable to open the file(s) in the running Notepad Next window.\n\n"
                    "Please try again, or close and reopen Notepad Next."));
}
```

## Diff

```diff
--- a/src/NotepadNextApplication.cpp
+++ b/src/NotepadNextApplication.cpp
-void NotepadNextApplication::sendInfoToPrimaryInstance()
+bool NotepadNextApplication::sendInfoToPrimaryInstance()
     // ... argument serialization unchanged ...
-    const bool success = sendMessage(buffer);
-    if (!success) {
-        qWarning("sendMessage() unsuccessful");
+    const int maxAttempts = 3;
+    const int timeout = 1000;
+    for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
+        if (sendMessage(buffer, timeout)) { return true; }
     }
+    return false;

--- a/src/main.cpp
+++ b/src/main.cpp
-        app.sendInfoToPrimaryInstance();
+        if (!app.sendInfoToPrimaryInstance()) {
+            QMessageBox::warning(...);
+        }
```

## Not in scope

| Alternative | Reason not adopted |
|---|---|
| Modify SingleApplication library source | Third-party CPM dependency, fork maintenance burden |
| Run event loop in secondary for async retry | Two processes visible in task manager, confusing |
| Defer `bringWindowToForeground` to `receivedMessage` | Breaks "activate window with no files" use case |
| Add `--new-instance` flag | Separate feature, not a bug fix |
| Secondary falls back to `init()` on IPC failure | Conflicts with shared memory / mutex |
