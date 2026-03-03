local L = {}

L.lexer = "nsis"

L.singleLineComment = "; "

L.extensions = {
	"nsi",
	"nsh",
}

L.keywords = {
	[0] = "Abort AddBrandingImage AddSize AllowRootDirInstall AllowSkipFiles AutoCloseWindow BGFont BGGradient BrandingText BringToFront Call CallInstDLL Caption ChangeUI CheckBitmap ClearErrors CompletedText ComponentText CopyFiles CRCCheck CreateDirectory CreateFont CreateShortCut Delete DeleteINISec DeleteINIStr DeleteRegKey DeleteRegValue DetailPrint DetailsButtonText DirText DirVar DirVerify EnableWindow EnumRegKey EnumRegValue Exch Exec ExecShell ExecWait ExpandEnvStrings File FileBufSize FileClose FileErrorText FileOpen FileRead FileReadByte FileReadUTF16LE FileSeek FileWrite FileWriteByte FileWriteUTF16LE FindClose FindFirst FindNext FindWindow FlushINI Function FunctionEnd GetCurInstType GetCurrentAddress GetDlgItem GetDLLVersion GetDLLVersionLocal GetErrorLevel GetExeName GetExePath GetFileTime GetFileTimeLocal GetFullPathName GetFunctionAddress GetInstDirError GetLabelAddress GetTempFileName Goto HideWindow Icon IfAbort IfErrors IfFileExists IfRebootFlag IfSilent InitPluginsDir InstallButtonText InstallColors InstallDir InstallDirRegKey InstProgressFlags InstType InstTypeGetText InstTypeSetText IntCmp IntCmpU IntFmt IntOp IsWindow LangString LangStringUP LicenseBkColor LicenseData LicenseForceSelection LicenseLangString LicenseText LoadLanguageFile LockWindow LogSet LogText ManifestDPIAware ManifestSupportedOS MessageBox MiscButtonText Nop Name OutFile Page PageEx PageExEnd PluginDir Pop Push Quit ReadEnvStr ReadINIStr ReadRegDWORD ReadRegStr Reboot RegDLL Rename RequestExecutionLevel ReserveFile Return RMDir SearchPath Section SectionEnd SectionGetFlags SectionGetInstTypes SectionGetSize SectionGetText SectionGroup SectionGroupEnd SectionIn SectionSetFlags SectionSetInstTypes SectionSetSize SectionSetText SendMessage SetAutoClose SetBrandingImage SetCompress SetCompressionLevel SetCompressor SetCompressorDictSize SetCtlColors SetCurInstType SetDatablockOptimize SetDateSave SetDetailsPrint SetDetailsView SetErrorLevel SetErrors SetFileAttributes SetFont SetOutPath SetOverwrite SetPluginUnload SetRebootFlag SetRegView SetShellVarContext SetSilent SetStaticBkColor ShowInstDetails ShowUninstDetails ShowWindow SilentInstall SilentUnInstall Sleep SpaceTexts StrCmp StrCmpS StrCpy StrLen SubSection SubSectionEnd Unicode UninstallButtonText UninstallCaption UninstallIcon UninstallSubCaption UninstallText UninstPage UnRegDLL UnsafeStrCpy Var VIAddVersionKey VIFileVersion VIProductVersion WindowIcon WriteINIStr WriteRegBin WriteRegDWORD WriteRegExpandStr WriteRegStr WriteUninstaller XPStyle !AddIncludeDir !AddPluginDir !appendfile !cd !define !delfile !echo !else !endif !error !execute !finalize !getdllversion !if !ifdef !ifmacrodef !ifmacrondef !ifndef !include !insertmacro !macro !macroend !macroundef !packhdr !searchparse !searchreplace !system !tempfile !undef !verbose !warning",
	[1] = "$0 $1 $2 $3 $4 $5 $6 $7 $8 $9 $R0 $R1 $R2 $R3 $R4 $R5 $R6 $R7 $R8 $R9 $ADMINTOOLS $APPDATA $CDBURN_AREA $CMDLINE $COMMONFILES $COMMONFILES32 $COMMONFILES64 $COOKIES $DESKTOP $DOCUMENTS $EXEDIR $EXEFILE $EXEPATH $FAVORITES $FONTS $HISTORY $HWNDPARENT $INTERNET_CACHE $INSTDIR $LANGUAGE $LOCALAPPDATA $MUSIC $NETHOOD ${NSISDIR} $OUTDIR $PICTURES $PLUGINSDIR $PRINTHOOD $PROFILE $PROGRAMFILES $PROGRAMFILES32 $PROGRAMFILES64 $QUICKLAUNCH $RECENT $RESOURCES $RESOURCES_LOCALIZED $SENDTO $SMPROGRAMS $SMSTARTUP $STARTMENU $SYSDIR $TEMP $TEMPLATES $VIDEOS $WINDIR $$ $\n $\r $\t",
	[2] = "ARCHIVE CUR END FILE_ATTRIBUTE_ARCHIVE FILE_ATTRIBUTE_HIDDEN FILE_ATTRIBUTE_NORMAL FILE_ATTRIBUTE_OFFLINE FILE_ATTRIBUTE_READONLY FILE_ATTRIBUTE_SYSTEM FILE_ATTRIBUTE_TEMPORARY HIDDEN HKCC HKCR HKCU HKDD HKEY_CLASSES_ROOT HKEY_CURRENT_CONFIG HKEY_CURRENT_USER HKEY_DYN_DATA HKEY_LOCAL_MACHINE HKEY_PERFORMANCE_DATA HKEY_USERS HKLM HKPD HKU IDABORT IDCANCEL IDIGNORE IDNO IDOK IDRETRY IDYES MB_ABORTRETRYIGNORE MB_DEFBUTTON1 MB_DEFBUTTON2 MB_DEFBUTTON3 MB_DEFBUTTON4 MB_ICONEXCLAMATION MB_ICONINFORMATION MB_ICONQUESTION MB_ICONSTOP MB_OK MB_OKCANCEL MB_RETRYCANCEL MB_RIGHT MB_SETFOREGROUND MB_TOPMOST MB_USERICON MB_YESNO MB_YESNOCANCEL NORMAL OFFLINE READONLY SET SHCTX SW_HIDE SW_SHOWMAXIMIZED SW_SHOWMINIMIZED SW_SHOWNORMAL SYSTEM TEMPORARY all auto both bottom bzip2 checkbox colored current false force hide ifdiff ifnewer lastused leave left listonly lzma nevershow none normal off on pop push radiobuttons right show silent silentlog smooth textonly top true try zlib",
	[3] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINE"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING DOUBLE QUOTE"] = {
		id = 2,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xEEEEEE),
	},
	["STRING LEFT QUOTE"] = {
		id = 3,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xC0C0C0),
	},
	["STRING RIGHT QUOTE"] = {
		id = 4,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xC0C0C0),
	},
	["FUNCTION"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["VARIABLE"] = {
		id = 6,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["LABEL"] = {
		id = 7,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFF80),
	},
	["USER DEFINED"] = {
		id = 8,
		fgColor = rgb(0xFDFFEC),
		bgColor = rgb(0xFF80FF),
		fontStyle = 4,
	},
	["SECTION"] = {
		id = 9,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["SUBSECTION"] = {
		id = 10,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["IF DEFINE"] = {
		id = 11,
		fgColor = rgb(0x808040),
		bgColor = rgb(0xFFFFFF),
	},
	["MACRO"] = {
		id = 12,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STRING VAR"] = {
		id = 13,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xEFEFEF),
	},
	["NUMBER"] = {
		id = 14,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFFFF),
	},
	["SECTION GROUP"] = {
		id = 15,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PAGE EX"] = {
		id = 16,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["FUNCTION DEFINITIONS"] = {
		id = 17,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 18,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
