local L = {}

L.lexer = "blitzbasic"

L.singleLineComment = "; "

L.extensions = {
	"bb",
}

L.keywords = {
	[0] = "abs accepttcpstream acos after and apptitle asc asin atan atan2 automidhandle autosuspend availvidmem backbuffer banksize before bin calldll case ceil changedir channelpan channelpitch channelplaying channelvolume chr closedir closefile closemovie closetcpserver closetcpstream closeudpstream cls clscolor color colorblue colorgreen colorred commandline const copybank copyfile copyimage copypixel copypixelfast copyrect copystream cos countgfxdrivers countgfxmodes counthostips createbank createdir createimage createnetplayer createprocess createtcpserver createtimer createudpstream currentdate currentdir currenttime data debuglog default delay delete deletedir deletefile deletenetplayer desktopbuffer dim dottedip drawblock drawblockrect drawimage drawimagerect drawmovie each else else if elseif end end function end if end select end type endgraphics endif eof execfile exit exp false field filepos filesize filetype first flip float floor flushjoy flushkeys flushmouse fontheight fontname fontsize fontstyle fontwidth for forever freebank freefont freeimage freesound freetimer frontbuffer function gammablue gammagreen gammared getcolor getenv getkey getmouse gfxdrivername gfxmodedepth gfxmodeexists gfxmodeformat gfxmodeheight gfxmodewidth global gosub goto grabimage graphics graphicsbuffer graphicsdepth graphicsformat graphicsheight graphicswidth handleimage hex hidepointer hostip hostnetgame if imagebuffer imageheight imagerectcollide imagerectoverlap imagescollide imagesoverlap imagewidth imagexhandle imageyhandle include input insert instr int joinnetgame joydown joyhat joyhit joypitch joyroll joytype joyu joyudir joyv joyvdir joyx joyxdir joyy joyyaw joyydir joyz joyzdir keydown keyhit keywait last left len line loadanimimage loadbuffer loadfont loadimage loadsound local lockbuffer lockedformat lockedpitch lockedpixels log log10 loopsound lower lset maskimage mid midhandle millisecs mod morefiles mousedown mousehit mousex mousexspeed mousey mouseyspeed mousez mousezspeed movemouse movieheight movieplaying moviewidth netmsgdata netmsgfrom netmsgto netmsgtype netplayerlocal netplayername new next nextfile not null openfile openmovie opentcpstream or origin oval pausechannel pausetimer peekbyte peekfloat peekint peekshort pi playcdtrack playmusic playsound plot pokebyte pokefloat pokeint pokeshort print queryobject rand read readavail readbyte readbytes readdir readfile readfloat readint readline readpixel readpixelfast readshort readstring rect rectsoverlap recvnetmsg recvudpmsg repeat replace resettimer resizebank resizeimage restore resumechannel resumetimer return right rnd rndseed rotateimage rset runtimeerror sar savebuffer saveimage scaleimage scanline seedrnd seekfile select sendnetmsg sendudpmsg setbuffer setenv setfont setgamma setgfxdriver sgn shl showpointer shr sin soundpan soundpitch soundvolume sqr startnetgame step stop stopchannel stopnetgame str string stringheight stringwidth systemproperty tan tcpstreamip tcpstreamport tcptimeouts text tformfilter tformimage then tileblock tileimage timerticks to totalvidmem trim true type udpmsgip udpmsgport udpstreamip udpstreamport udptimeouts unlockbuffer until updategamma upper viewport vwait waitkey waitmouse waittimer wend while write writebyte writebytes writefile writefloat writeint writeline writepixel writepixelfast writeshort writestring xor",
	[1] = "None",
	[2] = "None",
	[3] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD1"] = {
		id = 3,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 6,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 7,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD2"] = {
		id = 10,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD3"] = {
		id = 11,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD4"] = {
		id = 12,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["LABEL"] = {
		id = 15,
		fgColor = rgb(0x7F7F00),
		bgColor = rgb(0xFFFFFF),
	},
	["ERROR"] = {
		id = 16,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["HEXNUMBER"] = {
		id = 17,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["BINNUMBER"] = {
		id = 18,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
