!include "MUI.nsh"
!include "FileFunc.nsh"
!include "UninstallExisting.nsh"

SetCompressor /SOLID lzma

!define MUI_ABORTWARNING # This will warn the user if they exit from the installer.
!define MUI_ICON "..\src\NotepadNext\icons\newfile.ico"
!define MUI_UNICON "..\src\NotepadNext\icons\newfile.ico"
!define MUI_COMPONENTSPAGE_NODESC

!getdllversion "..\build\package\NotepadNext.exe" nnver_
!define APPVERSION ${nnver_1}.${nnver_2}
!define VERSION_MAJOR ${nnver_1}
!define VERSION_MINOR ${nnver_2}
!define VERSION_PATCH ${nnver_3}

!if ${VERSION_PATCH} != 0
	!undef APPVERSION
	!define APPVERSION ${nnver_1}.${nnver_2}.${nnver_3}
!endif

# Install pages
!insertmacro MUI_PAGE_WELCOME # Welcome to the installer page.
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY # In which folder install page.
!insertmacro MUI_PAGE_INSTFILES # Installing page.
!insertmacro MUI_PAGE_FINISH # Finished installation page.

# Uninstall pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


!insertmacro MUI_LANGUAGE "English"


Name "Notepad Next v${APPVERSION}" # Name of the installer (usually the name of the application to install).
OutFile "NotepadNext-v${APPVERSION}-Installer.exe" # Name of the installer's file.
InstallDir "$PROGRAMFILES64\NotepadNext" # Default installing folder ($PROGRAMFILES is Program Files folder).
ShowInstDetails show # This will always show the installation details.
RequestExecutionLevel admin
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

	# Check to see if there is uninstaller information
	ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "UninstallString"
	${If} $0 != ""
		ReadRegStr $1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayVersion"
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
FunctionEnd

Section "Notepad Next"
	SectionIn RO
	SetOutPath $INSTDIR

	File /r ..\build\package\*

	SetRegView 64

	# Installation info to show up in the Add/Remove panel
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayName" "Notepad Next ${APPVERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "Publisher" "Notepad Next"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayVersion" "${APPVERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "DisplayIcon" "$INSTDIR\NotepadNext.exe"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "NoRepair" 1
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"

	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IfErrors +3 0
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext" "EstimatedSize" "$0"

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

	WriteRegStr HKCR "*\shell\NotepadNext" "" "Edit with Notepad Next"
	WriteRegStr HKCR "*\shell\NotepadNext" "icon" "$INSTDIR\NotepadNext.exe"
	WriteRegStr HKCR "*\shell\NotepadNext\command" "" "$INSTDIR\NotepadNext.exe $\"%1$\""
SectionEnd

Section "Uninstall"
	SetRegView 64
	RMDir /r $INSTDIR

	# Desktop shortcut
	Delete "$DESKTOP\Notepad Next.lnk"

	# Start Menu shortcut
	Delete "$SMPROGRAMS\Notepad Next.lnk"

	# Context menu registry
	DeleteRegKey HKCR "*\shell\notepadnext"

	# Uninstall registry
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NotepadNext"
SectionEnd
