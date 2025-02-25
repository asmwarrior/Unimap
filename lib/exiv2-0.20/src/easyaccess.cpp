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
  File:      easyaccess.cpp
  Version:   $Rev: 2214 $
  Author(s): Carsten Pfeiffer <pfeiffer@kde.org>
             Andreas Huggel (ahu) <ahuggel@gmx.net>
  History:   28-Feb-09, gis: created
 */
// *****************************************************************************
#include "rcsid.hpp"
EXIV2_RCSID("@(#) $Id: easyaccess.cpp 2214 2010-05-09 16:00:30Z ahuggel $")

// *****************************************************************************
// included header files
#include "easyaccess.hpp"

// *****************************************************************************
namespace {

    using namespace Exiv2;

    /*!
      @brief Search \em ed for a Metadatum specified by the \em keys.
             The \em keys are searched in the order of their appearance, the
             first available Metadatum is returned.

      @param ed The %Exif metadata container to search
      @param keys Array of keys to look for
      @param count Number of elements in the array
     */
    ExifData::const_iterator findMetadatum(const ExifData& ed,
                                           const char* keys[],
                                           int count)
    {
        for (int i = 0; i < count; ++i) {
            ExifData::const_iterator pos = ed.findKey(ExifKey(keys[i]));
            if (pos != ed.end()) return pos;
        }
        return ed.end();
    } // findMetadatum

} // anonymous namespace

// *****************************************************************************
// class member definitions
namespace Exiv2 {

    ExifData::const_iterator orientation(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.Orientation",
            "Exif.Panasonic.Rotation",
            "Exif.MinoltaCs5D.Rotation",
            "Exif.MinoltaCs7D.Rotation"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator isoSpeed(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.ISOSpeedRatings",
            "Exif.Image.ISOSpeedRatings",
            "Exif.CanonSi.ISOSpeed",
            "Exif.CanonCs.ISOSpeed",
            "Exif.Nikon1.ISOSpeed",
            "Exif.Nikon2.ISOSpeed",
            "Exif.Nikon3.ISOSpeed",
            "Exif.NikonIi.ISO",
            "Exif.NikonIi.ISO2",
            "Exif.MinoltaCsNew.ISOSetting",
            "Exif.MinoltaCsOld.ISOSetting",
            "Exif.MinoltaCs5D.ISOSpeed",
            "Exif.MinoltaCs7D.ISOSpeed",
            "Exif.Pentax.ISO",
            "Exif.Olympus.ISOSpeed"
        };

        // Find the first ISO value which is not "0"
        const int cnt = EXV_COUNTOF(keys);
        ExifData::const_iterator md = ed.end();
        for (int idx = 0; idx < cnt; ) {
            md = findMetadatum(ed, keys + idx, cnt - idx);
            if (md == ed.end()) break;
            std::ostringstream os;
            md->write(os, &ed);
            bool ok = false;
            long v = parseLong(os.str(), ok);
            if (ok && v != 0) break;
            while (strcmp(keys[idx++], md->key().c_str()) != 0 && idx < cnt) {}
            md = ed.end();
        }

        return md;
    }

    ExifData::const_iterator flashBias(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonSi.FlashBias",
            "Exif.Panasonic.FlashBias",
            "Exif.Olympus.FlashBias",
            "Exif.OlympusCs.FlashExposureComp"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator exposureMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.ExposureProgram",
            "Exif.Image.ExposureProgram",
            "Exif.CanonCs.ExposureProgram",
            "Exif.MinoltaCs7D.ExposureMode",
            "Exif.MinoltaCs5D.ExposureMode",
            "Exif.MinoltaCsNew.ExposureMode",
            "Exif.MinoltaCsOld.ExposureMode",
            "Exif.Sigma.ExposureMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator sceneMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonCs.EasyMode",
            "Exif.Fujifilm.PictureMode",
            "Exif.MinoltaCsNew.SubjectProgram",
            "Exif.OlympusCs.SceneMode",
            "Exif.Panasonic.ShootingMode",
            "Exif.Panasonic.SceneMode",
            "Exif.Pentax.PictureMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator macroMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonCs.Macro",
            "Exif.Fujifilm.Macro",
            "Exif.Olympus.Macro",
            "Exif.OlympusCs.MacroMode",
            "Exif.Panasonic.Macro",
            "Exif.MinoltaCsNew.MacroMode",
            "Exif.MinoltaCsOld.MacroMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator imageQuality(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonCs.Quality",
            "Exif.Fujifilm.Quality",
            "Exif.Sigma.Quality",
            "Exif.Nikon1.Quality",
            "Exif.Nikon2.Quality",
            "Exif.Nikon3.Quality",
            "Exif.Olympus.Quality",
            "Exif.OlympusCs.Quality",
            "Exif.Panasonic.Quality",
            "Exif.Minolta.Quality",
            "Exif.MinoltaCsNew.Quality",
            "Exif.MinoltaCsOld.Quality",
            "Exif.MinoltaCs5D.Quality",
            "Exif.MinoltaCs7D.Quality"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator whiteBalance(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonSi.WhiteBalance",
            "Exif.Fujifilm.WhiteBalance",
            "Exif.Sigma.WhiteBalance",
            "Exif.Nikon1.WhiteBalance",
            "Exif.Nikon2.WhiteBalance",
            "Exif.Nikon3.WhiteBalance",
            "Exif.Olympus.WhiteBalance",
            "Exif.OlympusCs.WhiteBalance",
            "Exif.Panasonic.WhiteBalance",
            "Exif.MinoltaCs5D.WhiteBalance",
            "Exif.MinoltaCs7D.WhiteBalance",
            "Exif.MinoltaCsNew.WhiteBalance",
            "Exif.MinoltaCsOld.WhiteBalance",
            "Exif.Photo.WhiteBalance"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator lensName(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonCs.LensType",
            "Exif.NikonLd1.LensIDNumber",
            "Exif.NikonLd2.LensIDNumber",
            "Exif.NikonLd3.LensIDNumber",
            "Exif.Minolta.LensID",
            "Exif.Pentax.LensType"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator saturation(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.Saturation",
            "Exif.CanonCs.Saturation",
            "Exif.MinoltaCsNew.Saturation",
            "Exif.MinoltaCsOld.Saturation",
            "Exif.MinoltaCs7D.Saturation",
            "Exif.MinoltaCs5D.Saturation",
            "Exif.Fujifilm.Color",
            "Exif.Nikon3.Saturation",
            "Exif.Panasonic.Saturation",
            "Exif.Pentax.Saturation",
            "Exif.Sigma.Saturation"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator sharpness(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.Sharpness",
            "Exif.CanonCs.Sharpness",
            "Exif.Fujifilm.Sharpness",
            "Exif.MinoltaCsNew.Sharpness",
            "Exif.MinoltaCsOld.Sharpness",
            "Exif.MinoltaCs7D.Sharpness",
            "Exif.MinoltaCs5D.Sharpness",
            "Exif.Olympus.SharpnessFactor",
            "Exif.Panasonic.Sharpness",
            "Exif.Pentax.Sharpness",
            "Exif.Sigma.Sharpness"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator contrast(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.Contrast",
            "Exif.CanonCs.Contrast",
            "Exif.Fujifilm.Tone",
            "Exif.MinoltaCsNew.Contrast",
            "Exif.MinoltaCsOld.Contrast",
            "Exif.MinoltaCs7D.Contrast",
            "Exif.MinoltaCs5D.Contrast",
            "Exif.Olympus.Contrast",
            "Exif.Panasonic.Contrast",
            "Exif.Pentax.Contrast",
            "Exif.Sigma.Contrast"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator sceneCaptureType(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.SceneCaptureType",
            "Exif.Olympus.SpecialMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator meteringMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.MeteringMode",
            "Exif.Image.MeteringMode",
            "Exif.CanonCs.MeteringMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator make(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.Make"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator model(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.Model"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator exposureTime(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.ExposureTime",
            "Exif.Image.ExposureTime"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator fNumber(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.FNumber",
            "Exif.Image.FNumber"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator subjectDistance(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.SubjectDistance",
            "Exif.Image.SubjectDistance",
            "Exif.CanonSi.SubjectDistance",
            "Exif.MinoltaCsNew.FocusDistance",
            "Exif.Nikon1.FocusDistance",
            "Exif.Nikon3.FocusDistance",
            "Exif.NikonLd2.FocusDistance",
            "Exif.NikonLd3.FocusDistance",
            "Exif.Olympus.FocusDistance",
            "Exif.OlympusFi.FocusDistance"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator serialNumber(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.CameraSerialNumber",
            "Exif.Canon.SerialNumber",
            "Exif.Nikon3.SerialNumber",
            "Exif.Nikon3.SerialNO",
            "Exif.Fujifilm.SerialNumber",
            "Exif.Olympus.SerialNumber",
            "Exif.Sigma.SerialNumber"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

    ExifData::const_iterator focalLength(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.FocalLength",
            "Exif.Image.FocalLength",
            "Exif.Canon.FocalLength",
            "Exif.NikonLd2.FocalLength",
            "Exif.NikonLd3.FocalLength",
            "Exif.MinoltaCsNew.FocalLength",
            "Exif.Pentax.FocalLength"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }

}                                       // namespace Exiv2
