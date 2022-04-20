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
!define MULTIUSER_INSTALLMODE_DISPLAYNAME "${PRODUCT_NAME} v${VERSION}"
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
!insertmacro MUI_PAGE_DIRECTORY # In which folder install page.
!insertmacro MUI_PAGE_INSTFILES # Installing page.
!insertmacro MUI_PAGE_FINISH # Finished installation page.


# Uninstall pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MULTIUSER_UNPAGE_INSTALLMODE
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
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${nnver_1}.${nnver_2}"


Function PageWelcomeLicensePre
	${if} $InstallShowPagesBeforeComponents == 0
		Abort ; don't display the Welcome and License pages for the inner instance 
	${endif}
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


Section "Notepad Next"
	SectionIn RO
	SetOutPath $INSTDIR

	File /r /x libcrypto-1_1-x64.dll /x libssl-1_1-x64.dll ..\build\package\*

	SetRegView 64

	# Register the application (e.g. cmd> start notepadnext)
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\App Paths\NotepadNext.exe" "" "$INSTDIR\NotepadNext.exe"

	# Register 'Open With' menu suggestion. No real good documentation for this. https://stackoverflow.com/a/62783311
	WriteRegStr SHCTX "Software\Classes\NotepadNext\shell" "" "open"
	WriteRegStr SHCTX "Software\Classes\NotepadNext\shell\open\command" "" "$\"$INSTDIR\NotepadNext.exe$\" $\"%1$\""
	WriteRegStr SHCTX "Software\Classes\.txt\OpenWithProgids" "NotepadNext" ""
	# TODO: add more extensions

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

${MementoUnselectedSection} "Start Menu Shortcut" SEC_START_MENU_SHORTCUT
	CreateShortCut "$SMPROGRAMS\Notepad Next.lnk" "$INSTDIR\NotepadNext.exe"
${MementoSectionEnd}

Section "-Start Menu Shortcut" SEC_REMOVE_START_MENU_SHORTCUT
	Delete "$SMPROGRAMS\Notepad Next.lnk"
SectionEnd

# -----------------------------------------------

${MementoUnselectedSection} "Context Menu" SEC_CONTEXT_MENU
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

${MementoUnselectedSection} "Auto Updater" SEC_AUTO_UPDATER
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

	# Custom configurations
	DeleteRegKey SHCTX "Software\NotepadNext\"

	# Remove 'Open With' menu suggestion
	DeleteRegValue SHCTX "Software\Classes\.txt\OpenWithProgids" "NotepadNext"
	DeleteRegKey SHCTX "Software\Classes\NotepadNext"
	
	!insertmacro MULTIUSER_RegistryRemoveInstallInfo 
SectionEnd
