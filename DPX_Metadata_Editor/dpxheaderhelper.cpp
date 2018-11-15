//-----------------------------------------------------------------------------
// This file is part of DPX Metadata Editor
//
// Copyright (c) 2018 Thomas Aschenbach, Colorlab Corporation
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
//   DPX Metadata Editor is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------
#include "DPXHeaderHelper.h"
#include "DPX.h"
using namespace dpx;


QString GetTransferString (int i)
{
    switch(i)
    {
    case 0:
        return "User-defined";

    case 1:
        return "Printing density";

    case 2:
        return "Linear";

    case 3:
        return "Logarithmic";

    case 4:
        return "Unspecified video";

    case 5:
        return "SMPTE 240M";

    case 6:
        return "CCIR 709-1";

    case 7:
        return "CCIR 601-2 system B or G";

    case  8:
        return "CCIR 601-2 system M";

    case  9:
        return "NTSC composite video";

    case  10:
        return "PAL composite video";

    case  11:
        return "Z linear";

    case  12:
        return "Z homogeneous";

    default:
        return "Unknown";
    }

}


QString GetDescriptorString (int i)
{

    switch(i)
    {


    case 0:


        return "User-defined ";

    case 1:


        return "Red";

    case 2:


        return "Green";

    case 3:


        return "Blue";

    case  4:


        return "Alpha";

    case 6:


        return "Luminance";

    case 7:


        return "Chrominance";

    case 8:


        return "Depth";

    case 9:


        return "Composite video";

    case 50:


        return "RGB";

    case  51:


        return "RGBA";

    case 52:


        return "ABGR";

    case 100:


        return "CbYCrY";

    case 101:


        return "CbYaCrYa";

    case 102:


        return "CbYCr";

    case  103:


        return "CbYCra";

    case 150:


        return "User-defined 2-component element";

    case  151:


        return " User-defined 3-component element";

    case  152:


        return "  User-defined 4-component element";

    case  153:


        return "  User-defined 5-component element";

    case 154:


        return "  User-defined 6-component element";

    case 155:


        return "  User-defined 7-component element";

    case 156:


        return "  User-defined 8-component element";
    default:
        return "Unknown";
    }


}
QString GetColormetricString (int i)
{

    switch(i)
    {

    case 0:


        return "User-defined";

    case 1:


        return "Printing density";

    case 4:


        return "Unspecified video";

    case 5:


        return "SMPTE 240M";

    case 6:


        return "CCIR 709-1";

    case 7:


        return "CCIR 601-2 system B or G";

    case 8:


        return "CCIR 601-2 system M";

    case 9:


        return "NTSC composite video";

    case 10:


        return "PAL composite video";

    default:
        return "Unknown";
    }


}
QString GetOrientationString (int i)
{

    switch(i)
    {

    case 0:


        return "LeftToRightTopToBottom";

    case 1:


        return "RightToLeftTopToBottom";

    case 2:


        return "LeftToRightBottomToTop";

    case 3:


        return "RightToLeftBottomToTop";

    case 4:


        return "TopToBottomLeftToRight";

    case 5:


        return "TopToBottomRightToLeft";

    case 6:


        return "BottomToTopLeftToRight";

    case 7:


        return "BottomToTopRightToLeft";



    default:
        return "UndefinedOrientation";
    }


}
