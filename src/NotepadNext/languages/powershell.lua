local L = {}

L.lexer = "powershell"

L.extensions = {
	"ps1",
	"psm1",
}

L.keywords = {
	[0] = "break continue do else elseif filter for foreach function if in return switch until where while",
	[1] = "add-content add-history add-member add-pssnapin clear-content clear-item clear-itemproperty clear-variable compare-object convertfrom-securestring convert-path convertto-html convertto-securestring copy-item copy-itemproperty export-alias export-clixml export-console export-csv foreach-object format-custom format-list format-table format-wide get-acl get-alias get-authenticodesignature get-childitem get-command get-content get-credential get-culture get-date get-eventlog get-executionpolicy get-help get-history get-host get-item get-itemproperty get-location get-member get-pfxcertificate get-process get-psdrive get-psprovider get-pssnapin get-service get-tracesource get-uiculture get-unique get-variable get-wmiobject group-object import-alias import-clixml import-csv invoke-expression invoke-history invoke-item join-path measure-command measure-object move-item move-itemproperty new-alias new-item new-itemproperty new-object new-psdrive new-service new-timespan new-variable out-default out-file out-host out-null out-printer out-string pop-location push-location read-host remove-item remove-itemproperty remove-psdrive remove-pssnapin remove-variable rename-item rename-itemproperty resolve-path restart-service resume-service select-object select-string set-acl set-alias set-authenticodesignature set-content set-date set-executionpolicy set-item set-itemproperty set-location set-psdebug set-service set-tracesource set-variable sort-object split-path start-service start-sleep start-transcript stop-process stop-service stop-transcript suspend-service tee-object test-path trace-command update-formatdata update-typedata where-object write-debug write-error write-host write-output write-progress write-verbose write-warning",
	[2] = "ac asnp clc cli clp clv cpi cpp cvpa diff epal epcsv fc fl foreach ft fw gal gc gci gcm gdr ghy gi gl gm gp gps group gsv gsnp gu gv gwmi iex ihy ii ipal ipcsv mi mp nal ndr ni nv oh rdr ri rni rnp rp rsnp rv rvpa sal sasv sc select si sl sleep sort sp spps spsv sv tee where write cat cd clear cp h history kill lp ls mount mv popd ps pushd pwd r rm rmdir echo cls chdir copy del dir erase move rd ren set type",
	[5] = "component description example externalhelp forwardhelpcategory forwardhelptargetname functionality inputs link notes outputs parameter remotehelprunspace role synopsis",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 2,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["CHARACTER"] = {
		id = 3,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["VARIABLE"] = {
		id = 5,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["OPERATOR"] = {
		id = 6,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["INSTRUCTION WORD"] = {
		id = 8,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["CMDLET"] = {
		id = 9,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["ALIAS"] = {
		id = 10,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT STREAM"] = {
		id = 13,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["HERE STRING"] = {
		id = 14,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["HERE CHARACTER"] = {
		id = 15,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC KEYWORD"] = {
		id = 16,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
}
return L
