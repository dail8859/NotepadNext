!addplugindir /x86-unicode ".\NsisMultiUser\Plugins\x86-unicode\"
!addincludedir ".\NsisMultiUser\Include\"

SetCompressor /SOLID lzma

!getdllversion "..\build\package\NotepadNext.exe" nnver_
!define VERSION ${nnver_1}.${nnver_2}
!define VERSION_MAJOR ${nnver_1}
!define VERSION_MINOR ${nnver_2}
!define VERSION_PATCH ${nnver_3}
# dont think I'd ever need beyond a patch number

!if ${VERSION_PATCH} != 0
	!undef VERSION
	!define VERSION ${nnver_1}.${nnver_2}.${nnver_3}
!endif

# Configure NsisMultiUser: https://github.com/Drizin/NsisMultiUser/wiki
!define PRODUCT_NAME "Notepad Next"
!define COMPANY_NAME "${PRODUCT_NAME}"
!define PROGEXE "notepadnext.exe"
!define MULTIUSER_INSTALLMODE_UNINSTALL_REGISTRY_KEY "NotepadNext"
!define MULTIUSER_INSTALLMODE_DISPLAYNAME "${PRODUCT_NAME}"
!define MULTIUSER_INSTALLMODE_64_BIT 1
!define MULTIUSER_INSTALLMODE_DEFAULT_ALLUSERS 1
!define MULTIUSER_INSTALLMODE_ALLOW_BOTH_INSTALLATIONS 0
!define MULTIUSER_INSTALLMODE_ALLOW_ELEVATION 1
!define MULTIUSER_INSTALLMODE_ALLOW_ELEVATION_IF_SILENT 1 ; required for silent-mode allusers-uninstall to work, when using the workaround for Windows elevation bug


# Configure MUI
!define MUI_ABORTWARNING
!define MUI_ICON "..\icon\NotepadNext.ico"
!define MUI_UNICON "..\icon\NotepadNext.ico"
!define MUI_COMPONENTSPAGE_NODESC

!include "NsisMultiUser.nsh"
!include "NsisMultiUserLang.nsh"
!include "MUI2.nsh"
!include "Memento.nsh"
!include "FileFunc.nsh"
!include "utils.nsh"

# Detect if the Notepad Next application is running
!insertmacro CheckIfRunning ""
!insertmacro CheckIfRunning "un."

# Configure Memento
!define MEMENTO_REGISTRY_ROOT SHCTX
!define MEMENTO_REGISTRY_KEY Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext


# Install pages
!define MUI_PAGE_CUSTOMFUNCTION_PRE PageWelcomeLicensePre
!insertmacro MUI_PAGE_WELCOME
!define MUI_PAGE_CUSTOMFUNCTION_PRE PageWelcomeLicensePre
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MULTIUSER_PAGE_INSTALLMODE
!insertmacro MUI_PAGE_COMPONENTS
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE VerifyInstallDirEmpty
!insertmacro MUI_PAGE_DIRECTORY # In which folder install page.
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "CheckIfRunning"
!insertmacro MUI_PAGE_INSTFILES # Installing page.
# After installation, run Notepad Next by default
!define MUI_FINISHPAGE_RUN "$INSTDIR\NotepadNext.exe"
!insertmacro MUI_PAGE_FINISH # Finished installation page.


# Uninstall pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MULTIUSER_UNPAGE_INSTALLMODE
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "un.CheckIfRunning"
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


# Language stuff
!insertmacro MUI_LANGUAGE "English"
!insertmacro MULTIUSER_LANGUAGE_INIT



Name "Notepad Next v${VERSION}"
OutFile "NotepadNext-v${VERSION}-Installer.exe"
ShowInstDetails show
BrandingText " "

# Installer Information
VIProductVersion "${nnver_1}.${nnver_2}.${nnver_3}.${nnver_4}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Notepad Next"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright 2019 Justin Dailey"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Notepad Next v${VERSION} Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}"


Function PageWelcomeLicensePre
	${if} $InstallShowPagesBeforeComponents == 0
		Abort ; don't display the Welcome and License pages for the inner instance 
	${endif}
FunctionEnd

Function VerifyInstallDirEmpty
	Push $INSTDIR
	Call isEmptyDir
	Pop $0

	${If} $0 == 0
	${AndIf} ${FileExists} "$INSTDIR\*"
		SetRegView 64

		# The uninstaller will run prior to executing the installer (but has not been ran yet)
		# so if it is getting installed to the same location as the prior version, it is fine.
		ReadRegStr $R0 SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "InstallLocation"

		# The directory is the same as the previous version, so no need to worry.
		${If} $R0 == $INSTDIR
			Goto done
		${EndIf}

		MessageBox MB_ICONEXCLAMATION|MB_YESNO \
			`"$INSTDIR" already exists and is not empty! \
			This installer will delete all files and folders in that directory before \
			installing!$\n$\n\
			Do you want to continue?` \
			/SD IDYES \
			IDYES done
		Abort
	${EndIf}
	done:
FunctionEnd


Function .onInit
	${ifnot} ${UAC_IsInnerInstance}
		!insertmacro CheckSingleInstance "Setup" "Global" "NotepadNextSetupMutex"
	${endif}

	!insertmacro MULTIUSER_INIT

	${MementoSectionRestore}

	# Trigger the selection change manually so that the hidden sections get set appropriately
	Call .onSelChange
FunctionEnd

Function .onInstSuccess
	${MementoSectionSave}
FunctionEnd

Function un.onInit
	!insertmacro MULTIUSER_UNINIT
FunctionEnd

Section "-Run Uninstaller" SEC_UNINSTALL_PREVIOUS
	SetRegView 64

	; Check for uninstaller registry entry
	ReadRegStr $R0 SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "QuietUninstallString"

	${If} $R0 == ""
		Goto Done
	${EndIf}

	DetailPrint "Removing previous installation..."

	; Run the uninstaller
	ExecWait '$R0'

	Done:
SectionEnd

Section "Notepad Next"
	SectionIn RO
	SetOutPath $INSTDIR

	# Make sure it is empty
	RMDir /r $INSTDIR

	File /r /x libcrypto-1_1-x64.dll /x libssl-1_1-x64.dll ..\build\package\*

	SetRegView 64

	# Register the application. https://learn.microsoft.com/en-us/windows/win32/shell/app-registration#registering-applications
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\App Paths\NotepadNext.exe" "" "$INSTDIR\NotepadNext.exe"

	# Register verbs. https://learn.microsoft.com/en-us/windows/win32/shell/app-registration#registering-verbs-and-other-file-association-information
	WriteRegStr SHCTX "Software\Classes\Applications\NotepadNext.exe\shell\open\command" "" "$\"$INSTDIR\NotepadNext.exe$\" $\"%1$\""
	WriteRegStr SHCTX "Software\Classes\Applications\NotepadNext.exe\shell\edit\command" "" "$\"$INSTDIR\NotepadNext.exe$\" $\"%1$\""

	# Register it the perceived type so that Windows will suggest it as a possible application to use
	WriteRegStr SHCTX "Software\Classes\SystemFileAssociations\text\OpenWithList\NotepadNext.exe" "" ""

	WriteUninstaller "$INSTDIR\uninstall.exe"
	!insertmacro MULTIUSER_RegistryAddInstallInfo

	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IfErrors +3 0
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "EstimatedSize" "$0"
SectionEnd

# -----------------------------------------------

${MementoUnselectedSection} "Desktop Shortcut" SEC_DESKTOP_SHORTCUT
	CreateShortCut "$DESKTOP\Notepad Next.lnk" "$INSTDIR\NotepadNext.exe"
${MementoSectionEnd}

Section "-Remove Desktop Shortcut" SEC_REMOVE_DESKTOP_SHORTCUT
	Delete "$DESKTOP\Notepad Next.lnk"
SectionEnd

# -----------------------------------------------

${MementoSection} "Start Menu Shortcut" SEC_START_MENU_SHORTCUT
	CreateShortCut "$SMPROGRAMS\Notepad Next.lnk" "$INSTDIR\NotepadNext.exe"
${MementoSectionEnd}

Section "-Start Menu Shortcut" SEC_REMOVE_START_MENU_SHORTCUT
	Delete "$SMPROGRAMS\Notepad Next.lnk"
SectionEnd

# -----------------------------------------------

${MementoSection} "Context Menu" SEC_CONTEXT_MENU
	SetRegView 64

	WriteRegStr SHCTX "Software\Classes\*\shell\NotepadNext" "" "Edit with Notepad Next"
	WriteRegStr SHCTX "Software\Classes\*\shell\NotepadNext" "icon" "$INSTDIR\NotepadNext.exe"
	WriteRegStr SHCTX "Software\Classes\*\shell\NotepadNext\command" "" "$\"$INSTDIR\NotepadNext.exe$\" $\"%1$\""
${MementoSectionEnd}

Section "-Context Menu" SEC_REMOVE_CONTEXT_MENU
	SetRegView 64

	DeleteRegKey SHCTX "Software\Classes\*\shell\NotepadNext"
SectionEnd

# -----------------------------------------------

${MementoSection} "Auto Updater" SEC_AUTO_UPDATER
	SetRegView 64
	SetOutPath $INSTDIR

	File ..\build\package\libcrypto-1_1-x64.dll ..\build\package\libssl-1_1-x64.dll

	WriteRegDWORD SHCTX "Software\NotepadNext\NotepadNext\" "AutoUpdate" 1
${MementoSectionEnd}

Section "-Auto Updater" SEC_REMOVE_AUTO_UPDATER
	SetRegView 64

	# Disable the auto update, if there was an existing install the DLLs may hang around but that's fine for now
	WriteRegDWORD SHCTX "Software\NotepadNext\NotepadNext\" "AutoUpdate" 0
SectionEnd

# -----------------------------------------------


${MementoSectionDone}


Function .onSelChange
${If} ${SectionIsSelected} ${SEC_DESKTOP_SHORTCUT}
	!insertmacro UnselectSection ${SEC_REMOVE_DESKTOP_SHORTCUT}
${Else}
	!insertmacro SelectSection ${SEC_REMOVE_DESKTOP_SHORTCUT}
${EndIf}

${If} ${SectionIsSelected} ${SEC_START_MENU_SHORTCUT}
	!insertmacro UnselectSection ${SEC_REMOVE_START_MENU_SHORTCUT}
${Else}
	!insertmacro SelectSection ${SEC_REMOVE_START_MENU_SHORTCUT}
${EndIf}

${If} ${SectionIsSelected} ${SEC_CONTEXT_MENU}
	!insertmacro UnselectSection ${SEC_REMOVE_CONTEXT_MENU}
${Else}
	!insertmacro SelectSection ${SEC_REMOVE_CONTEXT_MENU}
${EndIf}

${If} ${SectionIsSelected} ${SEC_AUTO_UPDATER}
	!insertmacro UnselectSection ${SEC_REMOVE_AUTO_UPDATER}
${Else}
	!insertmacro SelectSection ${SEC_REMOVE_AUTO_UPDATER}
${EndIf}
FunctionEnd


Section "Uninstall"
	SetRegView 64

	RMDir /r $INSTDIR

	# Desktop shortcut
	Delete "$DESKTOP\Notepad Next.lnk"

	# Start Menu shortcut
	Delete "$SMPROGRAMS\Notepad Next.lnk"

	# Context menu registry
	DeleteRegKey SHCTX "Software\Classes\*\shell\NotepadNext"

	# Remove application registration
	DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\App Paths\NotepadNext.exe" "" "$INSTDIR\NotepadNext.exe"

	# Remove registered verbs
	DeleteRegKey SHCTX "Software\Classes\Applications\NotepadNext.exe"

	# Remove type registration
	DeleteRegKey SHCTX "Software\Classes\SystemFileAssociations\text\OpenWithList\NotepadNext.exe"

	!insertmacro MULTIUSER_RegistryRemoveInstallInfo 
SectionEnd
