local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"as",
	"mx",
}

L.keywords = {
	[0] = "add for lt tellTarget and function ne this break ge new typeof continue gt not var delete if on void do ifFrameLoaded onClipEvent while else in or with eq le return instanceof case default switch",
	[2] = "arguments constructor class dynamic false extends implements import interface intrinsic newline null private public super static true undefined Accessibility Arguments Array Boolean Button Camera ContextMenu ContextMenuItem CustomActions Color Date Error Function Key LoadVars LocalConnection Math Microphone Mouse MovieClip MovieClipLoader NetConnection NetStream Number PrintJob Object TextField StyleSheet TextFormat TextSnapshot SharedObject Selection Sound Stage String System XML XMLNode XMLSocket Void abs acos asin atan atan2 ceil cos exp floor log max min pow random round sin sqrt tan onActivity onChanged onClose onConnect onData onDragOut onDragOver onEnterFrame onID3 onKeyDown onKeyUp onKillFocus onLoad onLoadComplete onLoadError onLoadInit onLoadProgress onLoadStart onMouseDown onMouseMove onMouseUp onMouseWheel onPress onRelease onReleaseOutside onResize onRollOut onRollOver onScroller onSelect onSetFocus onSoundComplete onStatus onUnload onUpdate onXML addListener addPage addProperty addRequestHeader allowDomain allowInsecureDomain appendChild apply applyChanges asfunction attachAudio attachMovie attachSound attachVideo beginFill beginGradientFill call charAt charCodeAt clear clearInterval cloneNode close concat connect copy createElement createEmptyMovieClip createTextField createTextNode curveTo domain duplicateMovieClip endFill escape eval evaluate findText fscommand flush fromCharCode get getAscii getBeginIndex getBounds getBytesLoaded getBytesTotal getCaretIndex getCode getCount getDate getDay getDepth getEndIndex getFocus getFontList getFullYear getHours getInstanceAtDepth getLocal getMilliseconds getMinutes getMonth getNewTextFormat getNextHighestDepth getPan getProggress getProperty getRGB getSeconds getSelected getSelectedText getSize getStyle getStyleNames getSWFVersion getText getTextExtent getTextFormat getTextSnapshot getTime getTimer getTimezoneOffset getTransform getURL getUTCDate getUTCDay getUTCFullYear getUTCHours getUTCMilliseconds getUTCMinutes getUTCMonth getUTCSeconds getVersion getVolume getYear globalToLocal gotoAndPlay gotoAndStop hasChildNodes hide hideBuiltInItems hitTest hitTestTextNearPos indexOf insertBefore install isActive isDown isToggled join lastIndexOf lineStyle lineTo list load loadClip loadMovie loadMovieNum loadSound loadVariables loadVariablesNum localToGlobal mbchr mblength mbord mbsubstring MMExecute moveTo nextFrame nextScene parseCSS parseFloat parseInt parseXML pause play pop prevScene print printAsBitmap printAsBitmapNum printNum push registerClass removeListener removeMovieClip removeNode removeTextField replaceSel replaceText reverse seek send sendAndLoad setBufferTime set setDate setFocus setFullYear setGain setHours setInterval setMask setMilliseconds setMinutes setMode setMonth setMotionLevel setNewTextFormat setPan setProperty setQuality setRate setRGB setSeconds setSelectColor setSelected setSelection setSilenceLevel setStyle setTextFormat setTime setTransform setUseEchoSuppression setUTCDate setUTCFullYear setUTCHours setUTCMilliseconds setUTCMinutes setUTCMonth setUTCSeconds setVolume setYear shift show showSettings silenceLevel silenceTimeout slice sort sortOn splice split start startDrag stop stopAllSounds stopDrag substr substring swapDepths toggleHighQuality toLowerCase toString toUpperCase trace unescape uninstall unLoadClip unloadMovie unloadMovieNum unshift unwatch updateAfterEvent updateProperties useEchoSuppression valueOf watch endinitclip include initclip __proto__ _accProps _alpha _currentframe _droptarget _focusrect _framesloaded _global _height _highquality _level _lockroot _name _parent _quality _root _rotation _soundbuftime _target _totalframes _url _visible _width _x _xmouse _xscale _y _ymouse _yscale activityLevel align attributes autoSize avHardwareDisable background backgroundColor bandwidth blockIndent bold border borderColor bottomScroll bufferLenght bufferTime builtInItems bullet bytesLoaded bytesTotal callee caller capabilities caption childNodes color condenseWhite contentType currentFps customItems data deblocking docTypeDecl duration embedFonts enabled exactSettings firstChild focusEnabled font fps gain globalStyleFormat hasAccessibility hasAudio hasAudioEncoder hasEmbeddedVideo hasMP3 hasPrinting hasScreenBroadcast hasScreenPlayback hasStreamingAudio hasStreamingVideo hasVideoEncoder height hitArea hscroll html htmlText indent index italic instanceof int ignoreWhite isDebugger isFinite language lastChild leading leftMargin length loaded localFileReadDisable manufacturer maxChars maxhscroll maxscroll menu message motionLevel motionTimeout mouseWheelEnabled multiline muted name names NaN nextSibling nodeName nodeType nodeValue os parentNode password pixelAspectRatio playerType previousSibling prototype quality rate restrict resolutionX resolutionY rightMargin scaleMode screenColor screenDPI screenResolutionX screenResolutionY scroll selectable separatorBefore showMenu size smoothing status styleSheet tabChildren tabEnabled tabIndex tabStops target targetPath text textColor textHeight textWidth time trackAsMenu type underline url useCodepage useHandCursor variable version visible width wordWrap xmlDecl",
}

L.styles = {
	["DEFAULT"] = {
		id = 11,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["FUNCTION"] = {
		id = 20,
		fgColor = rgb(0x95004A),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(0x8000FF),
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
	["CHARACTER"] = {
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
	["VERBATIM"] = {
		id = 13,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["REGEX"] = {
		id = 14,
		fgColor = rgb(0x000000),
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
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR COMMENT"] = {
		id = 23,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR COMMENT DOC"] = {
		id = 24,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
