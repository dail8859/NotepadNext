local L = {}

L.lexer = "sql"

L.singleLineComment = "-- "

L.extensions = {
	"sql",
}

L.keywords = {
	[0] = "abs absolute access acos action add add_months adddate admin after aggregate all allocate alter and any app_name applock_mode applock_test are array as asc ascii asin assemblyproperty assertion asymkey_id at atan atn2 audit authid authorization autonomous_transaction avg backup before begin benchmark between bfilename bin binary_checksum bit_and bit_count bit_or body both breadth break browse bulk by call cascade cascaded case cast catalog catch ceil ceiling cert_id certproperty character charindex chartorowid check checkpoint checksum checksum_agg chr class clause close cluster clustered coalesce col_length col_name collate collation collect column columnproperty columns_updated comment commit completion compress compute concat concat_ws connect connection connectionproperty constant constraint constraints constructorcreate contains containsable containstable context_info continue conv convert corr corresponding cos cot count count_big covar_pop covar_samp create cross cube cume_dist current current_date current_path current_request_id current_role current_time current_timestamp current_user currval cursor_status cycle data database database_principal_id databasepropertyex datalength date_add date_format date_sub dateadd datediff datefirst dateformat datename datepart db_id db_name dbcc deallocate declare decode decryptbyasmkey decryptbycert decryptbykey decryptbykeyautocert decryptbypassphrase default deferrable deferred degrees delete dense_rank deny depth deref desc describe descriptor destroy destructor deterministic diagnostics dictionary difference disconnect disk distinct distributed do domain drop dump dynamic each else elsif empth encode encrypt encryptbyasmkey encryptbycert encryptbykey encryptbypassphrase end end-exec equals errlvl error_line error_message error_number error_procedure error_severity error_state escape eventdata every except exception exclusive exec execute exists exit exp export_set extends external extract false fetch file file_id file_idex file_name filegroup_id filegroup_name filegroupproperty fileproperty fillfactor first first_value floor fn_virtualfilestats for forall foreign format formatmessage found freetext freetexttable from from_days full fulltextcatalog fulltextcatalogproperty fulltextservice fulltextserviceproperty function general get get_filestream_transaction_context get_lock getansinull getdate getutcdate global go goto grant greatest group grouping grouping_id has_perms_by_name hashbytes having heap hex hextoraw holdlock host host_id host_name hour ident_current ident_incr ident_seed identified identity identity_insert identitycol if ifnull ignore immediate in include increment index index_col indexkey_property indexproperty indicator initcap initial initialize initially inner inout input insert instr instrb interface intersect into is is_member is_null is_numeric is_srvrolemember isdate isnull isnumeric isolation iterate java join key key_guid key_id kill lag language large last last_day last_value lateral lcase lead leading least left len length lengthb less level like limit limited lineno ln load local localtime localtimestamp locator lock log log10 loop lower lpad ltrim make_ref map match max maxextents merge mid min min_active_rowversion minus minute mlslabel mod mode modifies modify module month months_between names national natural naturaln new new_time newid newsequentialid next next_day nextval no noaudit nocheck nocompress nocopy nonclustered none not nowait ntile null nullif number_base nvl nvl2 object object_definition object_id object_name object_property object_schema_name objectproperty objectpropertyex ocirowid oct of off offline offsets old on online only opaque open opendatasource openquery openrowset openxml operation operator option or ord order ordinalityorganization original_db_name original_login others out outer output over package pad parameter parameters parsename partial partition path pathname patindex pctfree percent percent_rank permissions pi pivot plan pls_integer positive positiven postfix pow power pragma prefix preorder prepare preserve primary print prior private privileges proc procedure public publishingservername pwdcompare pwdencrypt quotename radians raise raiserror rand range rank ratio_to_export rawtohex read reads readtext reconfigure record recursive ref references referencing reftohex relative release release_lock rename repeat replace replicate replication resource restore restrict result return returns reverse revert revoke right rollback rollup round routine row row_number rowcount rowcount_big rowguidcol rowidtochar rowlabel rownum rows rowtype rpad rtrim rule save savepoint schema schema_id schema_name scope scope_identity scroll search second section securityaudit seddev_samp select separate sequence serverproperty session session_user set sets setuser share shutdown sign signbyasymkey signbycert sin sinh size some soundex sp_helplanguage space specific specifictype sql sql_variant_property sqlcode sqlerrm sqlexception sqlstate sqlwarning sqrt square start state statement static statistics stats_date std stddev stdev stdev_pop stdevp str strcmp structure stuff subdate substr substrb substring substring_index subtype successful sum suser_id suser_name suser_sid suser_sname switchoffset synonym sys_context sys_guid sysdate sysdatetime sysdatetimeoffset system_user sysutcdatetime table tablesample tan tanh temporary terminate textptr textsize textvalid than then timezone_abbr timezone_hour timezone_minute timezone_region to to_char to_date to_days to_number to_single_byte todatetimeoffset top trailing tran transaction translate translation treat trigger trigger_nestlevel trim true trunc truncate try tsequal type type_id type_name typeproperty ucase uid under unicode union unique unknown unnest unpivot update updatetext upper usage use user user_id user_name userenv using validate value values var var_pop var_samp variable variance varp varying verifysignedbyasmkey verifysignedbycert view vsize waitfor when whenever where while with without work write writetext xact_state zone",
	[1] = "None",
	[4] = "bfile bigint binary binary_integer bit blob bool boolean char char_base clob cursor date datetime datetime2 datetimeoffset day dec decimal double enum float hierarchyid image int integer interval long longblob longtext mediumblob mediumint mediumtext money nchar nclob ntext number numeric nvarchar precision raw real rowid smalldatetime smallint smallmoney sql_variant text time timestamp tinyblob tinyint tinytext uniqueidentifier urowid varbinary varchar varchar2 xml year",
}

L.styles = {
	["KEYWORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["USER1"] = {
		id = 16,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD2"] = {
		id = 19,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING2"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["Q OPERATOR"] = {
		id = 24,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
