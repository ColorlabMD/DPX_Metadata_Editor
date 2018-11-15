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

#include "dpxsequence.h"
#include "QMessageBox"
#include "QDebug"

#define UINT_10_TO_8(val) \
    (int)(((float)(val & 0x3FF) * (float)(0xFF)) / (float)(0x3FF))

#define FLIP32(val) \
    (uint32_t)(((val & 0xFF000000)>>24) | ((val & 0xFF0000) >> 8) | \
    ((val & 0xFF00) << 8) | ((val & 0xFF) << 24))


DPXSequence::DPXSequence()
{

    isready=false;
    autofilename_edit=false;
}

void DPXSequence::SetSource(QString filename)
{
    QFileInfo finfo(filename);
    dpxdirectory=finfo.dir();

    basefilename = filename.mid(filename.lastIndexOf("/")+1);
    basefilename =basefilename.mid(0,basefilename.length()-4);
    QRegExp ra("\\D");
    QRegExp rn("[A-Z]");

    basefilename= basefilename.mid(0,basefilename.lastIndexOf(ra)+1);//selected file base name


    dpxfiles = finfo.dir().entryList(QStringList() << "*.dpx" ,QDir::Files);



    QString seqnamevalidator;
    foreach(const QString & name , dpxfiles )  //check that each dpx file shares the slected base sequence name
    {
        seqnamevalidator= name.mid(0,name.length()-4);
        if (seqnamevalidator.mid(0,seqnamevalidator.lastIndexOf(ra)+1)!=basefilename)
            dpxfiles.removeOne(name);
    }

    numframes=dpxfiles.count();

    QString firstfile =  dpxfiles[0].mid(0,dpxfiles[0].length()-4);
    firstfile=   firstfile.mid(firstfile.lastIndexOf(ra)+1);


    firstFrameIndex= firstfile.toInt();
    lastFrameIndex= firstFrameIndex+(numframes-1);

    QString lastfile  =  dpxfiles.last().mid(0,dpxfiles.last().length()-4);
    lastfile=   lastfile.mid(lastfile.lastIndexOf(ra)+1);
    isready=true;
    if(lastfile.toInt()!=lastFrameIndex)  // check for broken sequence and warn
    {

        QMessageBox msgBox;
        msgBox.setWindowTitle("Broken Sequence Detected");
        msgBox.setText("There is a missing frame in the sequnce, Continue?");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
            isready=true;
        }else {
            isready=false;
        }

    }

    int d =ReadFile(firstFrameIndex,false);
    if(d==-1)  // check for broken sequence and warn
    {

        QMessageBox msgBox;
        msgBox.setWindowTitle("Bad Frame Detected");
        msgBox.setText("There is a bad frame in the sequnce, Continue?");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
            isready=true;
        }else {
            isready=false;
        }

    }

    imagewidth = currentheader.Width();
    imageheight=currentheader.Height();
    if (currentheader.chan[0].bitDepth==10 && currentheader.chan[0].descriptor ==50)
        image = new QImage(imagewidth,imageheight,QImage::Format_A2RGB30_Premultiplied);
    else
        image = new QImage(imagewidth,imageheight,QImage::Format_RGB888);


}
QString  DPXSequence::computecurrentkeycode(int fnum)
{



    QString kcode = keycodeeditstring.mid(0,10); // subString contains "is"
    QString kfeets = keycodeeditstring.mid(12,4);
    QString kframes = keycodeeditstring.mid(4,2);
    QString kc1 =  keycodeeditstring.mid(0,4);
    QString kc2 =  keycodeeditstring.mid(6,6);




    int relativeperfpos = (fnum-keycodeeditfnum);
    int currentperfpos =keycodetotalcounts + (relativeperfpos * keycodeperfsperframe);
    int hfeet=0;
    int hframes =0;
    if (keycodeuseframes)
    {


        hfeet= (currentperfpos/(keycodeperfspercount));
        hframes = (currentperfpos%(keycodeperfspercount))/keycodeperfsperframe;
    }
    else
    {

        hfeet= (currentperfpos/(keycodeperfspercount));
        hframes = (currentperfpos%(keycodeperfspercount));
    }





    QString CurrentKeycode = kc1 + QString("%1").arg(hframes, 2, 10, QChar('0'))  +kc2+  QString("%1").arg(hfeet, 4, 10, QChar('0'))   ;

    return CurrentKeycode;






}
QString   DPXSequence::computecurrenttimecode(int fnum)
{

    unsigned int totalframes;
    double framerate;
    char d[12];

    unsigned int sec;
    unsigned int frames;
    int fps_timebase=qRound(currentheader_edited.frameRate);
    int h,m,s,f;



    QStringList TCL =  timecodeeditstring.split(QRegExp("[:]"),QString::SkipEmptyParts);
    sec =(((TCL[0].toInt() * 3600 )+ (TCL[1].toInt()* 60)+TCL[2].toInt()));
    frames = TCL[3].toInt() +fnum-timecodeeditfnum;
    totalframes = frames+(sec*fps_timebase);
    sec=totalframes/fps_timebase;
    frames= totalframes%fps_timebase;

    h = (sec/3600);
    sec -= h*3600;
    m = sec/60;
    sec -=m*60;
    s = sec +  (frames/fps_timebase);
    f= frames%fps_timebase;

    //return  return QString::number(h,)+":"+QString::number(m)+":" +QString::number(s)+":"+QString::number(f);

    return  QString("%1").arg(h, 2, 10, QChar('0'))+":"+
            QString("%1").arg(m, 2, 10, QChar('0'))+":"+
            QString("%1").arg(s, 2, 10, QChar('0'))+":"+
            QString("%1").arg(f, 2, 10, QChar('0'));













}
int DPXSequence::Width()
{

    return imagewidth;
}
int DPXSequence::Height()
{

    return imageheight;
}
int DPXSequence::NumFrames()
{
    return dpxfiles.count();
}
int DPXSequence::FirstFrame()
{
    return    firstFrameIndex;
}
int DPXSequence::LastFrame()
{
    return lastFrameIndex;
}
QString DPXSequence::GetFrameFilename(int index)
{
    return dpxdirectory.path()+"/"+ dpxfiles[index-firstFrameIndex];
}
InStream img;
OutStream imgo;

// keep this around, since all the frames will be the same size




int DPXSequence::ReadFile(int position,bool readimage)
{
    currentframeindex=position;
    InStream img;
    QString filename= GetFrameFilename(position);
    if(!img.Open(filename.toStdString().c_str()))
    {
        QString msg;
        msg += "DPX: Cannot open ";
        msg += filename;
        msg += "\n";
        return -1;
        //if(errno) msg += strerror(errno);
        //		throw AeoException(msg);
    }


    // dpx::Writer dpxo;

    dpx.SetInStream(&img);
    if(!dpx.ReadHeader())
    {
        img.Close();
        QString msg;
        msg += "ReadFrameDPX_ImageData: Cannot parse DPX header of  ";
        msg += filename;
        msg += "\n";
        //if(errno) msg += strerror(errno);
        //throw AeoException(msg);
        return -1;
    }
    currentheader = dpx.header;


    //unsigned char * ud = new unsigned char[dpxsequence.currentheader.userSize];
    userheadersize=currentheader.userSize;
    if (userheadersize<32)
        hasuserdata=false;
    else
    {
        userheaderdata =  QByteArray(userheadersize,0);


        dpx.ReadUserData((unsigned char *)userheaderdata.data());
        hasuserdata=true;
    }
    if(readimage)
    {
        if (currentheader.chan[0].bitDepth==10 && currentheader.chan[0].descriptor ==50)
        {

            uint32_t imaggedatasize;


            imaggedatasize = imagewidth * imageheight *4;
            img.Seek(currentheader.chan[0].dataOffset,InStream::Origin());
            img.Read(image->bits(), imagewidth * imageheight *4);
            uint32_t* pData = (uint32_t*)image->bits();
            uint32_t pval;
            int nCount = imagewidth * imageheight;
            bool fb = currentheader.DatumSwap(0);
            for (int i = 0; i < nCount; i++)
            {
                pval = *pData;
                if(fb)
                    pval=  FLIP32(pval);
                pval >>= 2;
                pval+=0xC0000000;
                *pData++=pval;

            }
        }

        else
        {

            dpx.ReadImage(image->bits(),dpx::DataSize::kByte,dpx::Descriptor::kRGB);


        }
    }






    img.Close();
    dpx.~Reader();


}
void DPXSequence::validateheaders()
{

    for (int i=firstFrameIndex; i<lastFrameIndex+1; i++)
    {
        ReadFile(i, false);
        qDebug()<<"Filename Name Field "<<QString::fromUtf8(currentheader.fileName)<< "Real Name "<< dpxfiles[i-firstFrameIndex];
        // Check Items that are validatable via file and sequence knowledge
        if (QString::fromUtf8(currentheader.fileName) != dpxfiles[i-firstFrameIndex] )
        {

            qDebug()<<"Filename Name Error";
        }
        if(currentheader.SequenceLength()!=numframes)

        {
            //   qDebug()<<"Sequnce Length Error";
        }


    }


}
void DPXSequence::assignheadereditcopy()
{

    currentheader_edited = currentheader;
    userheaderdataedited = userheaderdata;



}
void DPXSequence::updateheader_keycodeonly(int fnum,QString  Keycode)
{
    InStream img;
    OutStream imgo;


    QString filename = GetFrameFilename(fnum);

    // ui->current_file_label->setText("Current File: "+filename );
    if(!img.Open(filename.toStdString().c_str()))
    {
        QString msg;
        msg += "DPX: Cannot open ";
        return;
    }

    dpx::Reader dpx;
    dpx::Writer dpxo;
    // dpx::Writer dpxo;


    dpx.SetInStream((&img));
    if(!dpx.ReadHeader())
    {
        img.Close();
        return;
        //throw AeoException(msg);
    }

    img.Close();
    imgo.OpenUpdate(filename.toStdString().c_str());
    dpxo.SetOutStream((&imgo));

    dpx.header.SetFileEdgeCode(Keycode.toStdString().c_str());



    dpxo.header=dpx.header;

    dpxo.WriteHeader();

    imgo.Close();




}
void DPXSequence::updateheaders(int fnum)
{
    InStream img;
    OutStream imgo;


    QString filename = GetFrameFilename(fnum);

    // ui->current_file_label->setText("Current File: "+filename );
    if(!img.Open(filename.toStdString().c_str()))
    {
        QString msg;
        msg += "DPX: Cannot open ";
        return;
    }

    dpx::Reader dpx;
    dpx::Writer dpxo;
    // dpx::Writer dpxo;


    dpx.SetInStream((&img));
    if(!dpx.ReadHeader())
    {
        img.Close();
        return;
        //throw AeoException(msg);
    }
    userheadersize=dpx.header.userSize;
    userheaderdata =  QByteArray(userheadersize,0);
    dpx.ReadUserData((unsigned char *)userheaderdata.data());
    img.Close();
    imgo.OpenUpdate(filename.toStdString().c_str());
    dpxo.SetOutStream((&imgo));

    dpxo.header=dpx.header;

    foreach (headerfield hf , changelist)
    {
        CopyHeaderFieldEdit(hf,&dpxo.header);
    }
    if (changelist.contains(headerfield::timeCode))
    {

        dpxo.header.SetTimeCode(computecurrenttimecode(fnum).toStdString().c_str());
    }
    if (changelist.contains(headerfield::keycode))
    {

        dpxo.header.SetFileEdgeCode(computecurrentkeycode(fnum).toStdString().c_str());
    }
    if (changelist.contains(headerfield::framePosition))
    {


        dpxo.header.SetFramePosition(((fnum - framepositionorig)+framepositionedit));
    }
    if (changelist.contains(headerfield::fileName) && autofilename_edit)
    {

        dpxo.header.SetFileName(dpxfiles[fnum-firstFrameIndex].toStdString().c_str());
    }
    if (changelist.contains(headerfield::userid) && hasuserdata)
    {
        // userheaderdata.(0,userid_edit.leftJustified(32,' '));

        strcpy(userheaderdata.data(),userid_edit.toStdString().c_str()) ;  // Qt(ss,ss,2);
        //  userheaderdata.(0x0,32-userid_edit.length());
        memset(userheaderdata.data()+userid_edit.length(),char(0),32-userid_edit.length());
        dpxo.WriteUserData(userheaderdata.data());

        //  dpxo.header.SetFileName(dpxfiles[fnum-firstFrameIndex].toStdString().c_str());
    }
    if (changelist.contains(headerfield::userdata) && hasuserdata)
    {
        // userheaderdata.(0,userid_edit.leftJustified(32,' '));

        strcpy(userheaderdata.data()+32,userdata_edit.toStdString().c_str()) ;  // Qt(ss,ss,2);
        //  userheaderdata.(0x0,32-userid_edit.length());
        memset(userheaderdata.data()+32+userdata_edit.length(),char(0),userheadersize-32-userdata_edit.length());
        dpxo.WriteUserData(userheaderdata.data());

        //  dpxo.header.SetFileName(dpxfiles[fnum-firstFrameIndex].toStdString().c_str());
    }
    dpxo.WriteHeader();

    imgo.Close();




}

int DPXSequence::CopyHeaderFieldEdit(headerfield editfield ,dpx::Header *dpxo)

{
    switch (editfield)
    {

    case headerfield::magicNumber:
    {
        dpxo->magicNumber=currentheader_edited.magicNumber;
        break; }
    case headerfield::imageOffset:
    {
        dpxo->imageOffset=currentheader_edited.imageOffset;
        break; }
    case headerfield::version:
    {
        dpxo->SetVersion(currentheader_edited.version);
        break; }
    case headerfield::fileSize:
    {
        dpxo->fileSize=currentheader_edited.fileSize;
        break; }
    case headerfield::dittoKey:
    {
        dpxo->dittoKey=currentheader_edited.dittoKey;
        break; }
    case headerfield::genericSize:
    {
        dpxo->genericSize=currentheader_edited.genericSize;
        break; }
    case headerfield::industrySize:
    {
        dpxo->industrySize=currentheader_edited.industrySize;
        break; }
    case headerfield::userSize:
    {
        dpxo->userSize=currentheader_edited.userSize;
        break; }

    case headerfield::encryptKey:
    {
        dpxo->encryptKey=currentheader_edited.encryptKey;
        break; }


    case headerfield::fileName:
    {
        dpxo->SetFileName(currentheader_edited.fileName);
        break; }
    case headerfield::creationTimeDate:
    {
        dpxo->SetCreationTimeDate(currentheader_edited.creationTimeDate);
        break; }
    case headerfield::creator:
    {
        dpxo->SetCreator(currentheader_edited.creator);
        break; }
    case headerfield::project:
    {
        dpxo->SetProject(currentheader_edited.project);
        break; }
    case headerfield::copyright:
    {
        dpxo->SetCopyright(currentheader_edited.copyright);
        break; }








    case headerfield::reserved1:
    {
        strcpy(dpxo->reserved1,currentheader_edited.reserved1);
        break; }
    case headerfield::imageOrientation:
    {
        dpxo->imageOrientation=currentheader_edited.imageOrientation;
        break; }
    case headerfield::numberOfElements:
    {
        dpxo->numberOfElements=currentheader_edited.numberOfElements;
        break; }
    case headerfield::pixelsPerLine:
    {
        dpxo->pixelsPerLine=currentheader_edited.pixelsPerLine;
        break; }
    case headerfield::linesPerElement:
    {
        dpxo->linesPerElement=currentheader_edited.linesPerElement;
        break; }
    case headerfield::dataSign:
    {
        dpxo->chan[0].dataSign=currentheader_edited.chan[0].dataSign;
        break; }
    case headerfield::lowData:
    {
        dpxo->chan[0].lowData=currentheader_edited.chan[0].lowData;
        break; }
    case headerfield::lowQuantity:
    {
        dpxo->chan[0].lowQuantity=currentheader_edited.chan[0].lowQuantity;
        break; }
    case headerfield::highData:
    {
        dpxo->chan[0].highData=currentheader_edited.chan[0].highData;
        break; }
    case headerfield::highQuantity:
    {
        dpxo->chan[0].highQuantity=currentheader_edited.chan[0].highQuantity;
        break; }
    case headerfield::descriptor:
    {
        dpxo->chan[0].descriptor=currentheader_edited.chan[0].descriptor;
        break; }
    case headerfield::transfer:
    {
        dpxo->chan[0].transfer=currentheader_edited.chan[0].transfer;
        break; }
    case headerfield::colorimetric:
    {
        dpxo->chan[0].colorimetric=currentheader_edited.chan[0].colorimetric;
        break; }
    case headerfield::bitDepth:
    {
        dpxo->chan[0].bitDepth=currentheader_edited.chan[0].bitDepth;
        break; }
    case headerfield::packing:
    {
        dpxo->chan[0].packing=currentheader_edited.chan[0].packing;
        break; }
    case headerfield::encoding:
    {
        dpxo->chan[0].encoding=currentheader_edited.chan[0].encoding;
        break; }
    case headerfield::dataOffset:
    {
        dpxo->chan[0].dataOffset=currentheader_edited.chan[0].dataOffset;
        break; }
    case headerfield::endOfLinePadding:
    {
        dpxo->chan[0].endOfLinePadding=currentheader_edited.chan[0].endOfLinePadding;
        break; }
    case headerfield::endOfImagePadding:
    {
        dpxo->chan[0].endOfImagePadding=currentheader_edited.chan[0].endOfImagePadding;
        break; }
    case headerfield::description:
    {
        dpxo->SetDescription(0,currentheader_edited.chan[0].description);
        break; }
    case headerfield::reserved2:
    {
        strcpy(dpxo->reserved2,currentheader_edited.reserved2);
        break; }
    case headerfield::xOffset:
    {
        dpxo->xOffset=currentheader_edited.xOffset;
        break; }
    case headerfield::yOffset:
    {
        dpxo->yOffset=currentheader_edited.yOffset;
        break; }
    case headerfield::xCenter:
    {
        dpxo->xCenter=currentheader_edited.xCenter;
        break; }
    case headerfield::yCenter:
    {
        dpxo->xCenter=currentheader_edited.yCenter;
        break; }
    case headerfield::xScannedSize:
    {
        dpxo->xScannedSize=currentheader_edited.xScannedSize;
        break; }
    case headerfield::yScannedSize:
    {
        dpxo->yScannedSize=currentheader_edited.yScannedSize;
        break; }

    case headerfield::xOriginalSize:
    {
        dpxo->xOriginalSize=currentheader_edited.xOriginalSize;
        break; }
    case headerfield::yOriginalSize:
    {
        dpxo->yOriginalSize=currentheader_edited.yOriginalSize;
        break; }

    case headerfield::sourceImageFileName:
    {
        dpxo->SetSourceImageFileName(currentheader_edited.sourceImageFileName);
        break; }
    case headerfield::sourceTimeDate:
    {
        dpxo->SetSourceTimeDate(currentheader_edited.sourceTimeDate);
        break; }
    case headerfield::inputDevice:
    {
        dpxo->SetInputDevice(currentheader_edited.inputDevice);
        break; }
    case headerfield::inputDeviceSerialNumber:
    {
        dpxo->SetInputDeviceSerialNumber(currentheader_edited.inputDeviceSerialNumber);
        break; }
    case headerfield::border:
    {
        dpxo->SetBorder(0,currentheader_edited.border[0]);
        dpxo->SetBorder(1,currentheader_edited.border[1]);
        dpxo->SetBorder(2,currentheader_edited.border[2]);
        dpxo->SetBorder(3,currentheader_edited.border[3]);
        break; }

    case headerfield::aspectRatio:
    {
        dpxo->SetAspectRatio(0,currentheader_edited.aspectRatio[0]);
        dpxo->SetAspectRatio(1,currentheader_edited.aspectRatio[1]);

        break; }
    case headerfield::reserved3:
    {
        strcpy(dpxo->reserved3,currentheader_edited.reserved3);
        break; }

        //***********Keycode*************
    case headerfield::format:
    {
        dpxo->SetFormat(currentheader_edited.format);
        break; }
    case headerfield::framePosition:
    {

        dpxo->framePosition=currentheader_edited.framePosition;
        break; }
    case headerfield::sequenceLength:
    {
        dpxo->sequenceLength=currentheader_edited.sequenceLength;
        break; }
    case headerfield::frameRate:
    {
        dpxo->frameRate=currentheader_edited.frameRate;
        break; }
    case headerfield::shutterAngle:
    {
        dpxo->shutterAngle=currentheader_edited.shutterAngle;
        break; }
    case headerfield::frameId:
    {
        dpxo->SetFrameId(currentheader_edited.frameId);
        break; }
    case headerfield::slateInfo:
    {
        dpxo->SetSlateInfo(currentheader_edited.slateInfo);
        break; }
    case headerfield::reserved4:
    {
        strcpy(dpxo->reserved4,currentheader_edited.reserved4);
        break; }

    case headerfield::timeCode:
    {
        dpxo->timeCode=currentheader_edited.timeCode;
        break; }
    case headerfield::userBits:
    {
        dpxo->userBits=currentheader_edited.userBits;
        break; }
    case headerfield::interlace:
    {
        dpxo->interlace=currentheader_edited.interlace;
        break; }
    case headerfield::fieldNumber:
    {
        dpxo->fieldNumber=currentheader_edited.fieldNumber;
        break; }
    case headerfield::videoSignal:
    {
        dpxo->videoSignal=currentheader_edited.videoSignal;
        break; }
    case headerfield::zero:
    {
        dpxo->zero=currentheader_edited.zero;
        break; }
    case headerfield::horizontalSampleRate:
    {
        dpxo->horizontalSampleRate=currentheader_edited.horizontalSampleRate;
        break; }
    case headerfield::verticalSampleRate:
    {
        dpxo->verticalSampleRate=currentheader_edited.verticalSampleRate;
        break; }
    case headerfield::temporalFrameRate:
    {
        dpxo->temporalFrameRate=currentheader_edited.temporalFrameRate;
        break; }
    case headerfield::timeOffset:
    {
        dpxo->timeOffset=currentheader_edited.timeOffset;
        break; }
    case headerfield::vgamma:
    {
        dpxo->gamma=currentheader_edited.gamma;
        break; }
    case headerfield::blackLevel:
    {
        dpxo->blackLevel=currentheader_edited.blackLevel;
        break; }
    case headerfield::blackGain:
    {
        dpxo->blackGain=currentheader_edited.blackGain;
        break; }
    case headerfield::breakPoint:
    {
        dpxo->breakPoint=currentheader_edited.breakPoint;
        break; }
    case headerfield::whiteLevel:
    {
        dpxo->whiteLevel=currentheader_edited.whiteLevel;
        break; }
    case headerfield::integrationTimes:
    {
        dpxo->integrationTimes=currentheader_edited.integrationTimes;
        break; }
    case headerfield::reserved5:
    {
        strcpy(dpxo->reserved5,currentheader_edited.reserved5);
        break; }


    }
    return 1;
}


