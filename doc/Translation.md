# Translation

## Overview of the Process

1. The strings meant for translation are marked using tr() in source code.
2. lupdate - A tool that scans the source files for tr() and places the strings in a .ts xml file. At this point the .ts file contains only strings that are meant to be translated.
3. A translator provides translations by opening the .ts file using Qt Linguist. At this point the .ts file contains both the string to be translated and the translation. 
3. lrelease - A tool that takes the .ts which contains translated strings and converts them into a binary .qm format that can be loaded into an application at runtime.
4. Application uses QTranslator is used to load .qm file(s) depending on the locale/settings.
5. QCoreApplication::installTranslator is used to install the QTranslator.
6. Result : All tr() invocations of step1 automatically get translations made in step 3.

## Reference and Detailed Info
[Internationalization with Qt](https://doc.qt.io/qt-5/internationalization.html)  
[Internationalization Wiki](https://wiki.qt.io/QtInternationalization)  
[Qt Linguist Manual](https://doc.qt.io/qt-5/linguist-translators.html)  
[TS file format](https://doc.qt.io/archives/qt-5.6/linguist-ts-file-format.html)  

## Required Tools

For translators:
- Qt Linguist

For developers:
- lupdate
- lrelease

## Translation Process for Developers

### Project Setup

Below is a snippet of the .pro file that contains French and Dutch translations.

```
TRANSLATIONS = arrowpad_fr.ts \
               arrowpad_nl.ts
```

### String Literals

Wherever your program uses a string literal (quoted text) that will be presented to the user, ensure that it is processed by the QCoreApplication::translate() function.
```
 QLabel *label = new QLabel(tr("Password:"));
```

If the quoted text is not in a member function of a QObject subclass, use either the tr() function of an appropriate class, or the QCoreApplication::translate() function directly:
```
void some_global_function(LoginWidget *logwid)
{
    QLabel *label = new QLabel(
                LoginWidget::tr("Password:"), logwid);
}

void same_global_function(LoginWidget *logwid)
{
    QLabel *label = new QLabel(
                QCoreApplication::translate("LoginWidget", "Password:"), logwid);
}
```


### Generating Translation Files

If new language is added to the project or new phrases (QLabels, any literals taht apperas on UI) added to the source code, run ```lupdate NotepadNext.pro```.  
This will produces or updates TS translation files.

### Translating with Qt Linguist

At this point there is no work from a developer side.

### Generating Translation Binary Files

Run lrelease to produce QM files out of TS files. The QM file format is a compact binary format that is used by the localized application.

```lrelease NotepadNext.pro```

TODO: "The TS files lrelease processes can be specified at the command line, or given indirectly by a Qt .pro project file." Look up this, can it be automated? 

### Use Translation Binary Files in Application

A very simple example of loading translation files. Once it is implemented, there is no more work on it.

```
QString locale = QLocale::system().name();
QTranslator translator;
translator.load(QString("arrowpad_") + locale);
app.installTranslator(&translator);
```

## Translation Process for Translators

1. If you want to help with tanslating but the targe language is not supported yet, please let the developers know and they create a new .ts file which you can use.
2. If the file is ready or has been already created, then start Qt Linguist.
3. Open the .ts file and translate the phrases.
4. Update the new .ts file in the NotepadNext repo.

QT Linguist is contained in the Qt SDK that you can download [here](https://www.qt.io/download-qt-installer).  
If you don't want to download the whole SDK, there is a community made standalone version for Qt Linguist: [https://github.com/lelegard/qtlinguist-installers/releases](https://github.com/lelegard/qtlinguist-installers/releases)  
If you don't want to install anything or the standalone version does not work as intended, we can send a list of phrases to translate.