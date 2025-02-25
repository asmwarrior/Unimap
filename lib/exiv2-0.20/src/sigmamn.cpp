// ***************************************************************** -*- C++ -*-
/*
 * Copyright (C) 2004-2010 Andreas Huggel <ahuggel@gmx.net>
 *
 * This program is part of the Exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
/*
  File:      sigmamn.cpp
  Version:   $Rev: 2045 $
  Author(s): Andreas Huggel (ahu) <ahuggel@gmx.net>
  History:   02-Apr-04, ahu: created
  Credits:   Sigma and Foveon MakerNote implemented according to the specification
             in "SIGMA and FOVEON EXIF MakerNote Documentation" by Foveon.
             <http://www.x3f.info/technotes/FileDocs/MakerNoteDoc.html>
 */
// *****************************************************************************
#include "rcsid.hpp"
EXIV2_RCSID("@(#) $Id: sigmamn.cpp 2045 2010-04-03 07:53:30Z ahuggel $")

// *****************************************************************************
// included header files
#include "types.hpp"
#include "sigmamn.hpp"
#include "value.hpp"
#include "i18n.h"                // NLS support.

// + standard includes
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstring>

// *****************************************************************************
// class member definitions
namespace Exiv2 {

    // Sigma (Foveon) MakerNote Tag Info
    const TagInfo SigmaMakerNote::tagInfo_[] = {
        TagInfo(0x0002, "SerialNumber", N_("Serial Number"),
                N_("Camera serial number"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0003, "DriveMode", N_("Drive Mode"),
                N_("Drive mode"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0004, "ResolutionMode", N_("Resolution Mode"),
                N_("Resolution mode"), sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0005, "AutofocusMode", N_("Autofocus Mode"),
                N_("Autofocus mode"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0006, "FocusSetting", N_("Focus Setting"),
                N_("Focus setting"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0007, "WhiteBalance", N_("White Balance"),
                N_("White balance"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0008, "ExposureMode", N_("Exposure Mode"),
                N_("Exposure mode"),
                sigmaIfdId, makerTags, asciiString, print0x0008),
        TagInfo(0x0009, "MeteringMode", N_("Metering Mode"),
                N_("Metering mode"),
                sigmaIfdId, makerTags, asciiString, print0x0009),
        TagInfo(0x000a, "LensRange", N_("Lens Range"),
                N_("Lens focal length range"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x000b, "ColorSpace", N_("Color Space"),
                N_("Color space"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x000c, "Exposure", N_("Exposure"),
                N_("Exposure"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x000d, "Contrast", N_("Contrast"),
                N_("Contrast"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x000e, "Shadow", N_("Shadow"),
                N_("Shadow"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x000f, "Highlight", N_("Highlight"),
                N_("Highlight"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x0010, "Saturation", N_("Saturation"),
                N_("Saturation"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x0011, "Sharpness", N_("Sharpness"),
                N_("Sharpness"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x0012, "FillLight", N_("Fill Light"),
                N_("X3 Fill light"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x0014, "ColorAdjustment", N_("Color Adjustment"),
                N_("Color adjustment"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x0015, "AdjustmentMode", N_("Adjustment Mode"),
                N_("Adjustment mode"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0016, "Quality", N_("Quality"),
                N_("Quality"),
                sigmaIfdId, makerTags, asciiString, printStripLabel),
        TagInfo(0x0017, "Firmware", N_("Firmware"),
                N_("Firmware"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0018, "Software", N_("Software"),
                N_("Software"),
                sigmaIfdId, makerTags, asciiString, printValue),
        TagInfo(0x0019, "AutoBracket", N_("Auto Bracket"),
                N_("Auto bracket"),
                sigmaIfdId, makerTags, asciiString, printValue),
        // End of list marker
        TagInfo(0xffff, "(UnknownSigmaMakerNoteTag)", "(UnknownSigmaMakerNoteTag)",
                N_("Unknown SigmaMakerNote tag"),
                sigmaIfdId, makerTags, invalidTypeId, printValue)
    };

    const TagInfo* SigmaMakerNote::tagList()
    {
        return tagInfo_;
    }

    std::ostream& SigmaMakerNote::printStripLabel(std::ostream& os,
                                                  const Value& value,
                                                  const ExifData*)
    {
        std::string v = value.toString();
        std::string::size_type pos = v.find(':');
        if (pos != std::string::npos) {
            if (v[pos + 1] == ' ') ++pos;
            v = v.substr(pos + 1);
        }
        return os << v;
    }

    std::ostream& SigmaMakerNote::print0x0008(std::ostream& os,
                                              const Value& value,
                                              const ExifData*)
    {
        switch (value.toString()[0]) {
        case 'P': os << _("Program"); break;
        case 'A': os << _("Aperture priority"); break;
        case 'S': os << _("Shutter priority"); break;
        case 'M': os << _("Manual"); break;
        default: os << "(" << value << ")"; break;
        }
        return os;
    }

    std::ostream& SigmaMakerNote::print0x0009(std::ostream& os,
                                              const Value& value,
                                              const ExifData*)
    {
        switch (value.toString()[0]) {
        case 'A': os << _("Average"); break;
        case 'C': os << _("Center"); break;
        case '8': os << _("8-Segment"); break;
        default: os << "(" << value << ")"; break;
        }
        return os;
    }

}                                       // namespace Exiv2
