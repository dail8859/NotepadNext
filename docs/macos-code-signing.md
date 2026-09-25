# macOS code signing and notarization

## Why the dmg was flagged as "damaged"

The `dmg` cmake target (`cmake/PackagingMac.cmake`) built the app bundle with
`macdeployqt` but never signed it with a Developer ID certificate, and CI never
submitted it to Apple for notarization. macOS marks any file downloaded through
a browser with the `com.apple.quarantine` extended attribute. On launch,
Gatekeeper checks a quarantined app against a notarization ticket; if the app
is unsigned or lacks one, Gatekeeper refuses to open it and reports it as
"damaged and should be moved to the Trash" — the app isn't actually corrupt,
it's just unsigned/unnotarized.

## What CI now does

`.github/workflows/build.yml` will sign, notarize, and staple the macOS dmg
automatically, but only when the required repository secrets are present. If
they're not set, the build behaves exactly as before (unsigned dmg, no
failure) — the workflow checks `MACOS_HAS_CODESIGN_SECRETS` before running any
of the signing/notarization steps.

## Required repository secrets

You need a paid Apple Developer Program membership ($99/year) to get a
Developer ID Application certificate; there is no free path to a Gatekeeper-
clean dmg for distribution outside the App Store.

| Secret | How to get it |
| --- | --- |
| `MACOS_CERTIFICATE_P12` | In Xcode or Keychain Access, export your **Developer ID Application** certificate (with its private key) as a `.p12` file, then `base64 -i cert.p12 \| pbcopy` and paste that as the secret value. |
| `MACOS_CERTIFICATE_PASSWORD` | The password you set when exporting the `.p12`. |
| `MACOS_CODESIGN_IDENTITY` | The identity string, e.g. `Developer ID Application: Your Name (TEAMID)`. List it locally with `security find-identity -v -p codesigning`. |
| `APPLE_NOTARIZATION_APPLE_ID` | The Apple ID email tied to your Developer account. |
| `APPLE_NOTARIZATION_PASSWORD` | An **app-specific password** for that Apple ID, generated at https://appleid.apple.com/account/manage — not your normal Apple ID password. |
| `APPLE_TEAM_ID` | Your 10-character Apple Developer Team ID, visible at https://developer.apple.com/account under Membership. |

Add these under the fork's repo Settings → Secrets and variables → Actions.

## What happens in CI, in order

1. **Import Code-Signing Certificate (mac)** decodes `MACOS_CERTIFICATE_P12`
   into a temporary keychain scoped to the runner (`$RUNNER_TEMP`), unlocked
   for the job only.
2. **Configure** passes `-DMACOS_CODESIGN_IDENTITY=...` to cmake.
3. `cmake/PackagingMac.cmake` runs `macdeployqt` with
   `-sign-for-notarization=<identity>`, which codesigns the bundle with the
   hardened runtime and a secure timestamp — both required for notarization.
4. **Notarize and Staple DMG (mac)** submits the dmg via
   `xcrun notarytool submit --wait`, then staples the resulting ticket to the
   dmg with `xcrun stapler staple` so it verifies offline.
5. **Verify Gatekeeper Acceptance (mac)** mounts the stapled dmg and runs
   `codesign --verify --deep --strict`, `spctl --assess --type execute`, and
   `xcrun stapler validate` against the mounted app/dmg — this is the CI
   regression check that a downloaded copy will actually pass Gatekeeper; if
   signing regresses, this step fails the build instead of shipping a broken
   dmg silently.

## Verifying locally after downloading a release

```bash
spctl --assess --type execute --verbose=2 /Applications/NotepadNext.app
```

Should print `accepted` once a signed/notarized release is downloaded.
