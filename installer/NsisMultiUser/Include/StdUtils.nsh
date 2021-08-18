#################################################################################
# StdUtils plug-in for NSIS
# Copyright (C) 2004-2018 LoRd_MuldeR <MuldeR2@GMX.de>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# http://www.gnu.org/licenses/lgpl-2.1.txt
#################################################################################

# DEVELOPER NOTES:
# - Please see "https://github.com/lordmulder/stdutils/" for news and updates!
# - Please see "Docs\StdUtils\StdUtils.html" for detailed function descriptions!
# - Please see "Examples\StdUtils\StdUtilsTest.nsi" for usage examples!

#################################################################################
# FUNCTION DECLARTIONS
#################################################################################

!ifndef ___STDUTILS__NSH___
!define ___STDUTILS__NSH___

!define StdUtils.Time             '!insertmacro _StdU_Time'          #time(), as in C standard library
!define StdUtils.GetMinutes       '!insertmacro _StdU_GetMinutes'    #GetSystemTimeAsFileTime(), returns the number of minutes
!define StdUtils.GetHours         '!insertmacro _StdU_GetHours'      #GetSystemTimeAsFileTime(), returns the number of hours
!define StdUtils.GetDays          '!insertmacro _StdU_GetDays'       #GetSystemTimeAsFileTime(), returns the number of days
!define StdUtils.Rand             '!insertmacro _StdU_Rand'          #rand(), as in C standard library
!define StdUtils.RandMax          '!insertmacro _StdU_RandMax'       #rand(), as in C standard library, with maximum value
!define StdUtils.RandMinMax       '!insertmacro _StdU_RandMinMax'    #rand(), as in C standard library, with minimum/maximum value
!define StdUtils.RandList         '!insertmacro _StdU_RandList'      #rand(), as in C standard library, with list support
!define StdUtils.RandBytes        '!insertmacro _StdU_RandBytes'     #Generates random bytes, returned as Base64-encoded string
!define StdUtils.FormatStr        '!insertmacro _StdU_FormatStr'     #sprintf(), as in C standard library, one '%d' placeholder
!define StdUtils.FormatStr2       '!insertmacro _StdU_FormatStr2'    #sprintf(), as in C standard library, two '%d' placeholders
!define StdUtils.FormatStr3       '!insertmacro _StdU_FormatStr3'    #sprintf(), as in C standard library, three '%d' placeholders
!define StdUtils.ScanStr          '!insertmacro _StdU_ScanStr'       #sscanf(), as in C standard library, one '%d' placeholder
!define StdUtils.ScanStr2         '!insertmacro _StdU_ScanStr2'      #sscanf(), as in C standard library, two '%d' placeholders
!define StdUtils.ScanStr3         '!insertmacro _StdU_ScanStr3'      #sscanf(), as in C standard library, three '%d' placeholders
!define StdUtils.TrimStr          '!insertmacro _StdU_TrimStr'       #Remove whitspaces from string, left and right
!define StdUtils.TrimStrLeft      '!insertmacro _StdU_TrimStrLeft'   #Remove whitspaces from string, left side only
!define StdUtils.TrimStrRight     '!insertmacro _StdU_TrimStrRight'  #Remove whitspaces from string, right side only
!define StdUtils.RevStr           '!insertmacro _StdU_RevStr'        #Reverse a string, e.g. "reverse me" <-> "em esrever"
!define StdUtils.ValidFileName    '!insertmacro _StdU_ValidFileName' #Test whether string is a valid file name - no paths allowed
!define StdUtils.ValidPathSpec    '!insertmacro _StdU_ValidPathSpec' #Test whether string is a valid full(!) path specification
!define StdUtils.ValidDomainName  '!insertmacro _StdU_ValidDomain'   #Test whether string is a valid host name or domain name
!define StdUtils.StrToUtf8        '!insertmacro _StdU_StrToUtf8'     #Convert string from Unicode (UTF-16) or ANSI to UTF-8 bytes
!define StdUtils.StrFromUtf8      '!insertmacro _StdU_StrFromUtf8'   #Convert string from UTF-8 bytes to Unicode (UTF-16) or ANSI
!define StdUtils.SHFileMove       '!insertmacro _StdU_SHFileMove'    #SHFileOperation(), using the FO_MOVE operation
!define StdUtils.SHFileCopy       '!insertmacro _StdU_SHFileCopy'    #SHFileOperation(), using the FO_COPY operation
!define StdUtils.AppendToFile     '!insertmacro _StdU_AppendToFile'  #Append contents of an existing file to another file
!define StdUtils.ExecShellAsUser  '!insertmacro _StdU_ExecShlUser'   #ShellExecute() as NON-elevated user from elevated installer
!define StdUtils.InvokeShellVerb  '!insertmacro _StdU_InvkeShlVrb'   #Invokes a "shell verb", e.g. for pinning items to the taskbar
!define StdUtils.ExecShellWaitEx  '!insertmacro _StdU_ExecShlWaitEx' #ShellExecuteEx(), returns the handle of the new process
!define StdUtils.WaitForProcEx    '!insertmacro _StdU_WaitForProcEx' #WaitForSingleObject(), e.g. to wait for a running process
!define StdUtils.GetParameter     '!insertmacro _StdU_GetParameter'  #Get the value of a specific command-line option
!define StdUtils.TestParameter    '!insertmacro _StdU_TestParameter' #Test whether a specific command-line option has been set
!define StdUtils.ParameterCnt     '!insertmacro _StdU_ParameterCnt'  #Get number of command-line tokens, similar to argc in main()
!define StdUtils.ParameterStr     '!insertmacro _StdU_ParameterStr'  #Get the n-th command-line token, similar to argv[i] in main()
!define StdUtils.GetAllParameters '!insertmacro _StdU_GetAllParams'  #Get complete command-line, but without executable name
!define StdUtils.GetRealOSVersion '!insertmacro _StdU_GetRealOSVer'  #Get the *real* Windows version number, even on Windows 8.1+
!define StdUtils.GetRealOSBuildNo '!insertmacro _StdU_GetRealOSBld'  #Get the *real* Windows build number, even on Windows 8.1+
!define StdUtils.GetRealOSName    '!insertmacro _StdU_GetRealOSStr'  #Get the *real* Windows version, as a "friendly" name
!define StdUtils.GetOSEdition     '!insertmacro _StdU_GetOSEdition'  #Get the Windows edition, i.e. "workstation" or "server"
!define StdUtils.GetOSReleaseId   '!insertmacro _StdU_GetOSRelIdNo'  #Get the Windows release identifier (on Windows 10)
!define StdUtils.GetOSReleaseName '!insertmacro _StdU_GetOSRelIdStr' #Get the Windows release (on Windows 10), as a "friendly" name
!define StdUtils.VerifyOSVersion  '!insertmacro _StdU_VrfyRealOSVer' #Compare *real* operating system to an expected version number
!define StdUtils.VerifyOSBuildNo  '!insertmacro _StdU_VrfyRealOSBld' #Compare *real* operating system to an expected build number
!define StdUtils.HashText         '!insertmacro _StdU_HashText'      #Compute hash from text string (CRC32, MD5, SHA1/2/3, BLAKE2)
!define StdUtils.HashFile         '!insertmacro _StdU_HashFile'      #Compute hash from file (CRC32, MD5, SHA1/2/3, BLAKE2)
!define StdUtils.NormalizePath    '!insertmacro _StdU_NormalizePath' #Simplifies the path to produce a direct, well-formed path
!define StdUtils.GetParentPath    '!insertmacro _StdU_GetParentPath' #Get parent path by removing the last component from the path
!define StdUtils.SplitPath        '!insertmacro _StdU_SplitPath'     #Split the components of the given path
!define StdUtils.GetDrivePart     '!insertmacro _StdU_GetDrivePart'  #Get drive component of path
!define StdUtils.GetDirectoryPart '!insertmacro _StdU_GetDirPart'    #Get directory component of path
!define StdUtils.GetFileNamePart  '!insertmacro _StdU_GetFNamePart'  #Get file name component of path
!define StdUtils.GetExtensionPart '!insertmacro _StdU_GetExtnPart'   #Get file extension component of path
!define StdUtils.TimerCreate      '!insertmacro _StdU_TimerCreate'   #Create a new event-timer that will be triggered periodically
!define StdUtils.TimerDestroy     '!insertmacro _StdU_TimerDestroy'  #Destroy a running timer created with TimerCreate()
!define StdUtils.ProtectStr       '!insertmacro _StdU_PrtctStr'      #Protect a given String using Windows' DPAPI
!define StdUtils.UnprotectStr     '!insertmacro _StdU_UnprtctStr'    #Unprotect a string that was protected via ProtectStr()
!define StdUtils.GetLibVersion    '!insertmacro _StdU_GetLibVersion' #Get the current StdUtils library version (for debugging)
!define StdUtils.SetVerbose       '!insertmacro _StdU_SetVerbose'    #Enable or disable "verbose" mode (for debugging)


#################################################################################
# MACRO DEFINITIONS
#################################################################################

!macro _StdU_Time out
	StdUtils::Time /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetMinutes out
	StdUtils::GetMinutes /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetHours out
	StdUtils::GetHours /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetDays out
	StdUtils::GetDays /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_Rand out
	StdUtils::Rand /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_RandMax out max
	push ${max}
	StdUtils::RandMax /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_RandMinMax out min max
	push ${min}
	push ${max}
	StdUtils::RandMinMax /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_RandList count max
	push ${max}
	push ${count}
	StdUtils::RandList /NOUNLOAD
!macroend

!macro _StdU_RandBytes out count
	push ${count}
	StdUtils::RandBytes /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_FormatStr out format val
	push `${format}`
	push ${val}
	StdUtils::FormatStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_FormatStr2 out format val1 val2
	push `${format}`
	push ${val1}
	push ${val2}
	StdUtils::FormatStr2 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_FormatStr3 out format val1 val2 val3
	push `${format}`
	push ${val1}
	push ${val2}
	push ${val3}
	StdUtils::FormatStr3 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ScanStr out format input default
	push `${format}`
	push `${input}`
	push ${default}
	StdUtils::ScanStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ScanStr2 out1 out2 format input default1 default2
	push `${format}`
	push `${input}`
	push ${default1}
	push ${default2}
	StdUtils::ScanStr2 /NOUNLOAD
	pop ${out1}
	pop ${out2}
!macroend

!macro _StdU_ScanStr3 out1 out2 out3 format input default1 default2 default3
	push `${format}`
	push `${input}`
	push ${default1}
	push ${default2}
	push ${default3}
	StdUtils::ScanStr3 /NOUNLOAD
	pop ${out1}
	pop ${out2}
	pop ${out3}
!macroend

!macro _StdU_TrimStr var
	push ${var}
	StdUtils::TrimStr /NOUNLOAD
	pop ${var}
!macroend

!macro _StdU_TrimStrLeft var
	push ${var}
	StdUtils::TrimStrLeft /NOUNLOAD
	pop ${var}
!macroend

!macro _StdU_TrimStrRight var
	push ${var}
	StdUtils::TrimStrRight /NOUNLOAD
	pop ${var}
!macroend

!macro _StdU_RevStr var
	push ${var}
	StdUtils::RevStr /NOUNLOAD
	pop ${var}
!macroend

!macro _StdU_ValidFileName out test
	push `${test}`
	StdUtils::ValidFileName /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ValidPathSpec out test
	push `${test}`
	StdUtils::ValidPathSpec /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ValidDomain out test
	push `${test}`
	StdUtils::ValidDomainName /NOUNLOAD
	pop ${out}
!macroend


!macro _StdU_StrToUtf8 out str
	push `${str}`
	StdUtils::StrToUtf8 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_StrFromUtf8 out trnc str
	push ${trnc}
	push `${str}`
	StdUtils::StrFromUtf8 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_SHFileMove out from to hwnd
	push `${from}`
	push `${to}`
	push ${hwnd}
	StdUtils::SHFileMove /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_SHFileCopy out from to hwnd
	push `${from}`
	push `${to}`
	push ${hwnd}
	StdUtils::SHFileCopy /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_AppendToFile out from dest offset maxlen
	push `${from}`
	push `${dest}`
	push ${offset}
	push ${maxlen}
	StdUtils::AppendToFile /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ExecShlUser out file verb args
	push `${file}`
	push `${verb}`
	push `${args}`
	StdUtils::ExecShellAsUser /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_InvkeShlVrb out path file verb_id
	push "${path}"
	push "${file}"
	push ${verb_id}
	StdUtils::InvokeShellVerb /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ExecShlWaitEx out_res out_val file verb args
	push `${file}`
	push `${verb}`
	push `${args}`
	StdUtils::ExecShellWaitEx /NOUNLOAD
	pop ${out_res}
	pop ${out_val}
!macroend

!macro _StdU_WaitForProcEx out handle
	push `${handle}`
	StdUtils::WaitForProcEx /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetParameter out name default
	push `${name}`
	push `${default}`
	StdUtils::GetParameter /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_TestParameter out name
	push `${name}`
	StdUtils::TestParameter /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ParameterCnt out
	StdUtils::ParameterCnt /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_ParameterStr out index
	push ${index}
	StdUtils::ParameterStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetAllParams out truncate
	push `${truncate}`
	StdUtils::GetAllParameters /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetRealOSVer out_major out_minor out_spack
	StdUtils::GetRealOsVersion /NOUNLOAD
	pop ${out_major}
	pop ${out_minor}
	pop ${out_spack}
!macroend

!macro _StdU_GetRealOSBld out
	StdUtils::GetRealOsBuildNo /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetRealOSStr out
	StdUtils::GetRealOsName /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_VrfyRealOSVer out major minor spack
	push `${major}`
	push `${minor}`
	push `${spack}`
	StdUtils::VerifyRealOsVersion /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_VrfyRealOSBld out build
	push `${build}`
	StdUtils::VerifyRealOsBuildNo /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetOSEdition out
	StdUtils::GetOsEdition /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetOSRelIdNo out
	StdUtils::GetOsReleaseId /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetOSRelIdStr out
	StdUtils::GetOsReleaseName /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_HashText out type text
	push `${type}`
	push `${text}`
	StdUtils::HashText /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_HashFile out type file
	push `${type}`
	push `${file}`
	StdUtils::HashFile /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_NormalizePath out path
	push `${path}`
	StdUtils::NormalizePath /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetParentPath out path
	push `${path}`
	StdUtils::GetParentPath /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_SplitPath out_drive out_dir out_fname out_ext path
	push `${path}`
	StdUtils::SplitPath /NOUNLOAD
	pop ${out_drive}
	pop ${out_dir}
	pop ${out_fname}
	pop ${out_ext}
!macroend

!macro _StdU_GetDrivePart out path
	push `${path}`
	StdUtils::GetDrivePart /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetDirPart out path
	push `${path}`
	StdUtils::GetDirectoryPart /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetFNamePart out path
	push `${path}`
	StdUtils::GetFileNamePart /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetExtnPart out path
	push `${path}`
	StdUtils::GetExtensionPart /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_TimerCreate out callback interval
	GetFunctionAddress ${out} ${callback}
	push ${out}
	push ${interval}
	StdUtils::TimerCreate /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_TimerDestroy out timer_id
	push ${timer_id}
	StdUtils::TimerDestroy /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_PrtctStr out dpsc salt text
	push `${dpsc}`
	push `${salt}`
	push `${text}`
	StdUtils::ProtectStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_UnprtctStr out trnc salt data
	push `${trnc}`
	push `${salt}`
	push `${data}`
	StdUtils::UnprotectStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdU_GetLibVersion out_ver out_tst
	StdUtils::GetLibVersion /NOUNLOAD
	pop ${out_ver}
	pop ${out_tst}
!macroend

!macro _StdU_SetVerbose enable
	Push ${enable}
	StdUtils::SetVerboseMode /NOUNLOAD
!macroend


#################################################################################
# MAGIC NUMBERS
#################################################################################

!define StdUtils.Const.ShellVerb.PinToTaskbar     0
!define StdUtils.Const.ShellVerb.UnpinFromTaskbar 1
!define StdUtils.Const.ShellVerb.PinToStart       2
!define StdUtils.Const.ShellVerb.UnpinFromStart   3

!endif # !___STDUTILS__NSH___
