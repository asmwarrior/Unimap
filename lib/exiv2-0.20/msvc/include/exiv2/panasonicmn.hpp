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
/*!
  @file    panasonicmn.hpp
  @brief   Panasonic MakerNote implemented using the following references:
           <a href="http://www.compton.nu/panasonic.html">Panasonic MakerNote Information</a> by Tom Hughes,
           Panasonic.pm of <a href="http://www.sno.phy.queensu.ca/~phil/exiftool/">ExifTool</a> by Phil Harvey,
           <a href="http://www.ozhiker.com/electronics/pjmt/jpeg_info/panasonic_mn.html">Panasonic Makernote Format Specification</a> by Evan Hunter.
  @version $Rev: 2109 $
  @author  Andreas Huggel (ahu)
           <a href="mailto:ahuggel@gmx.net">ahuggel@gmx.net</a>
  @author  Gilles Caulier (gc)
           <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
  @date    11-Jun-05, ahu: created
 */
#ifndef PANASONICMN_HPP_
#define PANASONICMN_HPP_

// *****************************************************************************
// included header files
#include "tags.hpp"
#include "types.hpp"

// + standard includes
#include <string>
#include <iosfwd>

// *****************************************************************************
// namespace extensions
namespace Exiv2 {

// *****************************************************************************
// class definitions

    //! MakerNote for Panasonic cameras
    class EXIV2API PanasonicMakerNote {
    public:
        //! Return read-only list of built-in Panasonic tags
        static const TagInfo* tagList();
        //! Return read-only list of built-in Panasonic RAW image tags (IFD0)
        static const TagInfo* tagListRaw();

        //! @name Print functions for Panasonic %MakerNote tags
        //@{
        //! Print SpotMode
        static std::ostream& print0x000f(std::ostream& os, const Value& value, const ExifData*);
        //! Print WhiteBalanceBias
        static std::ostream& print0x0023(std::ostream& os, const Value& value, const ExifData*);
        //@}

    private:
        //! Makernote tag list
        static const TagInfo tagInfo_[];
        //! Taglist for IFD0 of Panasonic RAW images
        static const TagInfo tagInfoRaw_[];

    }; // class PanasonicMakerNote

}                                       // namespace Exiv2

#endif                                  // #ifndef PANASONICMN_HPP_
