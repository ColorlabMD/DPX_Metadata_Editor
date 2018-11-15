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

#ifndef DPXSEQUENCE_H
#define DPXSEQUENCE_H
#include <QList>
#include <QDir>
#include "DPX.h"
#include "DPXHeader.h"
#include "DPXHeaderHelper.h"

#define MAX_USER_DATA 16000

enum headerfield {
    magicNumber,
    imageOffset,
    version,
    fileSize,
    dittoKey,
    genericSize,
    industrySize,
    userSize,
    fileName,
    creationTimeDate,
    creator,
    project,
    copyright,
    encryptKey,
    reserved1,

    imageOrientation,
    numberOfElements,
    pixelsPerLine,
    linesPerElement,
    dataSign,
    lowData,
    lowQuantity,
    highData,
    highQuantity,
    descriptor,
    transfer,
    colorimetric,
    bitDepth,
    packing,
    encoding,
    dataOffset,
    endOfLinePadding,
    endOfImagePadding,
    description,
    reserved2,







    xOffset,
    yOffset,
    xCenter,
    yCenter,
    xOriginalSize,
    yOriginalSize,
    sourceImageFileName,
    sourceTimeDate,
    inputDevice,
    inputDeviceSerialNumber,
    border,
    aspectRatio,
    xScannedSize,
    yScannedSize,
    reserved3,



    filmManufacturingIdCode,
    filmType,
    perfsOffset,
    prefix,
    count,
    format,
    framePosition,
    sequenceLength,
    heldCount,
    frameRate,
    shutterAngle,
    frameId,
    slateInfo,
    reserved4,

    timeCode,
    userBits,
    interlace,
    fieldNumber,
    videoSignal,
    zero,
    horizontalSampleRate,
    verticalSampleRate,
    temporalFrameRate,
    timeOffset,
    vgamma,
    blackLevel,
    blackGain,
    breakPoint,
    whiteLevel,
    integrationTimes,
    reserved5,
    userid,
    userdata,
    keycode

};
Q_ENUMS(headerfield)
class DPXSequence
{
public:
    DPXSequence();
    dpx::Reader dpx;
    dpx::Header currentheader;
    dpx::Header currentheader_edited;
    int firstFrameIndex;
    int lastFrameIndex;
    int currentframeindex;
    int  imagewidth;
    int  imageheight;
    int numframes;
    bool hasuserdata;
    QList <QString> dpxfiles;
    QSet<headerfield>  changelist;

    QDir dpxdirectory;
    QString basefilename;
    bool isready;
    int Width();
    int Height();

    int NumFrames();
    int FirstFrame();
    int LastFrame();
    QString GetFrameFilename (int index);
    void SetSource(QString filename);
    void GetHeader(int position);
    int ReadFile(int,bool);
    int CopyHeaderFieldEdit(headerfield editfield, dpx::Header *dpxo);
    QByteArray userheaderdataedited;
    // int userheadersize;
    QByteArray userheaderdata;
    int userheadersize;
   // QByteArray imagedata;
    QImage * image;

    QString userid_edit;
    QString userid_current;
    QString userdata_edit;
    QString userdata_current;
    QString Keycodeedit(int fnum);
    QString keycodeeditstring;
    int keycodeeditfnum;
    int keycodetotalcounts;
    int keycodeframespercount;
    int keycodeperfspercount;
    int keycodetotalperfs;
    int keycodeperfsperframe;
    bool keycodeuseframes;
    QString computecurrentkeycode(int fnum);

    int framepositionedit;
    int framepositionorig;
    bool setframepos;
    QString timecodeeditstring;
    int timecodeeditfnum;
    bool autofilename_edit;
    QString ubitsedit;
    QString computecurrenttimecode(int fnum);
    void validateheaders();
    void updateheader_keycodeonly(int fnum,QString  Keycode);

    void updateheaders(int fnum);

    void assignheadereditcopy();


    QStringList headerfieldlist= {
        "magicNumber",
        "imageOffset",
        "version",
        "fileSize",
        "dittoKey",
        "genericSize",
        "industrySize",
        "userSize",
        "fileName",
        "creationTimeDate",
        "creator",
        "project",
        "copyright",
        "encryptKey",
        "reserved1",

        "imageOrientation",
        "numberOfElements",
        "pixelsPerLine",
        "linesPerElement",
        "dataSign",
        "lowData",
        "lowQuantity",
        "highData",
        "highQuantity",
        "descriptor",
        "transfer",
        "colorimetric",
        "bitDepth",
        "packing",
        "encoding",
        "dataOffset",
        "endOfLinePadding",
        "endOfImagePadding",
        "description",
        "reserved2",







        "xOffset",
        "yOffset",
        "xCenter",
        "yCenter",
        "xOriginalSize",
        "yOriginalSize",
        "sourceImageFileName",
        "sourceTimeDate",
        "inputDevice",
        "inputDeviceSerialNumber",
        "border",
        "aspectRatio",
        "xScannedSize",
        "yScannedSize",
        "reserved3",



        "filmManufacturingIdCode",
        "filmType",
        "perfsOffset",
        "prefix",
        "count",
        "format",
        "framePosition",
        "sequenceLength",
        "heldCount",
        "frameRate",
        "shutterAngle",
        "frameId",
        "slateInfo",
        "reserved4",

        "timeCode",
        "userBits",
        "interlace",
        "fieldNumber",
        "videoSignal",
        "zero",
        "horizontalSampleRate",
        "verticalSampleRate",
        "temporalFrameRate",
        "timeOffset",
        "gamma",
        "blackLevel",
        "blackGain",
        "breakPoint",
        "whiteLevel",
        "integrationTimes",
        "reserved5",
        "userid",
        "userdata",
        "keycode"

    };

};

#endif // DPXSEQUENCE_H
