/*
  ==============================================================================
   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.
   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3
   Details of these licenses can be found at: www.gnu.org/licenses
   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
   ------------------------------------------------------------------------------
   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.
  ==============================================================================
*/

#ifndef APPCOLOURS_H_INCLUDED
#define APPCOLOURS_H_INCLUDED

enum ColourIds
{
    mainBackgroundColourId          = 0x2340000,
    mainAccentColourId              = 0x2345000,
    lightAccentColourId             = 0x2345600,
    whiteNoteColourId = 0x1005000,
    blackNoteColourId = 0x1005001,
    keySeparatorLineColourId = 0x1005002,
    mouseOverKeyOverlayColourId = 0x1005003,  /**< This colour will be overlaid on the normal note colour. */
    keyDownOverlayColourId = 0x1005004,  /**< This colour will be overlaid on the normal note colour. */
    textLabelColourId = 0x1005005,
    upDownButtonBackgroundColourId = 0x1005006,
    upDownButtonArrowColourId = 0x1005007,
    shadowColourId = 0x1005008
};


#endif   // APPCOLOURS_H_INCLUDED
