/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 * 
 * @author Wayne Zhang
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

// ########## color utilities  ########## 

/**
 * Calculate 'negtive' color, that is, compensation color. 
 * For example, back's compenstation color is white.
 */
#define invertColor(color) color ^ WHITE

/**
 * Convert RGB color value to BGR, as Lua do the samething, reason unknown.
 * To make color RGB values passed from CPP/NotepadNext to Lua scripts the 
 * same value, convert RGB to BGR: RBB->BGR->BRG = RGB
 */
#define rgb2bgr(x) ((x & 0x0000FF) << 16) | (x & 0x00FF00) | ((x & 0xFF0000) >> 16)

// ######### color utilities #############

#define WHITE   0xFFFFFF
#define BLACK   0x000000

#define DARK_DEFAULT_FG WHITE
#define DARK_DEFAULT_BG 0x2D2319

#define LIGHT_DEFAULT_FG BLACK
#define LIGHT_DEFAULT_BG WHITE

// dark theme menu colors
#define DARK_MENU_COLOR 0xE0E1E3
#define DARK_MENU_BG_COLOR 0x37414F

#define LIGHT_INTRUCTION_COLOR 0x0000FF   // instruction color - blue 
#define DARK_INTRUCTION_COLOR  0x00B4F0   // dark mode compensation color

#define LIGHT_OPERATOR_COLOR 0x800000     // operator color - dark blue 
#define DARK_OPERATOR_COLOR  WHITE        // dark mode compensation color

#define LIGHT_TYPE_COLOR 0x8000FF         // type color
#define DARK_TYPE_COLOR  0xF0B400         // dark mode compenstation color

#endif