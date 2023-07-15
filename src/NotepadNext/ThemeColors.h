/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THEME_COLORS_H
#define THEME_COLORS_H

#define WHITE   0xFFFFFF
#define BLACK   0x000000

#define DARK_DEFAULT_FG WHITE
#define DARK_DEFAULT_BG 0x2D2319

#define LIGHT_DEFAULT_FG BLACK
#define LIGHT_DEFAULT_BG WHITE

// dark theme menu colors
#define DARK_MENU_COLOR 0xE0E1E3
#define DARK_MENU_BG_COLOR 0x37414F

#define LIGHT_CPP_INSTRUCTION 0x0000FF  // CPP family instruction color - blue 
#define DARK_CPP_INSTRUCTION  0x00B4F0  // dark mode compensition color

#define LIGHT_CPP_OPERATOR 0x800000     // CPP family operator color - dark blue 
#define DARK_CPP_OPERATOR  WHITE        // dark mode compensition color

#define LIGHT_CPP_TYPE 0x8000FF         // CPP family instruction color
#define DARK_CPP_TYPE  0xF0B400 

#endif