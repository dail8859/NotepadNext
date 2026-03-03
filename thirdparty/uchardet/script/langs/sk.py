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

import re

## Mandatory Properties ##

# The human name for the language, in English.
name = 'Slovak'
# Use 2-letter ISO 639-1 if possible, 3-letter ISO code otherwise,
# or use another catalog as a last resort.
code = 'sk'
# ASCII characters are also used in French.
use_ascii = True
# The charsets we want to support and create data for.
charsets = ['ISO-8859-2', 'Windows-1250', 'IBM852', 'MAC-CENTRALEUROPE']

## Optional Properties ##

# Alphabet characters.
# If use_ascii=True, there is no need to add any ASCII characters.
# If case_mapping=True, there is no need to add several cases of a same
# character (provided Python algorithms know the right cases).
alphabet = 'áäčďĺľňóŕšťúýž'
# The starred page which was rewarded on the main page when I created
# the data.
start_pages = ['Dôkaz (matematika)']
# give possibility to select another code for the Wikipedia URL.
wikipedia_code = code
# 'a' and 'A' will be considered the same character, and so on.
# This uses Python algorithm to determine upper/lower-case of a given
# character.
case_mapping = True

# A function to clean content returned by the `wikipedia` python lib,
# in case some unwanted data has been overlooked.
# Note that we are already cleaning away the '=' from the title syntax
# of Wikipedia, as well as double spaces. But sometimes, Wikipedia in
# some language may return weird syntax or UI text which should be
# discarded. If you encounter one of these cases, use this function.
def clean_wikipedia_content(content):
    # Do your garbage text cleaning here.
    return content
