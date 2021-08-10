# https://nsis.sourceforge.io/Auto-uninstall_old_before_installing_new

!macro UninstallExisting exitcode uninstcommand
	Push `${uninstcommand}`
	Call UninstallExisting
	Pop ${exitcode}
!macroend

Function UninstallExisting
	Exch $1 ; uninstcommand
	Push $2 ; Uninstaller
	Push $3 ; Len
	StrCpy $3 ""
	StrCpy $2 $1 1
	StrCmp $2 '"' qloop sloop
	sloop:
		StrCpy $2 $1 1 $3
		IntOp $3 $3 + 1
		StrCmp $2 "" +2
		StrCmp $2 ' ' 0 sloop
		IntOp $3 $3 - 1
		Goto run
	qloop:
		StrCmp $3 "" 0 +2
		StrCpy $1 $1 "" 1 ; Remove initial quote
		IntOp $3 $3 + 1
		StrCpy $2 $1 1 $3
		StrCmp $2 "" +2
		StrCmp $2 '"' 0 qloop
	run:
		StrCpy $2 $1 $3 ; Path to uninstaller
		StrCpy $1 161 ; ERROR_BAD_PATHNAME
		GetFullPathName $3 "$2\.." ; $InstDir
		IfFileExists "$2" 0 +4
		ExecWait '"$2" /S _?=$3' $1 ; This assumes the existing uninstaller is a NSIS uninstaller, other uninstallers don't support /S nor _?=
		IntCmp $1 0 "" +2 +2 ; Don't delete the installer if it was aborted
		Delete "$2" ; Delete the uninstaller
		RMDir "$3" ; Try to delete $InstDir
		RMDir "$3\.." ; (Optional) Try to delete the parent of $InstDir
	Pop $3
	Pop $2
	Exch $1 ; exitcode
FunctionEnd