local L = {}

L.lexer = "verilog"

L.singleLineComment = "// "

L.extensions = {
	"v",
	"sv",
	"vh",
	"svh",
}

L.keywords = {
	[0] = "accept_on alias always always_comb always_ff always_latch and assert assign assume attribute automatic before begin bind bins binsof bit break buf bufif0 bufif1 byte case casex casez cell chandle checker class clocking cmos config const constraint context continue cover covergroup coverpoint cross deassign default defparam design disable dist do edge else end endattribute endcase endchecker endclass endclocking endconfig endfunction endgenerate endgroup endinterface endmodule endpackage endprimitive endprogram endproperty endsequence endspecify endtable endtask enum event eventually expect export extends extern final first_match for force foreach forever fork forkjoin function generate genvar global highz0 highz1 if iff ifnone ignore_bins illegal_bins implements implies import incdir include initial inout input inside instance int integer interconnect interface intersect join join_any join_none large let liblist library local localparam logic longint macromodule matches medium modport module nand negedge nettype new nexttime nmos nor noshowcancelled not notif0 notif1 null or output package packed parameter pmos posedge primitive priority program property protected pull0 pull1 pulldown pullup pulsestyle_ondetect pulsestyle_onevent pure rand randc randcase randsequence rcmos real realtime ref reg reject_on release repeat restrict return rnmos rpmos rtran rtranif0 rtranif1 scalared sequence shortint shortreal showcancelled signed small soft solve specify specparam static string strong strong0 strong1 struct super supply0 supply1 sync_accept_on sync_reject_on s_always s_eventually s_nexttime s_until s_until_with table tagged task this throughout time timeprecision timeunit tran tranif0 tranif1 tri tri0 tri1 triand trior trireg type typedef union unique unique0 unsigned until until_with untyped use var vectored virtual void wait wait_order wand weak weak0 weak1 while wildcard wire with within wor xnor xor",
	[1] = "$acos $acosh $asin $asinh $assertcontrol $assertkill $assertoff $asserton $assertpasson $assertpassoff $assertfailon $assertfailoff $assertnonvacuouson $assertvacuousoff $async$and$array $async$and$plane $async$nand$array $async$nand$plane $async$nor$array $async$nor$plane $async$or$array $async$or$plane $atan $atan2 $atanh $bits $bitstoreal $bitstoshortreal $cast $ceil $changed_gclk $changing_gclk $clog2 $comment $cos $cosh $countdrivers $countones $coverage_control $coverage_get $coverage_get_max $coverage_merge $coverage_save $date $dimensions $display $displayb $displayh $displayo $dist_chi_square $dist_erlang $dist_exponential $dist_normal $dist_poisson $dist_t $dist_uniform $dumpall $dumpfile $dumpflush $dumplimit $dumpoff $dumpon $dumpportsall $dumpportsflush $dumpportslimit $dumpportsoff $dumpportson $dumpvars $end $enddefinitions $error $exit $exp $falling_gclk $fatal $fclose $fdisplay $fdisplayb $fdisplayh $fdisplayo $fell $fell_gclk $feof $ferror $fflush $fgetc $fgets $finish $floor $fmonitor $fmonitorb $fmonitorh $fmonitoro $fopen $fread $fscanf $fseek $fstrobe $ftell $fullskew $future_gclk $fwrite $fwriteb $fwriteh $fwriteo $getpattern $get_coverage $high $history $hold $hypot $increment $incsave $info $input $isunbounded $isunknown $itor $key $left $list $ln $load_coverage_db $log $log10 $low $monitor $monitorb $monitorh $monitoro $monitoroff $monitoron $nochange $nokey $nolog $onehot $onehot0 $past $past_gclk $period $pow $printtimescale $q_add $q_exam $q_full $q_initialize $q_remove $random $readmemb $readmemh $realtime $realtobits $recovery $recrem $removal $reset $reset_count $reset_value $restart $rewind $right $rising_gclk $root $rose $rose_gclk $rtoi $sampled $save $scale $scope $setup $setuphold $set_coverage_db_name $sformat $sformatf $shortrealtobits $showscopes $showvariables $showvars $signed $sin $sinh $size $skew $sqrt $sreadmemb $sreadmemh $sscanf $stable $stable_gclk $steady_gclk $stime $stop $strobe $strobeb $strobeh $strobeo $swrite $sync$and$array $sync$and$plane $sync$nand$array $sync$nand$plane $sync$nor$array $sync$nor$plane $sync$or$array $sync$or$plane $system $tan $tanh $test$plusargs $time $timeformat $timescale $timeskew $typename $typeof $uandom $ungetc $unit $unpacked_dimensions $unsigned $upscope $urandom_range $value$plusargs $var $vcdclose $version $warning $width $write $writeb $writeh $writememb $writememh $writeo",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 11,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["TAGNAME"] = {
		id = 2,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD"] = {
		id = 7,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
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
	["COMMENT LINE BANG"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["USER"] = {
		id = 19,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
