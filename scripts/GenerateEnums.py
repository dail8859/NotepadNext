import sys
import os
import getopt
import pathlib

scintillaDirectory = "../src/scintilla"
scintillaScriptsDirectory = os.path.join(scintillaDirectory, "scripts")
sys.path.append(scintillaScriptsDirectory)

import Face
import FileGenerator
import HFacer

namespace = ""

deadValues = [
    "INDIC_CONTAINER",
    "INDIC_IME",
    "INDIC_IME_MAX",
    "INDIC_MAX",
]

def HMessages(f):
    out = ["enum class Message {"]
    for name in f.order:
        v = f.features[name]
        if v["Category"] != "Deprecated":
            if v["FeatureType"] in ["fun", "get", "set"]:
                out.append("    " + name + " = " + v["Value"] + ",")
    out.append("};")
    out.append("Q_ENUM(Message);")

    return out

def HEnumerations(f):
    out = []
    for name in f.order:
        v = f.features[name]
        if v["Category"] != "Deprecated":
            # Only want non-deprecated enumerations and lexers are not part of Scintilla API
            if v["FeatureType"] in ["enu"] and name != "Lexer":
                out.append("")
                prefixes = v["Value"].split()
                out.append("enum class " + name + " {")
                for valueName in f.order:
                    prefixMatched = ""
                    for p in prefixes:
                        if valueName.startswith(p) and valueName not in deadValues:
                            prefixMatched = p
                    if prefixMatched:
                        vEnum = f.features[valueName]
                        valueNameNoPrefix = ""
                        if valueName in f.aliases:
                            valueNameNoPrefix = f.aliases[valueName]
                        else:
                            valueNameNoPrefix = valueName[len(prefixMatched):]
                            if not valueNameNoPrefix:	# Removed whole name
                                valueNameNoPrefix = valueName
                            if valueNameNoPrefix.startswith("SC_"):
                                valueNameNoPrefix = valueNameNoPrefix[len("SC_"):]
                        pascalName = Face.PascalCase(valueNameNoPrefix)
                        out.append("    " + pascalName + " = " + vEnum["Value"] + ",")
                out.append("};")
                out.append(f"Q_ENUM({name});")

    out.append("\t")
    out.append("\tenum class Notification {")
    for name in f.order:
        v = f.features[name]
        if v["Category"] != "Deprecated":
            if v["FeatureType"] in ["evt"]:
                out.append("\t\t" + name + " = " + v["Value"] + ",")
    out.append("\t};")
    out.append(f"\tQ_ENUM(Notification);")
    return out


def RegenerateAll(root):
    HFacer.RegenerateAll(root / "src/scintilla/", False)

    f = Face.Face()
    f.ReadFromFile(root / "src/scintilla/include/Scintilla.iface")
    FileGenerator.Regenerate(root / "src/NotepadNext/ScintillaEnums.h", "/* ", HMessages(f) + HEnumerations(f))

    #FileGenerator.Regenerate(include / "ScintillaMessages.h", "//", HMessages(f))
    # FileGenerator.Regenerate(include / "ScintillaTypes.h", "//", HEnumerations(f), HConstants(f))
    # FileGenerator.Regenerate(include / "ScintillaCall.h", "//", HMethods(f))
    # FileGenerator.Regenerate(root / "call" / "ScintillaCall.cxx", "//", CXXMethods(f))

if __name__ == "__main__":
    print(pathlib.Path(__file__).resolve().parent.parent)
    RegenerateAll(pathlib.Path(__file__).resolve().parent.parent)

#GenerateFile("ScintillaEdit.h.template", "ScintillaEdit.h", "/* ", True, printHFile(f))