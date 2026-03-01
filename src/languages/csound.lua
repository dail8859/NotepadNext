local L = {}

L.lexer = "csound"

L.singleLineComment = "; "

L.extensions = {
	"orc",
	"sco",
	"csd",
}

L.keywords = {
	[0] = "a i db in or zr Add Dec Div Inc Mul Sub abs and cos dam dec div exp fin fof fog inh ino inq ins int inx inz lfo log mac mod mul not out pan pow rms rnd shl sin sqr sub sum tab tan tb0 tb1 tb2 tb3 tb4 tb5 tb6 tb7 tb8 tb9 urd vco xin xor zar zaw zir ziw zkr zkw adsr babo buzz cent clip comb cosh diff divz fini fink fmb3 fof2 fold fout frac ftsr gain goto in32 inch init line maca moog mute nrpn outc  outh outo outq outs outx outz peak port pset pvoc rand seed sinh  sqrt stix tabw tanh tb10 tb11 tb12 tb13 tb14 tb15 tone vadd vco2  vdiv vexp vibr vmap vmul vpow wrap xout xyin zacl zarg zawm ziwm  zkcl zkwm FLbox FLjoy FLrun adsyn ampdb atone birnd bqrez butbp  butbr buthp butlp clear ctrl7 dbamp dconv delay dumpk endin endop  event expon fouti foutk ftgen ftlen gauss gbuzz grain guiro igoto  ihold instr integ kgoto limit linen log10 loopg loopl lpf18 madsr  max_k metro noise nsamp oscil out32 outch outic outkc outq1 outq2  outq3 outq4 outs1 outs2 pareq pitch pluck portk print pvadd randh  randi rbjeq readk reson rezzy rnd31 scans scanu sense space tab_i  table tbvcf tempo timek times tival tonek tonex vaddv vbap4 vbap8  vbapz vcomb vcopy vdecr vdivv veloc vexpv vibes vincr vmult voice  vport vpowv vpvoc vsubv vwrap wgbow xadsr zamod zkmod FLhide FLkeyb  FLknob FLpack FLshow FLtabs FLtext active adsynt alpass areson  atonek atonex bamboo bbcutm bbcuts biquad cabasa cauchy cggoto  cigoto ckgoto clfilt cngoto convle cosinv cpsoct cpspch cpstun  cpuprc cross2 crunch ctrl14 ctrl21 delay1 delayk delayr delayw  deltap denorm diskin dumpk2 dumpk3 dumpk4 envlpx expseg filesr  fiopen fmbell follow foscil foutir ftlen2 ftload ftmorf ftsave  grain2 grain3 harmon hrtfer initc7 interp jitter linenr lineto  linseg locsig loopge loople lorenz loscil lowres lpread lpslot  mandel mandol mclock mdelay midic7 midiin midion mirror moscil  mpulse mrtmsg mxadsr nlfilt noteon notnum ntrpol octave octcps  octpch opcode oscbnk oscil1 oscil3 oscili osciln oscils oscilx  outiat outipb outipc outkat outkpb outkpc pchoct phasor planet  poscil printk prints pvread pvsftr pvsftw random readk2 readk3  readk4 reinit resonk resonr resonx resony resonz reverb rigoto  s16b14 s32b14 sekere sfload sfplay shaker sininv spat3d spdist  spsend strset table3 tablei tablew tabw_i taninv tigoto timout  turnon upsamp vbap16 vcella vco2ft vdel_k vdelay vlimit vmultv  vrandh vrandi wgclar xscans xscanu FLcolor FLcount FLgroup FLlabel  FLpanel FLvalue aftouch ampdbfs ampmidi aresonk balance bexprnd  biquada changed clockon cps2pch cpsmidi cpstmid cpstuni cpsxpch  dbfsamp dcblock deltap3 deltapi deltapn deltapx dispfft display  envlpxr exprand expsega expsegr filelen filter2 flanger fmmetal  fmrhode fmvoice follow2 foscili fprints ftchnls ftloadk ftlptim  ftsavek gogobel granule hilbert initc14 initc21 invalue jitter2  jspline linrand linsegr locsend logbtwo loopseg loscil3 lowresx  lphasor lposcil lpreson lpshold marimba massign midic14 midic21  midichn midion2 midiout moogvcf noteoff nreverb nstrnum octmidi  oscil1i outic14 outipat outkc14 outkpat pcauchy pchbend pchmidi  phaser1 phaser2 pinkish poisson polyaft poscil3 printk2 printks  product pvcross pvsanal pvsinfo pvsynth randomh randomi release  repluck reverb2 rspline rtclock seqtime sfilist sfinstr sfplay3  sfplaym sfplist slider8 sndwarp soundin spat3di spat3dt specsum  streson tableiw tablekt tableng tablera tablewa taninv2 tempest  tlineto transeg trigger trigseq trirand turnoff unirand valpass  vco2ift vdelay3 vdelayk vdelayx vexpseg vibrato vlinseg vlowres  vmirror waveset weibull wgbrass wgflute wgpluck wguide1 wguide2  xtratim zakinit FLbutton FLcolor2 FLprintk FLroller FLscroll  FLsetBox FLsetVal FLslider FLupdate betarand butterbp butterbr  butterhp butterlp chanctrl clockoff convolve cpsmidib ctrlinit  cuserrnd deltapxw distort1 downsamp duserrnd filepeak fmpercfl  fmwurlie fprintks hsboscil lowpass2 lpfreson lpinterp lposcil3  maxalloc midictrl multitap nestedap octmidib oscilikt outvalue  pchmidib powoftwo prealloc pvinterp pvsadsyn pvscross pvsfread  pvsmaska rireturn samphold schedule semitone sensekey setksmps  sfinstr3 sfinstrm sfplay3m sfpreset slider16 slider32 slider64  slider8f soundout specaddm specdiff specdisp specfilt spechist  specptrk specscal spectrum sprintks subinstr svfilter tablegpw  tableikt tablemix tableseg tablewkt tablexkt tb0_init tb1_init  tb2_init tb3_init tb4_init tb5_init tb6_init tb7_init tb8_init  tb9_init tempoval vco2init vdelayxq vdelayxs vdelayxw vecdelay  wgpluck2 wterrain xscanmap zfilter2 FLbutBank FLgetsnap FLpackEnd  FLprintk2 FLsetFont FLsetSize FLsetText FLsetsnap FLslidBnk  FLtabsEnd dripwater eventname ktableseg noteondur osciliktp  oscilikts pgmassign phasorbnk pitchamdf pvbufread readclock  sandpaper scantable schedwhen sfinstr3m sfpassign slider16f  slider32f slider64f sndwarpst soundoutc soundouts tablecopy  tableigpw tableimix tablexseg tb10_init tb11_init tb12_init  tb13_init tb14_init tb15_init timeinstk timeinsts vbap4move  vbap8move vbapzmove vdelayxwq vdelayxws xscansmap FLgroupEnd  FLloadsnap FLpack_end FLpanelEnd FLsavesnap FLsetAlign FLsetColor  FLsetVal_i FLtabs_end filenchnls noteondur2 scanhammer schedkwhen  tableicopy tambourine vbap16move vbaplsinit wgbowedbar FLgroup_end  FLpanel_end FLscrollEnd FLsetColor2 mididefault midinoteoff  sleighbells FLscroll_end subinstrinit FLsetPosition FLsetTextSize  FLsetTextType midinoteoncps midinoteonkey midinoteonoct  midinoteonpch midipitchbend schedwhenname FLsetTextColor  schedkwhenname midicontrolchange midiprogramchange  midipolyaftertouch midichannelaftertouch",
	[1] = "sr kr ar ksmps nchnls",
	[2] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x669999),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0x339933),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 3,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTR"] = {
		id = 4,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 5,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["OPCODE"] = {
		id = 6,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["HEADER STATEMENT"] = {
		id = 7,
		fgColor = rgb(0xCC2200),
		bgColor = rgb(0xFFFFFF),
	},
	["USER KEYWORDS"] = {
		id = 8,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT BLOCK"] = {
		id = 9,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["PARAMETER"] = {
		id = 10,
		fgColor = rgb(0x660099),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["A-RATE VARIABLE"] = {
		id = 11,
		fgColor = rgb(0x660099),
		bgColor = rgb(0xFFFFFF),
	},
	["K-RATE VARIABLE"] = {
		id = 12,
		fgColor = rgb(0x660099),
		bgColor = rgb(0xFFFFFF),
	},
	["I-RATE VARIABLE"] = {
		id = 13,
		fgColor = rgb(0x660099),
		bgColor = rgb(0xFFFFFF),
	},
	["GLOBAL VARIABLE"] = {
		id = 14,
		fgColor = rgb(0x660099),
		bgColor = rgb(0xFFFFFF),
	},
	["END OF LINE WHERE STRING IS NOT CLOSED"] = {
		id = 15,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xE0C0E0),
	},
}
return L
