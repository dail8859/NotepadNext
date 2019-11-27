#!/bin/python3
# -*- coding: utf-8 -*-

# ##### BEGIN LICENSE BLOCK #####
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is Mozilla Universal charset detector code.
#
# The Initial Developer of the Original Code is
# Netscape Communications Corporation.
# Portions created by the Initial Developer are Copyright (C) 2001
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#          Jehan <jehan@girinstud.io>
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ##### END LICENSE BLOCK #####

# Third party modules.
import unicodedata
import subprocess
import wikipedia
import importlib
import optparse
import datetime
import operator
import requests
import sys
import re
import os

# Custom modules.
import charsets.db
from charsets.codepoints import *

# Command line processing.
usage = 'Usage: {} <LANG-CODE>\n' \
        '\nEx: `{} fr`'.format(__file__, __file__)

description = "Internal tool for uchardet to generate language data."
cmdline = optparse.OptionParser(usage, description = description)
cmdline.add_option('--max-page',
                   help = 'Maximum number of Wikipedia pages to parse (useful for debugging).',
                   action = 'store', type = 'int', dest = 'max_page', default = None)
cmdline.add_option('--max-depth',
                   help = 'Maximum depth when following links from start page (default: 2).',
                   action = 'store', type = 'int',
                   dest = 'max_depth', default = 2)
(options, langs) = cmdline.parse_args()
if len(langs) < 1:
    print("Please select at least one language code.\n")
    exit(1)
if len(langs) > 1:
    print("This script is meant to generate data for one language at a time.\n")
    exit(1)
lang = langs[0]

# Load the language data.
sys_path_backup = sys.path
current_dir = os.path.dirname(os.path.realpath(__file__))
sys.path = [current_dir + '/langs']

try:
    lang = importlib.import_module(lang.lower())
except ImportError:
    print('Unknown language code "{}": '
          'file "langs/{}.py" does not exist.'.format(lang, lang.lower()))
    exit(1)
sys.path = sys_path_backup

charsets = charsets.db.load(lang.charsets)

if not hasattr(lang, 'start_pages') or lang.start_pages is None or \
   lang.start_pages == []:
    # Let's start with the main page, assuming it should have links
    # to relevant pages. In locale wikipedia, this page is usually redirected
    # to a relevant page.
    print("Warning: no `start_pages` set for '{}'. Using ['Main_Page'].\n"
          "         If you don't get good data, it is advised to set a "
          "start_pages` variable yourself.".format(lang.code))
    lang.start_pages = ['Main_Page']
if not hasattr(lang, 'wikipedia_code') or lang.wikipedia_code is None:
    lang.wikipedia_code = lang.code
if not hasattr(lang, 'clean_wikipedia_content') or lang.clean_wikipedia_content is None:
    lang.clean_wikipedia_content = None
if hasattr(lang, 'case_mapping'):
    lang.case_mapping = bool(lang.case_mapping)
else:
    lang.case_mapping = False
if not hasattr(lang, 'custom_case_mapping'):
    lang.custom_case_mapping = None
if not hasattr(lang, 'alphabet') or lang.alphabet is None:
    lang.alphabet = None

def local_lowercase(text, lang):
    lowercased = ''
    for l in text:
        if lang.custom_case_mapping is not None and \
           l in lang.custom_case_mapping:
            lowercased += lang.custom_case_mapping[l]
        elif l.isupper() and \
             lang.case_mapping and \
             len(unicodedata.normalize('NFC', l.lower())) == 1:
            lowercased += l.lower()
        else:
            lowercased += l
    return lowercased

if lang.alphabet is not None:
    # Allowing to provide an alphabet in string format rather than list.
    lang.alphabet = list(lang.alphabet)
    if lang.use_ascii:
        lang.alphabet += [chr(l) for l in range(65, 91)] + [chr(l) for l in range(97, 123)]
    if lang.case_mapping or lang.custom_case_mapping is not None:
        lang.alphabet = [local_lowercase(l, lang) for l in lang.alphabet]
        #alphabet = []
        #for l in lang.alphabet:
            #if l.isupper() and \
               #lang.custom_case_mapping is not None and \
               #l in lang.custom_case_mapping:
                #alphabet.append(lang.custom_case_mapping[l])
            #elif l.isupper() and \
                 #lang.case_mapping and \
                 #len(unicodedata.normalize('NFC', l.lower())) == 1:
                #alphabet.append(l.lower())
            #else:
                #alphabet.append(l)
    lang.alphabet = list(set(lang.alphabet))

# Starting processing.
wikipedia.set_lang(lang.wikipedia_code)

visited_pages = []

# The full list of letter characters.
# The key is the unicode codepoint,
# and the value is the occurrence count.
characters = {}
# Sequence of letters.
# The key is the couple (char1, char2) in unicode codepoint,
# the value is the occurrence count.
sequences = {}
prev_char = None

def process_text(content, lang):
    global charsets
    global characters
    global sequences
    global prev_char

    if lang.clean_wikipedia_content is not None:
        content = lang.clean_wikipedia_content(content)
    # Clean out the Wikipedia syntax for titles.
    content = re.sub(r'(=+) *([^=]+) *\1',
                     r'\2', content)
    # Clean multiple spaces. Newlines and such are normalized to spaces,
    # since they have basically a similar role in the purpose of uchardet.
    content = re.sub(r'\s+', ' ', content)

    if lang.case_mapping or lang.custom_case_mapping is not None:
        content = local_lowercase(content, lang)

    # In python 3, strings are UTF-8.
    # Looping through them return expected characters.
    for char in content:
        is_letter = False
        if ord(char) in characters:
            characters[ord(char)] += 1
            is_letter = True
        else:
            # We save the character if it is at least in one of the
            # language encodings and its not a special character.
            for charset in charsets:
                # Does the character exist in the charset?
                try:
                    codepoint = char.encode(charset, 'ignore')
                except LookupError:
                    # unknown encoding. Use iconv from command line instead.
                    try:
                        call = subprocess.Popen(['iconv', '-f', 'UTF-8', '-t', charset],
                                                stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                                stderr=subprocess.DEVNULL)
                        if call.poll() is not None:
                            (_, error) = call.communicate(input='')
                            print('Error: `iconv` ended with error "{}".\n'.format(error))
                            exit(1)
                        (codepoint, _) = call.communicate(input=char.encode('UTF-8'))
                    except FileNotFoundError:
                        print('Error: "{}" is not a supported charset by python and `iconv` is not installed.\n')
                        exit(1)

                if codepoint == b'':
                    continue
                # ord() is said to return the unicode codepoint.
                # But it turns out it also gives the codepoint for other
                # charsets if I turn the string to encoded bytes first.
                # Not sure if that is a bug or expected.
                codepoint = ord(codepoint)
                if charsets[charset].charmap[codepoint] == LET:
                    characters[ord(char)] = 1
                    is_letter = True
                    break
        if is_letter:
            if prev_char is not None:
                if (prev_char, ord(char)) in sequences:
                    sequences[(prev_char, ord(char))] += 1
                else:
                    sequences[(prev_char, ord(char))] = 1
            prev_char = ord(char)
        else:
            prev_char = None

def visit_pages(titles, depth, lang, logfd):
    global visited_pages
    global options

    if len(titles) == 0:
        return

    next_titles = []
    for title in titles:
        if options.max_page is not None and \
           len(visited_pages) > options.max_page:
            return
        if title in visited_pages:
            continue
        visited_pages += [title]
        try:
            page = wikipedia.page(title)
        except (wikipedia.exceptions.PageError,
                wikipedia.exceptions.DisambiguationError):
            # Let's just discard a page when I get an exception.
            print("Discarding page {}.\n".format(title))
            continue
        logfd.write("\n{} (revision {})".format(title, page.revision_id))

        process_text(page.content, lang)
        next_titles += page.links

    if depth >= options.max_depth:
        return

    visit_pages (next_titles, depth + 1, lang, logfd)

language_c = lang.name.replace('-', '_').title()
build_log = current_dir + '/BuildLangModelLogs/Lang{}Model.log'.format(language_c)
logfd = open(build_log, 'w')
logfd.write('= Logs of language model for {} ({}) =\n'.format(lang.name, lang.code))
logfd.write('\n- Generated by {}'.format(os.path.basename(__file__)))
logfd.write('\n- Started: {}'.format(str(datetime.datetime.now())))
logfd.write('\n- Maximum depth: {}'.format(options.max_depth))
if options.max_page is not None:
    logfd.write('\n- Max number of pages: {}'.format(options.max_page))
logfd.write('\n\n== Parsed pages ==\n')
try:
    visit_pages(lang.start_pages, 0, lang, logfd)
except requests.exceptions.ConnectionError:
    print('Error: connection to Wikipedia failed. Aborting\n')
    exit(1)
logfd.write('\n\n== End of Parsed pages ==')
logfd.write('\n\n- Wikipedia parsing ended at: {}\n'.format(str(datetime.datetime.now())))

########### CHARACTERS ###########

# Character ratios.
ratios = {}
n_char = len(characters)
occurrences = sum(characters.values())

logfd.write("\n{} characters appeared {} times.\n".format(n_char, occurrences))
for char in characters:
    ratios[char] = characters[char] / occurrences
    #logfd.write("Character '{}' usage: {} ({} %)\n".format(chr(char),
    #                                                       characters[char],
    #                                                       ratios[char] * 100))

sorted_ratios = sorted(ratios.items(), key=operator.itemgetter(1),
                       reverse=True)
# Accumulated ratios of the frequent chars.
accumulated_ratios = 0

# If there is no alphabet defined, we just use the first 64 letters, which was
# the original default.
# If there is an alphabet, we make sure all the alphabet characters are in the
# frequent list, and we stop then. There may therefore be more or less than
# 64 frequent characters depending on the language.
if lang.alphabet is None:
    freq_count = 64
else:
    freq_count = 0
    for order, (char, ratio) in enumerate(sorted_ratios):
        if len(lang.alphabet) == 0:
            break
        if chr(char) in lang.alphabet:
            lang.alphabet.remove(chr(char))
        freq_count += 1
    else:
        if len(lang.alphabet) > 0:
            print("Error: alphabet characters are absent from data collection"
                  "\n       Please check the configuration or the data."
                  "\n       Missing characters: {}".format(", ".join(lang.alphabet)))
            exit(1)

logfd.write('\nFirst {} characters:'.format(freq_count))
for order, (char, ratio) in enumerate(sorted_ratios):
    if order >= freq_count:
        break
    logfd.write("\n[{:2}] Char {}: {} %".format(order, chr(char), ratio * 100))
    accumulated_ratios += ratio

logfd.write("\n\nThe first {} characters have an accumulated ratio of {}.\n".format(freq_count, accumulated_ratios))

with open(current_dir + '/header-template.cpp', 'r') as header_fd:
    c_code = header_fd.read()

c_code += '\n/********* Language model for: {} *********/\n\n'.format(lang.name)
c_code += '/**\n * Generated by {}\n'.format(os.path.basename(__file__))
c_code += ' * On: {}\n'.format(str(datetime.datetime.now()))
c_code += ' **/\n'

c_code += \
"""
/* Character Mapping Table:
 * ILL: illegal character.
 * CTR: control character specific to the charset.
 * RET: carriage/return.
 * SYM: symbol (punctuation) that does not belong to word.
 * NUM: 0 - 9.
 *
 * Other characters are ordered by probabilities
 * (0 is the most common character in the language).
 *
 * Orders are generic to a language. So the codepoint with order X in
 * CHARSET1 maps to the same character as the codepoint with the same
 * order X in CHARSET2 for the same language.
 * As such, it is possible to get missing order. For instance the
 * ligature of 'o' and 'e' exists in ISO-8859-15 but not in ISO-8859-1
 * even though they are both used for French. Same for the euro sign.
 */
"""

for charset in charsets:
    charset_c = charset.replace('-', '_').title()
    CTOM_str = 'static const unsigned char {}_CharToOrderMap[]'.format(charset_c)
    CTOM_str += ' =\n{'
    for line in range(0, 16):
        CTOM_str += '\n  '
        for column in range(0, 16):
            cp = line * 16 + column
            cp_type = charsets[charset].charmap[cp]
            if cp_type == ILL:
                CTOM_str += 'ILL,'
            elif cp_type == RET:
                CTOM_str += 'RET,'
            elif cp_type == CTR:
                CTOM_str += 'CTR,'
            elif cp_type == SYM:
                CTOM_str += 'SYM,'
            elif cp_type == NUM:
                CTOM_str += 'NUM,'
            else: # LET
                try:
                    uchar = bytes([cp]).decode(charset)
                except UnicodeDecodeError:
                    print('Unknown character 0X{:X} in {}.'.format(cp, charset))
                    print('Please verify your charset specification.\n')
                    exit(1)
                except LookupError:
                    # Unknown encoding. Use iconv instead.
                    try:
                        call = subprocess.Popen(['iconv', '-t', 'UTF-8', '-f', charset],
                                                stdin=subprocess.PIPE,
                                                stdout=subprocess.PIPE,
                                                stderr=subprocess.PIPE)
                        if call.poll() is not None:
                            (_, error) = call.communicate(input='')
                            print('Error: `iconv` ended with error "{}".\n'.format(error))
                            exit(1)
                        (uchar, _) = call.communicate(input=bytes([cp]))
                        uchar = uchar.decode('UTF-8')
                    except FileNotFoundError:
                        print('Error: "{}" is not a supported charset by python and `iconv` is not installed.\n')
                        exit(1)
                #if lang.case_mapping and uchar.isupper() and \
                   #len(unicodedata.normalize('NFC', uchar.lower())) == 1:
                   # Unless we encounter special cases of characters with no
                   # composed lowercase, we lowercase it.
                if lang.case_mapping or lang.custom_case_mapping is not None:
                    uchar = local_lowercase(uchar, lang)
                for order, (char, ratio) in enumerate(sorted_ratios):
                    if char == ord(uchar):
                        CTOM_str += '{:3},'.format(order)
                        break
                else:
                    CTOM_str += '{:3},'.format(n_char)
                    n_char += 1
        CTOM_str += ' /* {:X}X */'.format(line)
    CTOM_str += '\n};\n/*'
    CTOM_str += 'X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  XA  XB  XC  XD  XE  XF'
    CTOM_str += ' */\n\n'
    c_code += CTOM_str

########### SEQUENCES ###########

ratios = {}
occurrences = sum(sequences.values())
ratio_512 = 0
ratio_1024 = 0

sorted_seqs = sorted(sequences.items(), key=operator.itemgetter(1),
                     reverse=True)
for order, ((c1, c2), count) in enumerate(sorted_seqs):
    if order < 512:
        ratio_512 += count
    elif order < 1024:
        ratio_1024 += count
    else:
        break
ratio_512 /= occurrences
ratio_1024 /= occurrences

logfd.write("\n{} sequences found.\n".format(len(sorted_seqs)))

c_code += """
/* Model Table:
 * Total sequences: {}
 * First 512 sequences: {}
 * Next 512 sequences (512-1024): {}
 * Rest: {}
 * Negative sequences: TODO""".format(len(sorted_seqs),
                                      ratio_512,
                                      ratio_1024,
                                      1 - ratio_512 - ratio_1024)

logfd.write("\nFirst 512 (typical positive ratio): {}".format(ratio_512))
logfd.write("\nNext 512 (512-1024): {}".format(ratio))
logfd.write("\nRest: {}".format(1 - ratio_512 - ratio_1024))

c_code += "\n */\n"

LM_str = 'static const PRUint8 {}LangModel[]'.format(language_c)
LM_str += ' =\n{'
for line in range(0, freq_count):
    LM_str += '\n  '
    for column in range(0, freq_count):
        # Let's not make too long lines.
        if freq_count > 40 and column == int(freq_count / 2):
            LM_str += '\n   '
        first_order = int(line)
        second_order = column
        if first_order < len(sorted_ratios) and second_order < len(sorted_ratios):
            (first_char, _) = sorted_ratios[first_order]
            (second_char, _) = sorted_ratios[second_order]
            if (first_char, second_char) in sequences:
                for order, (seq, _) in enumerate(sorted_seqs):
                    if seq == (first_char, second_char):
                        if order < 512:
                            LM_str += '3,'
                        elif order < 1024:
                            LM_str += '2,'
                        else:
                            LM_str += '1,'
                        break
                else:
                    pass # impossible!
                    LM_str += '0,'
            else:
                LM_str += '0,'
        else:
            # It may indeed happen that we find less than 64 letters used for a
            # given language.
            LM_str += '0,'
LM_str += '\n};\n'
c_code += LM_str

for charset in charsets:
    charset_c = charset.replace('-', '_').title()
    SM_str = '\n\nconst SequenceModel {}{}Model ='.format(charset_c, language_c)
    SM_str += '\n{\n  '
    SM_str += '{}_CharToOrderMap,\n  {}LangModel,'.format(charset_c, language_c)
    SM_str += '\n  {},'.format(freq_count)
    SM_str += '\n  (float){},'.format(ratio_512)
    SM_str += '\n  {},'.format('PR_TRUE' if lang.use_ascii else 'PR_FALSE')
    SM_str += '\n  "{}"'.format(charset)
    SM_str += '\n};'
    c_code += SM_str


lang_model_file = current_dir + '/../src/LangModels/Lang{}Model.cpp'.format(language_c)
with open(lang_model_file, 'w') as cpp_fd:
    cpp_fd.write(c_code)

logfd.write('\n\n- Processing end: {}\n'.format(str(datetime.datetime.now())))
logfd.close()

print("The following language model file has been generated: {}"
      "\nThe build log is available in: {}"
      "\nTest them and commit them.".format(lang_model_file, build_log))
