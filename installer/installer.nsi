SetCompressor /SOLID lzma

# Configure the MultiUser settings before including it
!define MULTIUSER_INSTALLMODE_INSTDIR "NotepadNext"
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_USE_PROGRAMFILES64

!getdllversion "..\build\package\NotepadNext.exe" nnver_
!define APPVERSION ${nnver_1}.${nnver_2}
!define VERSION_MAJOR ${nnver_1}
!define VERSION_MINOR ${nnver_2}
!define VERSION_PATCH ${nnver_3}
# dont think I'd ever need beyond a patch number

!if ${VERSION_PATCH} != 0
	!undef APPVERSION
	!define APPVERSION ${nnver_1}.${nnver_2}.${nnver_3}
!endif

# Configure MUI
!define MUI_ABORTWARNING
!define MUI_ICON "..\src\NotepadNext\icons\newfile.ico"
!define MUI_UNICON "..\src\NotepadNext\icons\newfile.ico"
!define MUI_COMPONENTSPAGE_NODESC

# Now that everything is defined, add includes
!include "MultiUser.nsh"
!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "UninstallExisting.nsh"

# Install pages
!insertmacro MUI_PAGE_WELCOME # Welcome to the installer page.
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MULTIUSER_PAGE_INSTALLMODE
!insertmacro MUI_PAGE_DIRECTORY # In which folder install page.
!insertmacro MUI_PAGE_INSTFILES # Installing page.
!insertmacro MUI_PAGE_FINISH # Finished installation page.

# Uninstall pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Name "Notepad Next v${APPVERSION}"
OutFile "NotepadNext-v${APPVERSION}-Installer.exe"
ShowInstDetails show
BrandingText " "

# Installer Information
VIProductVersion "${nnver_1}.${nnver_2}.${nnver_3}.${nnver_4}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${APPVERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Notepad Next"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright 2019 Justin Dailey"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Notepad Next ${APPVERSION} Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${nnver_1}.${nnver_2}"


Function .onInit
	SetRegView 64

	System::Call 'kernel32::CreateMutex(p 0, i 0, t "myMutex") p .r1 ?e'
	Pop $R0

	StrCmp $R0 0 +3
	MessageBox MB_OK|MB_ICONEXCLAMATION "Another instance of the installer is already running."
	Abort

	!insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
	SetRegView 64

	!insertmacro MULTIUSER_UNINIT
FunctionEnd


Section "Notepad Next"
	SectionIn RO
	SetOutPath $INSTDIR

	# Check to see if there is uninstaller information
	ReadRegStr $0 SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "UninstallString"
	${If} $0 != ""
		ReadRegStr $1 SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayVersion"
		${If} ${Cmd} `MessageBox MB_YESNO|MB_ICONQUESTION "Notepad Next v$1 is already installed. Uninstall previous version?" /SD IDYES IDYES`
			!insertmacro UninstallExisting $0 $0
			${If} $0 <> 0
				MessageBox MB_YESNO|MB_ICONSTOP "Failed to uninstall, continue anyway?" /SD IDYES IDYES +2
				Abort
			${Else}
				MessageBox MB_OK|MB_ICONINFORMATION "The previous version has been successfully uninstalled."
			${EndIf}
		${EndIf}
	${EndIf}

	File /r ..\build\package\*

	SetRegView 64

	# Installation info to show up in the Add/Remove panel
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayName" "Notepad Next ${APPVERSION}"
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "Publisher" "Notepad Next"
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayVersion" "${APPVERSION}"
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayIcon" "$INSTDIR\NotepadNext.exe"
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "NoModify" 1
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "NoRepair" 1
	
	# Specifically call the installer however this is set to install
	${If} $MultiUser.InstallMode == "AllUsers"
		WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "UninstallString" "$\"$INSTDIR\uninstall.exe$\" /AllUsers"
		WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /AllUsers /S"
	${Else}
		WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "UninstallString" "$\"$INSTDIR\uninstall.exe$\" /CurrentUser"
		WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /CurrentUser /S"
	${EndIf}

	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IfErrors +3 0
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "EstimatedSize" "$0"

	# Register the application 
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\App Paths\NotepadNext.exe" "" "$INSTDIR\NotepadNext.exe"

	# Register 'Open With' menu suggestion. No real good documentation for this. https://stackoverflow.com/a/62783311
	WriteRegStr SHCTX "Software\Classes\NotepadNext\shell" "" "open"
	WriteRegStr SHCTX "Software\Classes\NotepadNext\shell\open\command" "" "$\"$INSTDIR\NotepadNext.exe$\" $\"%1$\""
	WriteRegStr SHCTX "Software\Classes\.txt\OpenWithProgids" "NotepadNext" ""
	# TODO: add more extensions

	WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section /o "Desktop Shortcut"
	CreateShortCut "$DESKTOP\Notepad Next.lnk" "$INSTDIR\NotepadNext.exe"
SectionEnd

Section /o "Start Menu Shortcut"
	CreateShortCut "$SMPROGRAMS\Notepad Next.lnk" "$INSTDIR\NotepadNext.exe"
SectionEnd

Section /o "Context Menu"
	SetRegView 64

	WriteRegStr SHCTX "Software\Classes\*\shell\NotepadNext" "" "Edit with Notepad Next"
	WriteRegStr SHCTX "Software\Classes\*\shell\NotepadNext" "icon" "$INSTDIR\NotepadNext.exe"
	WriteRegStr SHCTX "Software\Classes\*\shell\NotepadNext\command" "" "$\"$INSTDIR\NotepadNext.exe$\" $\"%1$\""
SectionEnd

Section "Uninstall"
	SetRegView 64
	RMDir /r $INSTDIR

	# Desktop shortcut
	Delete "$DESKTOP\Notepad Next.lnk"

	# Start Menu shortcut
	Delete "$SMPROGRAMS\Notepad Next.lnk"

	# Context menu registry
	DeleteRegKey SHCTX "Software\Classes\*\shell\NotepadNext"

	# Uninstall registry
	DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext"

	# Remove application registration
	DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\App Paths\NotepadNext.exe" "" "$INSTDIR\NotepadNext.exe"
	
	# Remove 'Open With' menu suggestion
	DeleteRegValue SHCTX "Software\Classes\.txt\OpenWithProgids" "NotepadNext"
	DeleteRegKey SHCTX "Software\Classes\NotepadNext"
SectionEnd
