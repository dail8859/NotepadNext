# This file is part of Notepad Next.
# Copyright 2019 Justin Dailey
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


include(Version.pri)

# Add custom defines
DEFINES += APP_VERSION='"\\\"$$APP_VERSION\\\""'
DEFINES += APP_COPYRIGHT='"\\\"$$APP_COPYRIGHT\\\""'

!isEmpty(DISTRIBUTION) {
    DEFINES += APP_DISTRIBUTION='"\\\" ($$DISTRIBUTION)\\\""'
}
else {
    DEFINES += APP_DISTRIBUTION='"\\\"\\\""'
}

CONFIG += c++1z
CONFIG -= debug_and_release debug_and_release_target

# Controls if we want to define our own regex engine using QRegularExpression
DEFINES += SCI_OWNREGEX

# Build ADS package as static
DEFINES += ADS_STATIC

# https://stackoverflow.com/questions/78598141/first-stdmutexlock-crashes-in-application-built-with-latest-visual-studio
msvc {
    DEFINES += _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
}

msvc:QMAKE_CXXFLAGS += /guard:cf
msvc:QMAKE_LFLAGS += /guard:cf
