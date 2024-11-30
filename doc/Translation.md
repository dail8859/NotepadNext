# Translation

## Overview of the Process

1. Strings meant for translation are marked using `tr()` in the source code.
1. Qt's translation tool `lupdate` scans the source files for `tr()` and places the strings in a .ts XML file. At this point, the .ts file contains only strings that are meant to be translated.
1. A translator provides translations by opening the .ts file using Qt Linguist. At this point, the .ts file contains both the string to be translated and the translation.
1. When the application is built, the .ts files are converted into a binary .qm format.
1. The application loads the .qm files at runtime (depending on the locale/settings) and all `tr()` invocations are translated.

## Required Tools

For translators:
- Qt Linguist

For developers:
- lupdate

## Translation Process for Developers

Follow the [Writing Source Code for Translation](https://doc.qt.io/qt-6/i18n-source-translation.html) guide to properly expose string to be translated. Once strings are ready to be released to translators, use the `lupdate` tool. If you use Qt creator, you can execute the lupdate command from the menu `Tools > External > Qt linguist > lupdate` or from the console/terminal:

```
$ lupdate NotepadNext.pro
```

When the application is built, it automatically uses `lrelease` to convert the list of .ts files into binary .qm files for the application to load.

## Translation Process for Translators

If you want to help with translations you can follow the [Qt Linguist Manual](https://doc.qt.io/qt-6/linguist-translators.html). There may be a partial application translation already in the `/i18n/` directory. You can update it or create a new `.ts` file if your language does not exist yet. Once changes have been made to the `.ts`
. file you can create a pull request so new translated strings can be included in the application.

Qt Linguist is contained in the Qt SDK that you can download [here](https://www.qt.io/download-qt-installer). If you don't want to download the whole SDK, there is a community made a standalone version for Qt Linguist [here](https://github.com/lelegard/qtlinguist-installers/releases).

## Reference and Detailed Info

- [Internationalization with Qt](https://doc.qt.io/qt-6/internationalization.html)
- [Writing Source Code for Translation](https://doc.qt.io/qt-6/i18n-source-translation.html)
- [Qt Linguist Manual](https://doc.qt.io/qt-6/linguist-translators.html)
