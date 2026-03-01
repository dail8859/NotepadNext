# uchardet

[uchardet](https://www.freedesktop.org/wiki/Software/uchardet/) is an encoding detector library, which takes a sequence of bytes in an unknown character encoding without any additional information, and attempts to determine the encoding of the text. Returned encoding names are [iconv](https://www.gnu.org/software/libiconv/)-compatible.

uchardet started as a C language binding of the original C++ implementation of the universal charset detection library by Mozilla. It can now detect more charsets, and more reliably than the original implementation.

## Supported Languages/Encodings

  * International (Unicode)
    * UTF-8
    * UTF-16BE / UTF-16LE
    * UTF-32BE / UTF-32LE / X-ISO-10646-UCS-4-34121 / X-ISO-10646-UCS-4-21431
  * Arabic
    * ISO-8859-6
    * WINDOWS-1256
  * Bulgarian
    * ISO-8859-5
    * WINDOWS-1251
  * Chinese
    * ISO-2022-CN
    * BIG5
    * EUC-TW
    * GB18030
    * HZ-GB-2312
  * Croatian:
    * ISO-8859-2
    * ISO-8859-13
    * ISO-8859-16
    * Windows-1250
    * IBM852
    * MAC-CENTRALEUROPE
  * Czech
    * Windows-1250
    * ISO-8859-2
    * IBM852
    * MAC-CENTRALEUROPE
  * Danish
    * IBM865
    * ISO-8859-1
    * ISO-8859-15
    * WINDOWS-1252
  * English
    * ASCII
  * Esperanto
    * ISO-8859-3
  * Estonian
    * ISO-8859-4
    * ISO-8859-13
    * ISO-8859-13
    * Windows-1252
    * Windows-1257
  * Finnish
    * ISO-8859-1
    * ISO-8859-4
    * ISO-8859-9
    * ISO-8859-13
    * ISO-8859-15
    * WINDOWS-1252
  * French
    * ISO-8859-1
    * ISO-8859-15
    * WINDOWS-1252
  * German
    * ISO-8859-1
    * WINDOWS-1252
  * Greek
    * ISO-8859-7
    * WINDOWS-1253
  * Hebrew
    * ISO-8859-8
    * WINDOWS-1255
  * Hungarian:
    * ISO-8859-2
    * WINDOWS-1250
  * Irish Gaelic
    * ISO-8859-1
    * ISO-8859-9
    * ISO-8859-15
    * WINDOWS-1252
  * Italian
    * ISO-8859-1
    * ISO-8859-3
    * ISO-8859-9
    * ISO-8859-15
    * WINDOWS-1252
  * Japanese
    * ISO-2022-JP
    * SHIFT_JIS
    * EUC-JP
  * Korean
    * ISO-2022-KR
    * EUC-KR / UHC
  * Lithuanian
    * ISO-8859-4
    * ISO-8859-10
    * ISO-8859-13
  * Latvian
    * ISO-8859-4
    * ISO-8859-10
    * ISO-8859-13
  * Maltese
    * ISO-8859-3
  * Norwegian
    * IBM865
    * ISO-8859-1
    * ISO-8859-15
    * WINDOWS-1252
  * Polish:
    * ISO-8859-2
    * ISO-8859-13
    * ISO-8859-16
    * Windows-1250
    * IBM852
    * MAC-CENTRALEUROPE
  * Portuguese
    * ISO-8859-1
    * ISO-8859-9
    * ISO-8859-15
    * WINDOWS-1252
  * Romanian:
    * ISO-8859-2
    * ISO-8859-16
    * Windows-1250
    * IBM852
  * Russian
    * ISO-8859-5
    * KOI8-R
    * WINDOWS-1251
    * MAC-CYRILLIC
    * IBM866
    * IBM855
  * Slovak
    * Windows-1250
    * ISO-8859-2
    * IBM852
    * MAC-CENTRALEUROPE
  * Slovene
    * ISO-8859-2
    * ISO-8859-16
    * Windows-1250
    * IBM852
    * MAC-CENTRALEUROPE
  * Spanish
    * ISO-8859-1
    * ISO-8859-15
    * WINDOWS-1252
  * Swedish
    * ISO-8859-1
    * ISO-8859-4
    * ISO-8859-9
    * ISO-8859-15
    * WINDOWS-1252
  * Thai
    * TIS-620
    * ISO-8859-11
  * Turkish:
    * ISO-8859-3
    * ISO-8859-9
  * Vietnamese:
    * VISCII
    * Windows-1258
  * Others
    * WINDOWS-1252

## Installation

### Debian/Ubuntu/Mint

    apt-get install uchardet libuchardet-dev

### Mageia

    urpmi libuchardet libuchardet-devel

### Fedora

    dnf install uchardet uchardet-devel

### Gentoo

    emerge uchardet

### Mac

    brew install uchardet
  
  or

    port install uchardet

### Windows

Binary packages are provided in Fedora and Msys2 repositories. There may
exist other pre-built packages but I am not aware of them.
Nevertheless the library is very easily and quickly compilable under
Windows as well, so finding a binary package is not necessary.
Some did it successfully with the [CMake Windows
installer](https://cmake.org/download/) and MinGW. It should be possible
to use MinGW-w64 instead of MinGW, in particular to build both 32 and
64-bit DLL libraries).

Note also that it is very easily cross-buildable (for instance from a
GNU/Linux machine; [crossroad](https://pypi.org/project/crossroad/) may
help, this is what we use in our CI).

### Build from source

Releases are available from:
https://www.freedesktop.org/software/uchardet/releases/

If you prefer a development version, clone the git repository:

    git clone https://gitlab.freedesktop.org/uchardet/uchardet.git

The source can be browsed at: https://gitlab.freedesktop.org/uchardet/uchardet

    cmake .
    make
    make install

### Build with flatpak-builder

Here is a working "module" section to include in your Flatpak's json manifest:

```
"modules": [
    {
        "name": "uchardet",
        "buildsystem": "cmake",
        "builddir": true,
        "config-opts": [ "-DCMAKE_INSTALL_LIBDIR=lib" ],
        "sources": [
            {
                ...
            }
        ]
    }
]
```

### Build with CMake exported targets

uchardet installs a standard pkg-config file which will make it easily
discoverable by any modern build system. Nevertheless if your project also uses
CMake and you want to discover uchardet installation using CMake exported
targets, you may find and link uchardet with:

```
project(sample LANGUAGES C)
find_package ( uchardet )
if (uchardet_FOUND)
  add_executable( sample sample.c )
  target_link_libraries ( sample PRIVATE uchardet::libuchardet )
endif ()
```

Note though that we recommend the library discovery with `pkg-config` because it
is standard and generic. Therefore it will always work, even if we decided to
change our own build system (which is not planned right now, but may always
happen). This is why we advise to use standard `pkg-config` discovery.

Some more CMake specificities may be found in the [commit
message](https://gitlab.freedesktop.org/uchardet/uchardet/-/commit/d7dad549bd5a3442b92e861bcd2c5cda2adeea27)
which implemented such support.

## Usage

### Command Line

```
uchardet Command Line Tool
Version 0.0.8

Authors: BYVoid, Jehan
Bug Report: https://gitlab.freedesktop.org/uchardet/uchardet/-/issues

Usage:
 uchardet [Options] [File]...

Options:
 -v, --version         Print version and build information.
 -h, --help            Print this help.
```

### Library

See [uchardet.h](https://gitlab.freedesktop.org/uchardet/uchardet/-/blob/master/src/uchardet.h)

## History

As said in introduction, this was initially a project of Mozilla to
allow better detection of page encodings, and it used to be part of
Firefox. If not mistaken, this is not the case anymore (probably because
nowadays most websites better announce their encoding, and also UTF-8 is
much more widely spread).

Techniques used by universalchardet are described at https://www-archive.mozilla.org/projects/intl/universalcharsetdetection

It is to be noted that a lot has changed since the original code, yet
the base concept is still around, basing detection not just on encoding
rules, but importantly on analysis of character statistics in languages.

Original code by Mozilla does not seem to be found anymore anywhere, but
it's probably not too far from the initial commit of this repository.

Mozilla code was extracted and packaged into a standalone library under
the name `uchardet` by BYVoid in 2011, in a personal repository.
Starting 2015, I (i.e. Jehan) started contributing, "standardized"
the output to be iconv-compatible, added various encoding/language
support and streamlined generation of sources for new support of
encoding/languages by using texts from Wikipedia as statistics source on
languages through Python scripts. Then I soon became co-maintainer.
In 2016, `uchardet` became a freedesktop project.

## Related Projects

Some of these are bindings of `uchardet`, others are forks of the same
initial code, which has diverged over time, others are native port in
other languages.
This list is not exhaustive and only meant as point of interest. We
don't follow the status for these projects.

  * [R-uchardet](https://cran.r-project.org/package=uchardet) R binding on CRAN
  * [python-chardet](https://github.com/chardet/chardet) Python port
  * [ruby-rchardet](http://rubyforge.org/projects/chardet/) Ruby port
  * [juniversalchardet](http://code.google.com/p/juniversalchardet/) Java port of universalchardet
  * [jchardet](http://jchardet.sourceforge.net/) Java port of chardet
  * [nuniversalchardet](http://code.google.com/p/nuniversalchardet/) C# port of universalchardet
  * [nchardet](http://www.conceptdevelopment.net/Localization/NCharDet/) C# port of chardet
  * [uchardet-enhanced](https://bitbucket.org/medoc/uchardet-enhanced) A fork of mozilla universalchardet
  * [rust-uchardet](https://github.com/emk/rust-uchardet) Rust language binding of uchardet
  * [libchardet](https://github.com/Joungkyun/libchardet) Another C/C++ API wrapping Mozilla code.

## Used by

* [mpv](https://mpv.io/) for subtitle detection
* [Tepl](https://wiki.gnome.org/Projects/Tepl)
* [Nextcloud IOS app](https://github.com/nextcloud/ios)
* [Codelite](https://codelite.org)
* [QtAV](https://www.qtav.org/)
* …

## Licenses

* [Mozilla Public License Version 1.1](http://www.mozilla.org/MPL/1.1/)
* [GNU General Public License, version 2.0](http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) or later.
* [GNU Lesser General Public License, version 2.1](http://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html) or later.

See the file `COPYING` for the complete text of these 3 licenses.

## Code of Conduct

The `uchardet` project is hosted by [freedesktop.org](https://www.freedesktop.org/)
and as such follows its code of conduct. In other words, it means we
will treat anyone with respect and expect anyone to do the same.

Please read [freedesktop.org Code of Conduct](https://www.freedesktop.org/wiki/CodeOfConduct).

In case of any problem regarding abusive behavior in uchardet project,
please contact the maintainer (Jehan) or create a bug report (possibly
private if needed).
