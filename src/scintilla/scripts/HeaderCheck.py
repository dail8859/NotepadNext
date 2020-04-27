#!/usr/bin/env python3
# Script to check that headers are in a consistent order
# Canonical header order is defined in scripts/HeaderOrder.txt
# Requires Python 3.6 or later

import codecs, glob, os

patterns = [
    "include/*.h",
    "src/*.cxx",
    "lexlib/*.cxx",
    "lexers/*.cxx",
    "win32/*.cxx",
    "gtk/*.cxx",
    "cocoa/*.mm",
    "cocoa/*.h",
    "test/unit/*.cxx",
    "lexilla/src/*.cxx",
    "lexilla/test/*.cxx",
]

def IsHeader(x):
    return x.strip().startswith("#") and ("include" in x or "import" in x)

def HeaderFromIncludeLine(s):
    #\s*#\s*(include|import)\s+\S+\s*
    return s.strip()[1:].strip()[7:].strip()

def ExtractHeaders(filename):
    with codecs.open(filename, "r", "windows-1252") as infile:
        return [HeaderFromIncludeLine(l) for l in infile if IsHeader(l)]

def CheckFiles(root):
    # Find all the lexer source code files
    filePaths = []
    for p in patterns:
        filePaths += glob.glob(os.path.join(root, p))
    # The Qt platform code interleaves system and Scintilla headers
    #~ filePaths += glob.glob(root + "/qt/ScintillaEditBase/*.cpp")
    #~ filePaths += glob.glob(root + "/qt/ScintillaEdit/*.cpp")
    #~ print(filePaths)
    masterHeaderList = ExtractHeaders(os.path.join(root, "scripts/HeaderOrder.txt"))
    orderedPaths = sorted(filePaths, key=str.casefold)
    allIncs = set()
    for f in orderedPaths:
        if "LexCaml" in f: # LexCaml adds system headers in #if to be an external lexer
            continue
        print("   File ", f)
        incs = ExtractHeaders(f)
        #~ print("\n".join(incs))
        news = set(incs) - set(masterHeaderList)
        allIncs = allIncs.union(set(incs))
        m = 0
        i = 0
        while i < len(incs):
            if m == len(masterHeaderList):
                print("**** extend", incs[i:])
                masterHeaderList.extend(incs[i:])
                break
            if masterHeaderList[m] == incs[i]:
                #~ print("equal", masterHeaderList[m])
                i += 1
                m += 1
            else:
                if masterHeaderList[m] not in incs:
                    #~ print("skip", masterHeaderList[m])
                    m += 1
                elif incs[i] not in masterHeaderList:
                    print(f + ":1: Add master", incs[i])
                    masterHeaderList.insert(m, incs[i])
                    i += 1
                    m += 1
                else:
                    print(f + ":1: Header out of order", incs[i], masterHeaderList[m])
                    print("incs", " ".join(incs))
                    i += 1
                    #~ return
        #print("Master header list", " ".join(masterHeaderList))
    unused = sorted(set(masterHeaderList) - allIncs)
    if unused:
        print("In HeaderOrder.txt but not used")
        print("\n".join(unused))

CheckFiles("..")
