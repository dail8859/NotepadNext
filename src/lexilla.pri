# This file is part of Notepad Next.
# Copyright 2021 Justin Dailey
#
# Notepad Next is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Notepad Next is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.


HEADERS += \
    $$PWD/lexilla/access/LexillaAccess.h \
    $$PWD/lexilla/include/Lexilla.h \
    $$PWD/lexilla/include/SciLexer.h \
    $$PWD/lexilla/lexlib/Accessor.h \
    $$PWD/lexilla/lexlib/CatalogueModules.h \
    $$PWD/lexilla/lexlib/CharacterCategory.h \
    $$PWD/lexilla/lexlib/CharacterSet.h \
    $$PWD/lexilla/lexlib/DefaultLexer.h \
    $$PWD/lexilla/lexlib/LexAccessor.h \
    $$PWD/lexilla/lexlib/LexerBase.h \
    $$PWD/lexilla/lexlib/LexerModule.h \
    $$PWD/lexilla/lexlib/LexerNoExceptions.h \
    $$PWD/lexilla/lexlib/LexerSimple.h \
    $$PWD/lexilla/lexlib/OptionSet.h \
    $$PWD/lexilla/lexlib/PropSetSimple.h \
    $$PWD/lexilla/lexlib/SparseState.h \
    $$PWD/lexilla/lexlib/StringCopy.h \
    $$PWD/lexilla/lexlib/StyleContext.h \
    $$PWD/lexilla/lexlib/SubStyles.h \
    $$PWD/lexilla/lexlib/WordList.h

SOURCES += \
    $$PWD/lexilla/access/LexillaAccess.cxx \
    $$PWD/lexilla/lexers/LexA68k.cxx \
    $$PWD/lexilla/lexers/LexAPDL.cxx \
    $$PWD/lexilla/lexers/LexASY.cxx \
    $$PWD/lexilla/lexers/LexAU3.cxx \
    $$PWD/lexilla/lexers/LexAVE.cxx \
    $$PWD/lexilla/lexers/LexAVS.cxx \
    $$PWD/lexilla/lexers/LexAbaqus.cxx \
    $$PWD/lexilla/lexers/LexAda.cxx \
    $$PWD/lexilla/lexers/LexAsm.cxx \
    $$PWD/lexilla/lexers/LexAsn1.cxx \
    $$PWD/lexilla/lexers/LexBaan.cxx \
    $$PWD/lexilla/lexers/LexBash.cxx \
    $$PWD/lexilla/lexers/LexBasic.cxx \
    $$PWD/lexilla/lexers/LexBatch.cxx \
    $$PWD/lexilla/lexers/LexBibTeX.cxx \
    $$PWD/lexilla/lexers/LexBullant.cxx \
    $$PWD/lexilla/lexers/LexCIL.cxx \
    $$PWD/lexilla/lexers/LexCLW.cxx \
    $$PWD/lexilla/lexers/LexCOBOL.cxx \
    $$PWD/lexilla/lexers/LexCPP.cxx \
    $$PWD/lexilla/lexers/LexCSS.cxx \
    $$PWD/lexilla/lexers/LexCaml.cxx \
    $$PWD/lexilla/lexers/LexCmake.cxx \
    $$PWD/lexilla/lexers/LexCoffeeScript.cxx \
    $$PWD/lexilla/lexers/LexConf.cxx \
    $$PWD/lexilla/lexers/LexCrontab.cxx \
    $$PWD/lexilla/lexers/LexCsound.cxx \
    $$PWD/lexilla/lexers/LexD.cxx \
    $$PWD/lexilla/lexers/LexDMAP.cxx \
    $$PWD/lexilla/lexers/LexDMIS.cxx \
    $$PWD/lexilla/lexers/LexDataflex.cxx \
    $$PWD/lexilla/lexers/LexDiff.cxx \
    $$PWD/lexilla/lexers/LexECL.cxx \
    $$PWD/lexilla/lexers/LexEDIFACT.cxx \
    $$PWD/lexilla/lexers/LexEScript.cxx \
    $$PWD/lexilla/lexers/LexEiffel.cxx \
    $$PWD/lexilla/lexers/LexErlang.cxx \
    $$PWD/lexilla/lexers/LexErrorList.cxx \
    $$PWD/lexilla/lexers/LexFSharp.cxx \
    $$PWD/lexilla/lexers/LexFlagship.cxx \
    $$PWD/lexilla/lexers/LexForth.cxx \
    $$PWD/lexilla/lexers/LexFortran.cxx \
    $$PWD/lexilla/lexers/LexGAP.cxx \
    $$PWD/lexilla/lexers/LexGui4Cli.cxx \
    $$PWD/lexilla/lexers/LexHTML.cxx \
    $$PWD/lexilla/lexers/LexHaskell.cxx \
    $$PWD/lexilla/lexers/LexHex.cxx \
    $$PWD/lexilla/lexers/LexHollywood.cxx \
    $$PWD/lexilla/lexers/LexIndent.cxx \
    $$PWD/lexilla/lexers/LexInno.cxx \
    $$PWD/lexilla/lexers/LexJSON.cxx \
    $$PWD/lexilla/lexers/LexJulia.cxx \
    $$PWD/lexilla/lexers/LexKVIrc.cxx \
    $$PWD/lexilla/lexers/LexKix.cxx \
    $$PWD/lexilla/lexers/LexLaTeX.cxx \
    $$PWD/lexilla/lexers/LexLisp.cxx \
    $$PWD/lexilla/lexers/LexLout.cxx \
    $$PWD/lexilla/lexers/LexLua.cxx \
    $$PWD/lexilla/lexers/LexMMIXAL.cxx \
    $$PWD/lexilla/lexers/LexMPT.cxx \
    $$PWD/lexilla/lexers/LexMSSQL.cxx \
    $$PWD/lexilla/lexers/LexMagik.cxx \
    $$PWD/lexilla/lexers/LexMake.cxx \
    $$PWD/lexilla/lexers/LexMarkdown.cxx \
    $$PWD/lexilla/lexers/LexMatlab.cxx \
    $$PWD/lexilla/lexers/LexMaxima.cxx \
    $$PWD/lexilla/lexers/LexMetapost.cxx \
    $$PWD/lexilla/lexers/LexModula.cxx \
    $$PWD/lexilla/lexers/LexMySQL.cxx \
    $$PWD/lexilla/lexers/LexNim.cxx \
    $$PWD/lexilla/lexers/LexNimrod.cxx \
    $$PWD/lexilla/lexers/LexNsis.cxx \
    $$PWD/lexilla/lexers/LexNull.cxx \
    $$PWD/lexilla/lexers/LexOScript.cxx \
    $$PWD/lexilla/lexers/LexOpal.cxx \
    $$PWD/lexilla/lexers/LexPB.cxx \
    $$PWD/lexilla/lexers/LexPLM.cxx \
    $$PWD/lexilla/lexers/LexPO.cxx \
    $$PWD/lexilla/lexers/LexPOV.cxx \
    $$PWD/lexilla/lexers/LexPS.cxx \
    $$PWD/lexilla/lexers/LexPascal.cxx \
    $$PWD/lexilla/lexers/LexPerl.cxx \
    $$PWD/lexilla/lexers/LexPowerPro.cxx \
    $$PWD/lexilla/lexers/LexPowerShell.cxx \
    $$PWD/lexilla/lexers/LexProgress.cxx \
    $$PWD/lexilla/lexers/LexProps.cxx \
    $$PWD/lexilla/lexers/LexPython.cxx \
    $$PWD/lexilla/lexers/LexR.cxx \
    $$PWD/lexilla/lexers/LexRaku.cxx \
    $$PWD/lexilla/lexers/LexRebol.cxx \
    $$PWD/lexilla/lexers/LexRegistry.cxx \
    $$PWD/lexilla/lexers/LexRuby.cxx \
    $$PWD/lexilla/lexers/LexRust.cxx \
    $$PWD/lexilla/lexers/LexSAS.cxx \
    $$PWD/lexilla/lexers/LexSML.cxx \
    $$PWD/lexilla/lexers/LexSQL.cxx \
    $$PWD/lexilla/lexers/LexSTTXT.cxx \
    $$PWD/lexilla/lexers/LexScriptol.cxx \
    $$PWD/lexilla/lexers/LexSmalltalk.cxx \
    $$PWD/lexilla/lexers/LexSorcus.cxx \
    $$PWD/lexilla/lexers/LexSpecman.cxx \
    $$PWD/lexilla/lexers/LexSpice.cxx \
    $$PWD/lexilla/lexers/LexStata.cxx \
    $$PWD/lexilla/lexers/LexTACL.cxx \
    $$PWD/lexilla/lexers/LexTADS3.cxx \
    $$PWD/lexilla/lexers/LexTAL.cxx \
    $$PWD/lexilla/lexers/LexTCL.cxx \
    $$PWD/lexilla/lexers/LexTCMD.cxx \
    $$PWD/lexilla/lexers/LexTeX.cxx \
    $$PWD/lexilla/lexers/LexTxt2tags.cxx \
    $$PWD/lexilla/lexers/LexVB.cxx \
    $$PWD/lexilla/lexers/LexVHDL.cxx \
    $$PWD/lexilla/lexers/LexVerilog.cxx \
    $$PWD/lexilla/lexers/LexVisualProlog.cxx \
    $$PWD/lexilla/lexers/LexX12.cxx \
    $$PWD/lexilla/lexers/LexYAML.cxx \
    $$PWD/lexilla/lexlib/Accessor.cxx \
    $$PWD/lexilla/lexlib/CharacterCategory.cxx \
    $$PWD/lexilla/lexlib/CharacterSet.cxx \
    $$PWD/lexilla/lexlib/DefaultLexer.cxx \
    $$PWD/lexilla/lexlib/LexerBase.cxx \
    $$PWD/lexilla/lexlib/LexerModule.cxx \
    $$PWD/lexilla/lexlib/LexerNoExceptions.cxx \
    $$PWD/lexilla/lexlib/LexerSimple.cxx \
    $$PWD/lexilla/lexlib/PropSetSimple.cxx \
    $$PWD/lexilla/lexlib/StyleContext.cxx \
    $$PWD/lexilla/lexlib/WordList.cxx \
    $$PWD/lexilla/src/Lexilla.cxx

INCLUDEPATH += $$PWD/lexilla/access/ $$PWD/lexilla/lexlib/ $$PWD/lexilla/include/