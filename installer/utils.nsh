# Stolen from https://github.com/Drizin/NsisMultiUser/blob/f6a4865a6045ca1334c099359b23e2b7342fa7de/Demos/Common/Utils.nsh

!define ERROR_ALREADY_EXISTS 0x000000b7
!define ERROR_ACCESS_DENIED 0x5

!macro CheckSingleInstanceFunc UNINSTALLER_PREFIX
	; parameters:
	; $0 - TYPE - "Setup" or "Application"
	; $1 - SCOPE - "Global" or "Local" (default if empty)
	; $2 - MUTEX_NAME - unique mutex name
	Function ${UNINSTALLER_PREFIX}CheckSingleInstance
		Push $3
		Push $4
		Push $5

		${if} $1 == ""
			StrCpy $1 "Local"
		${endif}

		${if} $0 == "Setup"
			StrCpy $5 "The setup of ${PRODUCT_NAME}"
		${else}
			StrCpy $5 "${PRODUCT_NAME}"
		${endif}

		try:
		System::Call 'kernel32::CreateMutex(i 0, i 0, t "$1\$2") i .r3 ?e'
		Pop $4 ; the stack contains the result of GetLastError

		${if} $0 == "Application"
		    ${andif} $3 <> 0
			System::Call 'kernel32::CloseHandle(i $3)' ; close the Application mutex
		${endif}

		${if} $4 = ${ERROR_ALREADY_EXISTS}
			${orif} $4 = ${ERROR_ACCESS_DENIED}	; ERROR_ACCESS_DENIED means the mutex was created by another user and we don't have access to open it, so application is running
			; will display NSIS taskbar button, no way to hide it before GUIInit, $HWNDPARENT is 0
			MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "$5 is already running.$\r$\n\
				Please, close all instances of it and click Retry to continue, or Cancel to exit." /SD IDCANCEL IDCANCEL cancel
			System::Call 'kernel32::CloseHandle(i $3)' ; for next CreateMutex call to succeed
			Goto try

			cancel:
			Quit ; will SetErrorLevel 2 - Installation aborted by script
		${endif}

		Pop $5
		Pop $4
		Pop $3
	FunctionEnd
!macroend

!macro CheckSingleInstance TYPE SCOPE MUTEX_NAME
	Push $0
	Push $1
	Push $2
	
	StrCpy $0 "${TYPE}"
	StrCpy $1 "${SCOPE}"
	StrCpy $2 "${MUTEX_NAME}"
	!ifndef __UNINSTALL__
		Call CheckSingleInstance
	!else
		Call un.CheckSingleInstance
	!endif
	
	Pop $2
	Pop $1
	Pop $0
!macroend

!insertmacro CheckSingleInstanceFunc ""