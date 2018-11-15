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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "DPX.h"
#include "DPXHeader.h"
#include "DPXHeaderHelper.h"
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QDesktopServices>
#include <QInputDialog>
#include <QFile>
#include <QScrollArea>
#include <QProgressDialog>
#include <cstdio>
#include <exception>
#include <DPX.h>
#include <QMenuBar>
#include <fstream>
#include <qmath.h>
#include "confirmwritedialog.h"
#include <keycodedialog.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMenuBar* m_menuBar = new QMenuBar(this);

    QMenu *oneMenu = new QMenu("File");
    QAction*  newAct = new QAction(tr("About"), this);
    newAct->setMenuRole(QAction::AboutRole );
    connect( newAct, &QAction::triggered, this, &MainWindow::showAbout);
    oneMenu->addAction(newAct);

    // ..QMenuBar *menubar= this->menuBar();
    //menubar->setNativeMenuBar(true);
    // QAction* aboutaction ;
    // aboutaction = m_menuBar->addAction("About..");
    header_textfields = ui->tabWidget->findChildren<QLineEdit*>();
    on_EnableEditCB_stateChanged(0);

    dpx_window = new DPXviewer();
    m_menuBar->addAction(oneMenu->menuAction());

    editsmade=false;

    header_textfields_normaledit.append(ui->gfh_timedate_tf);
    header_textfields_normaledit.append(ui->gfh_creator_tf);
    header_textfields_normaledit.append(ui->gfh_project_tf);
    header_textfields_normaledit.append(ui->gfh_copyright_tf);
    header_textfields_normaledit.append(ui->gfh_encryptkey_tf);
    header_textfields_normaledit.append(ui->goh_timedate_tf);
    header_textfields_normaledit.append(ui->goh_inputdevice_tf);
    header_textfields_normaledit.append(ui->goh_inputdevicesn_tf);
    header_textfields_normaledit.append(ui->gih_description_tf);
    header_textfields_normaledit.append(ui->ifh_framerate_tf);
    header_textfields_normaledit.append(ui->ifh_frameid_tf);
    header_textfields_normaledit.append(ui->ifh_slate_tf);
    header_textfields_normaledit.append(ui->ith_framerate_tf);

    header_textfields_advedit.append(header_textfields);
    header_textfields_advedit.removeAll(ui->gfh_magic_tf);
    header_textfields_advedit.removeAll(ui->gfh_industrysize_tf);
    header_textfields_advedit.removeAll(ui->gfh_imageoffset_tf);
    header_textfields_advedit.removeAll(ui->gfh_genericsize_tf);
    header_textfields_advedit.removeAll(ui->gfh_filesize_tf);
    header_textfields_advedit.removeAll(ui->gfh_version_tf);
    header_textfields_advedit.removeAll(ui->gfh_usersize_tf);

    header_textfields_advedit.removeAll(ui->gih_bitsize_tf);
    header_textfields_advedit.removeAll(ui->gih_pixelsperline_tf);
    header_textfields_advedit.removeAll(ui->gih_elementcount_tf);
    header_textfields_advedit.removeAll(ui->gih_linesperelement_tf);
    header_textfields_advedit.removeAll(ui->gih_dataoffset_tf);
    header_textfields_advedit.removeAll(ui->gih_packing_tf);
    header_textfields_advedit.removeAll(ui->gih_eoipaddding_tf);
    header_textfields_advedit.removeAll(ui->gih_eolpadding_tf);

    header_textfields_advedit.removeAll(ui->ifh_count_tf);
    header_textfields_advedit.removeAll(ui->ifh_prefix_tf);
    header_textfields_advedit.removeAll(ui->ifh_filmtype_tf);
    header_textfields_advedit.removeAll(ui->ifh_offset_tf);
    header_textfields_advedit.removeAll(ui->ifh_filmmanid_tf);

    header_textfields_alledit.append(header_textfields);
    header_textfields_alledit.removeAll(ui->ifh_count_tf);
    header_textfields_alledit.removeAll(ui->ifh_prefix_tf);
    header_textfields_alledit.removeAll(ui->ifh_filmtype_tf);
    header_textfields_alledit.removeAll(ui->ifh_offset_tf);
    header_textfields_alledit.removeAll(ui->ifh_filmmanid_tf);
    header_textfields_alledit.removeAll(ui->ith_timecode_tf);
    header_textfields_alledit.removeAll(ui->ith_userbits_tf);


    assignQLEValidators();





}

MainWindow::~MainWindow()
{
    if(dpx_window)
        delete dpx_window;

}
void MainWindow::showAbout()
{
    AboutDialog * ad = new AboutDialog();
    ad->show();

}
void MainWindow::on_openSequenc_BTN_clicked()
{

    int ft;

    QString selectedType = "";
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("DPX Source"), "/",
                                                    tr("DPX FIles (*.dpx)"));

    if(filename.isEmpty()) return;

    QFileInfo finfo = filename;
    dpxsequence.SetSource(filename);
    if(dpxsequence.isready)
    {

        ui->outlabel->setText(  QString::number(markeroutframe));
        ui->inlabel->setText(QString::number(markerinframe));
        //  this->scan.SourceScan(filename.toStdString(),fileFilterArr[ft].fileType);


        ui->Frame_horizontalSlider->setMinimum(dpxsequence.firstFrameIndex);
        ui->Frame_horizontalSlider->setMaximum(dpxsequence.lastFrameIndex);
        ui->Frame_spinBox->setRange(dpxsequence.firstFrameIndex,dpxsequence.lastFrameIndex);
        ui->numfiles_label->setText(QString::number(dpxsequence.NumFrames()));
        ui->firstframe_label->setText(QString::number(dpxsequence.FirstFrame()));
        ui->lastframe_label->setText(QString::number(dpxsequence.LastFrame()));
        markerinframe = dpxsequence.FirstFrame();
        markeroutframe = dpxsequence.LastFrame();
        ui->inlabel->setText(QString::number( markerinframe));
        ui->outlabel->setText(QString::number( markeroutframe));
        update_Read_DPX(dpxsequence.FirstFrame());
        ui->Frame_horizontalSlider->setEnabled(true);
        ui->Frame_spinBox->setEnabled(true);
        ui->showviewer_btn->setEnabled(true);

        ui->Edit_GB->setEnabled(true);



    }
    else
    {
        ui->Frame_horizontalSlider->setEnabled(false);
        ui->Frame_spinBox->setEnabled(false);
        ui->showviewer_btn->setEnabled(false);

        ui->Edit_GB->setEnabled(false);

    }
}


void MainWindow::update_UI_Fields()
{

    uint32_t magic = dpxsequence.currentheader.MagicNumber();
    ui->gfh_magic_tf->setText( QString::number(magic));

    ui->gfh_magicstring_label->setText( QString::fromUtf8((char *)(&magic),4));
    ui->gfh_imageoffset_tf->setText( QString::number(dpxsequence.currentheader.ImageOffset()));
    ui->gfh_version_tf->setText( dpxsequence.currentheader.version);
    ui->gfh_filesize_tf->setText( QString::number(dpxsequence.currentheader.FileSize()));
    ui->gfh_dittokey_tf->setText( QString::number(dpxsequence.currentheader.DittoKey()));
    ui->gfh_genericsize_tf->setText( QString::number(dpxsequence.currentheader.GenericSize()));
    ui->gfh_industrysize_tf->setText( QString::number(dpxsequence.currentheader.IndustrySize()));
    ui->gfh_usersize_tf->setText( QString::number(dpxsequence.currentheader.UserSize()));

    if (dpxsequence.autofilename_edit)
    {

        ui->gfh_filename_tf->setText( dpxsequence.dpxfiles[dpxsequence.currentframeindex- dpxsequence.firstFrameIndex]);
    }
    else
    {

        ui->gfh_filename_tf->setText( dpxsequence.currentheader.fileName);
    }
    ui->gfh_timedate_tf->setText( dpxsequence.currentheader.creationTimeDate);
    ui->gfh_creator_tf->setText( dpxsequence.currentheader.creator);

    ui->gfh_project_tf->setText( dpxsequence.currentheader.project);
    ui->gfh_copyright_tf->setText( dpxsequence.currentheader.copyright);
    ui->gfh_encryptkey_tf->setText( QString::number(dpxsequence.currentheader.EncryptKey()));
    ui->gfh_reserved_tf->setText( dpxsequence.currentheader.reserved1);

    ui->goh_xoffset_tf->setText( QString::number(dpxsequence.currentheader.XOffset()));
    ui->goh_yoffset_tf->setText( QString::number(dpxsequence.currentheader.YOffset()));
    ui->goh_xcenter_tf->setText( QString::number(dpxsequence.currentheader.XCenter()));
    ui->goh_ycenter_tf->setText( QString::number(dpxsequence.currentheader.YCenter()));
    ui->goh_xoriginalsize_tf->setText( QString::number(dpxsequence.currentheader.XOriginalSize()));
    ui->goh_yoriginalsize_tf->setText( QString::number(dpxsequence.currentheader.YOriginalSize()));
    ui->goh_xscannedsize_tf->setText( QString::number(dpxsequence.currentheader.XScannedSize()));
    ui->goh_yscannedsize_tf->setText( QString::number(dpxsequence.currentheader.YScannedSize()));
    ui->goh_filename_tf->setText( dpxsequence.currentheader.sourceImageFileName);
    ui->goh_timedate_tf->setText( dpxsequence.currentheader.sourceTimeDate);
    ui->goh_inputdevice_tf->setText( dpxsequence.currentheader.inputDevice);
    ui->goh_inputdevicesn_tf->setText( dpxsequence.currentheader.inputDeviceSerialNumber);

    ui->goh_border0_tf->setText( QString::number( dpxsequence.currentheader.Border(0)));
    ui->goh_border1_tf->setText( QString::number( dpxsequence.currentheader.Border(1)));
    ui->goh_border2_tf->setText( QString::number( dpxsequence.currentheader.Border(2)));
    ui->goh_border3_tf->setText( QString::number( dpxsequence.currentheader.Border(3)));
    ui->goh_aspectratio0_tf->setText( QString::number(dpxsequence.currentheader.AspectRatio(0)));
    ui->goh_aspectratio1_tf->setText( QString::number(dpxsequence.currentheader.AspectRatio(1)));
    ui->goh_aspect_label->setText( QString::number(float(dpxsequence.currentheader.AspectRatio(0)/dpxsequence.currentheader.AspectRatio(1)))+ ":1");
    ui->goh_reserved_tf->setText( dpxsequence.currentheader.reserved3);
    int numelements=dpxsequence.currentheader.NumberOfElements();

    // ui->gih_orientation_eb->setCurrentIndex(max(dpxsequence.currentheader.imageOrientation)
    ui->gih_orientation_tf->setText( QString::number(dpxsequence.currentheader.ImageOrientation()));
    ui->gih_orientation_label->setText(GetOrientationString(dpxsequence.currentheader.ImageOrientation()));
    ui->gih_elementcount_tf->setText( QString::number(dpxsequence.currentheader.NumberOfElements()));
    ui->gih_pixelsperline_tf->setText( QString::number(dpxsequence.currentheader.PixelsPerLine()));
    ui->gih_linesperelement_tf->setText( QString::number(dpxsequence.currentheader.LinesPerElement()));
    ui->gih_element_box->clear();
    for (int x=0; x<numelements; x++)
    {
        ui->gih_element_box->addItem(QString::number(x));

    }
    int elementnumberselected=  ui->gih_element_box->currentIndex();

    ui->gih_datasign_tf->setText( QString::number(dpxsequence.currentheader.DataSign(0)));
    ui->gih_lowdata_tf->setText( QString::number(dpxsequence.currentheader.chan[0].lowData));
    ui->gih_lowquantity_tf->setText( QString::number(dpxsequence.currentheader.chan[0].lowQuantity));
    ui->gih_highdata_tf->setText( QString::number(dpxsequence.currentheader.chan[0].highData));
    ui->gih_highquantity_tf->setText( QString::number(dpxsequence.currentheader.chan[0].highQuantity));

    ui->gih_descriptor_tf->setText( QString::number(dpxsequence.currentheader.chan[0].descriptor));
    ui->gih_descriptor_label->setText(GetDescriptorString(dpxsequence.currentheader.chan[0].descriptor));
    ui->gih_transfer_tf->setText( QString::number(dpxsequence.currentheader.chan[0].transfer));
    ui->gih_transfers_label->setText(GetTransferString(dpxsequence.currentheader.chan[0].transfer));
    ui->gih_colormetric_tf->setText( QString::number(dpxsequence.currentheader.chan[0].colorimetric));
    ui->gih_colorimetric_label->setText(GetColormetricString(dpxsequence.currentheader.chan[0].colorimetric));
    ui->gih_bitsize_tf->setText( QString::number(dpxsequence.currentheader.chan[0].bitDepth));

    ui->gih_packing_tf->setText( QString::number(dpxsequence.currentheader.chan[0].packing));
    ui->gih_encoding_tf->setText( QString::number(dpxsequence.currentheader.chan[0].encoding));
    ui->gih_dataoffset_tf->setText( QString::number(dpxsequence.currentheader.chan[0].dataOffset));
    ui->gih_eolpadding_tf->setText( QString::number(dpxsequence.currentheader.chan[0].endOfLinePadding));
    ui->gih_eoipaddding_tf->setText( QString::number(dpxsequence.currentheader.chan[0].endOfImagePadding));
    ui->gih_description_tf->setText( (dpxsequence.currentheader.chan[0].description));
    ui->gih_reserved_tf->setText( (dpxsequence.currentheader.reserved2));
    //QString * t = new QString(dpxsequence.currentheader.userSize) ;

    ui->ifh_filmmanid_tf->setText( (dpxsequence.currentheader.filmManufacturingIdCode));
    ui->ifh_filmtype_tf->setText((dpxsequence.currentheader.filmType));
    ui->ifh_offset_tf->setText( (dpxsequence.currentheader.perfsOffset));
    ui->ifh_prefix_tf->setText( (dpxsequence.currentheader.prefix));
    ui->ifh_count_tf->setText( (dpxsequence.currentheader.count));
    ui->ifh_format_tf->setText( (dpxsequence.currentheader.format));
    ui->ifh_framepostion_tf->setText( QString::number(dpxsequence.currentheader.FramePosition()));
    ui->ifh_sequencelength_tf->setText( QString::number(dpxsequence.currentheader.SequenceLength()));
    ui->ifh_heldcount_tf->setText( QString::number(dpxsequence.currentheader.HeldCount()));
    ui->ifh_framerate_tf->setText( QString::number(dpxsequence.currentheader.FrameRate()));
    ui->ifh_shutterangle_tf->setText( QString::number(dpxsequence.currentheader.ShutterAngle()));
    ui->ifh_frameid_tf->setText( (dpxsequence.currentheader.frameId));
    ui->ifh_slate_tf->setText( (dpxsequence.currentheader.slateInfo));
    ui->ifh_reserved_tf->setText( (dpxsequence.currentheader.reserved4));





    ui->ith_timecode_tf->setText( QString::number(dpxsequence.currentheader.timeCode));
    char * tc = new char[12];
    dpxsequence.currentheader.TimeCode(tc);
    ui->ith_timecode_label->setText( (tc));
    if( dpxsequence.changelist.contains(headerfield::timeCode))
    {
        //Compute timecode
        QPalette palette;
        palette.setColor(QPalette::Text,Qt::GlobalColor::red);
        ui->ith_timecode_label->setPalette(palette);
        ui->ith_timecode_label->setText( dpxsequence.computecurrenttimecode(dpxsequence.currentframeindex));

    }

    else
    {

        QPalette palette;
        palette.setColor(QPalette::Text,Qt::GlobalColor::black);
        ui->ith_timecode_label->setPalette(palette);

    }
    if( dpxsequence.changelist.contains(headerfield::keycode))
    {
        //Compute timecode
        QPalette palette;
        palette.setColor(QPalette::Text,Qt::GlobalColor::red);
        ui->Keycodelabel->setPalette(palette);
        ui->Keycodelabel->setText(dpxsequence.computecurrentkeycode(dpxsequence.currentframeindex));
        //ui->ith_timecode_label->setText( dpxsequence.computecurrenttimecode(dpxsequence.currentframeindex));

    }

    else
    {

        QPalette palette;
        palette.setColor(QPalette::Text,Qt::GlobalColor::black);

        ui->Keycodelabel->setPalette(palette);
        char * kk= new char[17];
        dpxsequence.currentheader.FilmEdgeCode(kk);
        ui->Keycodelabel->setText(kk);

    }

    ui->ith_userbits_tf->setText( QString::number(dpxsequence.currentheader.userBits));
    char * ub = new char[16];
    dpxsequence.currentheader.UserBits(ub);
    ui->ith_userbits_label->setText( (ub));
    ui->ith_interlace_tf->setText( QString::number(dpxsequence.currentheader.Interlace()));
    ui->ith_fieldnumber_tf->setText( QString::number(dpxsequence.currentheader.fieldNumber));
    ui->ith_videosignal_tf->setText( QString::number(dpxsequence.currentheader.videoSignal));
    ui->ith_padding_tf->setText( QString::number(dpxsequence.currentheader.zero));
    ui->ith_hsamplerate_tf->setText( QString::number(dpxsequence.currentheader.horizontalSampleRate));
    ui->ith_vsamplerate_tf->setText( QString::number(dpxsequence.currentheader.verticalSampleRate));
    ui->ith_framerate_tf->setText( QString::number(dpxsequence.currentheader.temporalFrameRate));
    ui->ith_timeoffset_tf->setText( QString::number(dpxsequence.currentheader.timeOffset));
    ui->ith_gamma_tf->setText( QString::number(dpxsequence.currentheader.gamma));
    ui->ith_blacklevel_tf->setText( QString::number(dpxsequence.currentheader.blackLevel));
    ui->ith_blackgain_tf->setText( QString::number(dpxsequence.currentheader.blackGain));
    ui->ith_breakpoint_tf->setText( QString::number(dpxsequence.currentheader.breakPoint));
    ui->ith_whitelevel_tf->setText( QString::number(dpxsequence.currentheader.whiteLevel));
    ui->ith_integrationtime_tf->setText( QString::number(dpxsequence.currentheader.integrationTimes));
    ui->ith_reserved_tf->setText( (dpxsequence.currentheader.reserved5));



   if (dpxsequence.currentheader.UserSize()>32)
{
    ui->ud_userlength_tf->setText(QString::number(dpxsequence.userheadersize));
    //  ui->ud_userid_tf->setText(t);

    QString  td ;
    QString  t = QString::fromUtf8(reinterpret_cast<const char*>(dpxsequence.userheaderdata.data()),32);
    if (!ui->viewhex_cb->isChecked())
        td = QString::fromUtf8(reinterpret_cast<const char*>(dpxsequence.userheaderdata.data()+32),dpxsequence.userheadersize-32);
    else
        td =   QString::fromUtf8(reinterpret_cast<const char*>( dpxsequence.userheaderdata.toHex().data()+32),dpxsequence.userheadersize-32);
    //ui->UD_textfield_tf->setText(QString::fromStdString(s.substr(32,s.length()-32))  );

    dpxsequence.userid_current=t;
    ui->ud_userid_tf->setText(t);
    QSignalBlocker blocker{ui->UD_textedit_tf};

    ui->UD_textedit_tf->clear();
    ui->UD_textedit_tf->appendPlainText(td);
    blocker.unblock();
   }

    ui->current_file_label->setText("Current File: " + dpxsequence.GetFrameFilename(dpxsequence.currentframeindex));
}

void MainWindow::update_Read_DPX(int position)

{
    if(dpx_window->isVisible())
    {dpxsequence.ReadFile(position,true);
        dpx_window->ShowFrame(dpxsequence.image);
    }
    else
        dpxsequence.ReadFile(position,false);
    ui->UD_textedit_tf->setMaximumBlockCount(dpxsequence.userheadersize-32);
    update_UI_Fields();
}

void MainWindow::on_Frame_horizontalSlider_sliderMoved(int position)
{
    ui->Frame_spinBox->setValue(position);

}

void MainWindow::on_Frame_spinBox_valueChanged(int arg1)
{
    ui->Frame_horizontalSlider->setValue(arg1);
    update_Read_DPX( arg1);
}



void MainWindow::on_viewer_cb_clicked()
{


}

void MainWindow::on_showviewer_btn_clicked()
{


    if(dpxsequence.isready && !dpx_window->isVisible() )
    {
        //  if(dpxsequence.currentheader.chan[0].bitDepth==10 && dpxsequence.currentheader.chan[0].descriptor==50)
        {

            dpx_window->dpx_image_height=dpxsequence.Height();
            dpx_window->dpx_image_width=dpxsequence.Width();
            dpx_window->show();
            update_Read_DPX(ui->Frame_spinBox->value());
            //update_Read_DPX(ui->Frame_spinBox->value());
        }
        /*
        else
        {

            QMessageBox msgBox;
            msgBox.setWindowTitle("Only RGB 10Bit Supported");
            msgBox.setText("Only RGB 10Bit Supported");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec() ;
        }
*/
    }

}

void MainWindow::on_writeheaders_btn_clicked()
{
    int first =markerinframe ;
    int last = markeroutframe;

    if(!dpxsequence.changelist.isEmpty())
    {
        //close_editmode();
        ui->progressBar->setEnabled(true);
        qApp->processEvents();
        QString flist="  ";
        foreach(headerfield chf , dpxsequence.changelist )
        {

            flist+=dpxsequence.headerfieldlist[chf];
            flist+=", ";
        }
        confirmwriteDialog cwd;
        cwd.setdata( "The following fields will be written:"+flist.remove(flist.length()-2,2)+  + "\nFrom frame index: "+
                     QString::number(first)+ " To frame index: "+ QString::number(last))
                ;
        // act on dialog return code

        int dialogCode = cwd.exec();
        if(dialogCode == QDialog::Accepted)
        { // YesButton clicked }
            ui->progressBar->setMinimum(first);
            ui->progressBar->setMaximum(last);
            QMetaObject::invokeMethod(this,"progressbarupdate",Qt::ConnectionType::QueuedConnection,Q_ARG(int,first),Q_ARG(int,last));
            for (int i=first; i<=last; i++)
            {



                dpxsequence.updateheaders(i);
                QMetaObject::invokeMethod(this,"progressbarupdate",Qt::ConnectionType::QueuedConnection,Q_ARG(int,i),Q_ARG(int,last));
                qApp->processEvents();

            }

            close_editmode();
            ui->progressBar->setValue(0);

            qApp->processEvents();
            QMessageBox msgBox;
            msgBox.setWindowTitle("Header Write Complete");
            msgBox.setText("DPX headers have been modified");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec() ;
            ui->progressBar->setEnabled(false);
        }
    }

    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("No Changes");
        msgBox.setText("No Fields Have Been Modified");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec() ;

    }
}




void MainWindow::on_SetInBtn_clicked()
{

    markerinframe =  ui->Frame_spinBox->value();
    ui->inlabel->setText(QString::number(markerinframe));
}

void MainWindow::on_SetOutBtn_clicked()
{
    markeroutframe =  ui->Frame_spinBox->value();
    ui->outlabel->setText(QString::number(markeroutframe));
}
void MainWindow::close_editmode()
{
    editmode=0;
    ui->edittype_GB->setEnabled(false);
    ui->editbasic_rb->setChecked(true);
    ui->writeheaders_btn->setEnabled(true);
    ui->EnableEditCB->setCheckState(Qt::CheckState::Unchecked);
    QPalette palette;
    palette.setColor(QPalette::Text,Qt::black);
    ui->UD_textedit_tf->setPalette(palette);
    if (dpxsequence.isready)
        update_Read_DPX(ui->Frame_spinBox->value());





    foreach(QLineEdit* dpx_h_le , header_textfields )
    {

        dpx_h_le->setReadOnly(true);
        setQLineEditTextColor(Qt::black,dpx_h_le);
        setQLineEditBaseColor(Qt::white,dpx_h_le);
    }

}
void MainWindow::on_ValidateButton_clicked()
{

    dpxsequence.validateheaders();
}

void MainWindow::on_advedit_rb_clicked(bool checked)
{
    if (ui->EnableEditCB->isChecked())
    {
        editmode = 2;

        change_textfieldsforedit();
    }
}

void MainWindow::on_alledit_rb_clicked(bool checked)
{ if (ui->EnableEditCB->isChecked())
    {
        editmode = 3;

        change_textfieldsforedit();
    }
}

void MainWindow::on_editbasic_rb_clicked(bool checked)
{ if (ui->EnableEditCB->isChecked())
    {
        editmode = 1;
        change_textfieldsforedit();
    }
}
void MainWindow::change_textfieldsforedit()
{

    foreach(QLineEdit* dpx_h_le , header_textfields )
    {

        dpx_h_le->setReadOnly(true);
        setQLineEditTextColor(Qt::black,dpx_h_le);
        setQLineEditBaseColor(Qt::white,dpx_h_le);
    }
    if (editmode==1)
    {
        foreach(QLineEdit* dpx_h_le , header_textfields_normaledit )
        {
            dpx_h_le->setReadOnly(false);

            setQLineEditBaseColor(Qt::cyan,dpx_h_le);

        }
    }
    else if (editmode==2)
    {
        foreach(QLineEdit* dpx_h_le , header_textfields_advedit )
        {
            dpx_h_le->setReadOnly(false);

            setQLineEditBaseColor(Qt::cyan,dpx_h_le);

        }
    }
    else  //mode 3
    {
        foreach(QLineEdit* dpx_h_le , header_textfields_alledit)
        {
            dpx_h_le->setReadOnly(false);

            setQLineEditBaseColor(Qt::cyan,dpx_h_le);

        }
    }



}
void MainWindow::begin_editmode (int edit_type)
{

    dpxsequence.assignheadereditcopy();

    ui->edittype_GB->setEnabled(true);

    ui->writeheaders_btn->setEnabled(true);


}
void MainWindow::on_EnableEditCB_stateChanged(int arg1)
{

    if (arg1 && dpxsequence.isready)
    {
        ui->Frame_horizontalSlider->setEnabled(false);
        ui->Frame_spinBox->setEnabled(false);
        editmode=1;
        begin_editmode(1);
        change_textfieldsforedit();
        //     ui->en
        ui->SetTC_btn->setEnabled(true);
        ui->setkeycode_btn->setEnabled(true);
        ui->SetUB_btn->setEnabled(true);

        ui->gfh_autofilename_cb->setEnabled(true);
        if(dpxsequence.hasuserdata)
            ui->UD_textedit_tf->setReadOnly(false); //  ->setEnabled(true);
       ui->progressBar->setValue(0);
    }
    else
    {
        close_editmode();
        ui->Frame_horizontalSlider->setEnabled(true);
        ui->Frame_spinBox->setEnabled(true);
        ui->SetTC_btn->setEnabled(false);
        ui->setkeycode_btn->setEnabled(false);
        ui->SetUB_btn->setEnabled(false);

        ui->gfh_autofilename_cb->setEnabled(false);

        ui->UD_textedit_tf->setReadOnly(true);
        dpxsequence.changelist.clear();
    }


}

void MainWindow::on_EnableEditAdvanced_CB_stateChanged(int arg1)
{

    if (arg1)
    {
        editmode=2;
        begin_editmode(2);
    }
    else
    {

        close_editmode();
    }



}

void MainWindow::on_EnableEditAllCB_stateChanged(int arg1)
{

    if (arg1)
    {
        editmode=3;
        begin_editmode(3);
    }
    else
    {

        close_editmode();
    }

}

void MainWindow::reset_texfields()
{









}
void MainWindow::onTextEdit(const QString& text)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    if (edit)
    {
        // Do something with QLineEdit

        QPalette *palette = new QPalette();
        palette->setColor(QPalette::Text,Qt::red);
        edit->setPalette(*palette);
    }
    else
    {
        // Just to make sure that you have not make mistake with connecting signals

    }
}

void MainWindow::setQLineEditBaseColor(Qt::GlobalColor icolor,QLineEdit * iQLE)
{
    iQLE->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Base,icolor);
    iQLE->setPalette(palette);


}
void MainWindow::setQLineEditTextColor(Qt::GlobalColor icolor,QLineEdit * iQlE)
{

    QPalette palette;
    palette.setColor(QPalette::Text,icolor);
    iQlE->setPalette(palette);


}

//*********************Generic File Header Changes ******************************************

void MainWindow::on_gfh_filename_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetFileName(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.fileName;
    QString O =   dpxsequence.currentheader.fileName;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::fileName);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::fileName);
    }
}

void MainWindow::on_gfh_timedate_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetCreationTimeDate(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.creationTimeDate;
    QString O =   dpxsequence.currentheader.creationTimeDate;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::creationTimeDate);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::creationTimeDate);
    }

}

void MainWindow::on_gfh_creator_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetCreator(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.creator;
    QString O =   dpxsequence.currentheader.creator;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::creator);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::creator);
    }

}


void MainWindow::on_gfh_project_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetProject(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.project;
    QString O =   dpxsequence.currentheader.project;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::project);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::project);
    }
}


void MainWindow::on_gfh_copyright_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetCopyright(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.copyright;
    QString O =   dpxsequence.currentheader.copyright;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::copyright);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::copyright);
    }
}


void MainWindow::on_gfh_encryptkey_tf_textEdited(const QString &arg1)
{

    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetEncryptKey(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.encryptKey;
    uint32_t O =   dpxsequence.currentheader.encryptKey;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::encryptKey);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::encryptKey);
    }
}
//*************************************Advanced GFH Edit Fields**************************************************
void MainWindow::on_gfh_magic_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.magicNumber=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.magicNumber;
    uint32_t O =   dpxsequence.currentheader.magicNumber;
    ui->gfh_magicstring_label->setText( QString::fromUtf8((char *)(&dpxsequence.currentheader_edited.magicNumber),4));
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::magicNumber);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::magicNumber);
    }
}

void MainWindow::on_gfh_imageoffset_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetImageOffset(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.imageOffset;
    uint32_t O =   dpxsequence.currentheader.imageOffset;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::imageOffset);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::imageOffset);
    }
}

void MainWindow::on_gfh_version_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetVersion(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.version;
    QString O =   dpxsequence.currentheader.version;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::version);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::version);
    }
}

void MainWindow::on_gfh_filesize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetFileSize(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.fileSize;
    uint32_t O =   dpxsequence.currentheader.fileSize;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::fileSize);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::fileSize);
    }
}

void MainWindow::on_gfh_dittokey_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetDittoKey(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.dittoKey;
    uint32_t O =   dpxsequence.currentheader.dittoKey;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::dittoKey);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::dittoKey);
    }
}

void MainWindow::on_gfh_genericsize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.genericSize=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.genericSize;
    uint32_t O =   dpxsequence.currentheader.genericSize;


    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::genericSize);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::genericSize);
    }
}

void MainWindow::on_gfh_industrysize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.industrySize=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.industrySize;
    uint32_t O =   dpxsequence.currentheader.industrySize;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::industrySize);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::industrySize);
    }
}

void MainWindow::on_gfh_usersize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.userSize=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.userSize;
    uint32_t O =   dpxsequence.currentheader.userSize;
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::userSize);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::userSize);
    }
}
//***********************************************************************************************

//*********************Generic Image Header Changes ******************************************

void MainWindow::on_gih_orientation_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetImageOrientation( dpx::Orientation(arg1.toUInt()));
    uint32_t E =   dpxsequence.currentheader_edited.imageOrientation;
    uint32_t O =   dpxsequence.currentheader.imageOrientation;
    ui->gih_orientation_label->setText(GetOrientationString(dpxsequence.currentheader_edited.imageOrientation));
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::imageOrientation);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::imageOrientation);
    }
}

void MainWindow::on_gih_elementcount_tf_textEdited(const QString &arg1)
{

}

void MainWindow::on_gih_pixelsperline_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetPixelsPerLine(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.pixelsPerLine;
    uint32_t O =   dpxsequence.currentheader.pixelsPerLine;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::pixelsPerLine);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::pixelsPerLine);
    }
}

void MainWindow::on_gih_linesperelement_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetLinesPerElement(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.linesPerElement;
    uint32_t O =   dpxsequence.currentheader.linesPerElement;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::linesPerElement);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::linesPerElement);
    }
}

void MainWindow::on_gih_datasign_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetDataSign(0,arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].dataSign;
    uint32_t O =   dpxsequence.currentheader.chan[0].dataSign;
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::dataSign);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::dataSign);
    }
}

void MainWindow::on_gih_packing_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].packing=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].packing;
    uint32_t O =   dpxsequence.currentheader.chan[0].packing;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::packing);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::packing);
    }
}

void MainWindow::on_gih_lowdata_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].lowData=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].lowData;
    uint32_t O =   dpxsequence.currentheader.chan[0].lowData;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::lowData);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::lowData);
    }
}

void MainWindow::on_gih_lowquantity_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].lowQuantity=(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.chan[0].lowQuantity;
    float O =   dpxsequence.currentheader.chan[0].lowQuantity;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::lowQuantity);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::lowQuantity);
    }
}

void MainWindow::on_gih_highdata_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].highData=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].highData;
    uint32_t O =   dpxsequence.currentheader.chan[0].highData;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::highData);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::highData);
    }

}

void MainWindow::on_gih_highquantity_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].highQuantity=(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.chan[0].highQuantity;
    float O =   dpxsequence.currentheader.chan[0].highQuantity;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::highQuantity);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::highQuantity);
    }
}

void MainWindow::on_gih_encoding_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].encoding=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].encoding;
    uint32_t O =   dpxsequence.currentheader.chan[0].encoding;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::encoding);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::encoding);
    }
}

void MainWindow::on_gih_dataoffset_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].dataOffset=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].dataOffset;
    uint32_t O =   dpxsequence.currentheader.chan[0].dataOffset;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::dataOffset);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::dataOffset);
    }
}

void MainWindow::on_gih_eolpadding_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].endOfLinePadding=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].endOfLinePadding;
    uint32_t O =   dpxsequence.currentheader.chan[0].endOfLinePadding;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::endOfLinePadding);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::endOfLinePadding);
    }
}

void MainWindow::on_gih_eoipaddding_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].endOfImagePadding=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].endOfImagePadding;
    uint32_t O =   dpxsequence.currentheader.chan[0].endOfImagePadding;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::endOfImagePadding);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::endOfImagePadding);
    }
}

void MainWindow::on_gih_descriptor_tf_textEdited(const QString &arg1)
{

    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].descriptor=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].descriptor;
    uint32_t O =   dpxsequence.currentheader.chan[0].descriptor;
    ui->gih_descriptor_label->setText(GetDescriptorString(dpxsequence.currentheader_edited.chan[0].descriptor));
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::descriptor);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::descriptor);
    }
}

void MainWindow::on_gih_transfer_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].transfer=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].transfer;
    uint32_t O =   dpxsequence.currentheader.chan[0].transfer;
    ui->gih_transfers_label->setText(GetTransferString(dpxsequence.currentheader_edited.chan[0].transfer));
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::transfer);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::transfer);
    }
}

void MainWindow::on_gih_colormetric_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].colorimetric=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].colorimetric;
    uint32_t O =   dpxsequence.currentheader.chan[0].colorimetric;
    ui->gih_colorimetric_label->setText(GetColormetricString(dpxsequence.currentheader_edited.chan[0].colorimetric));
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::colorimetric);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::colorimetric);
    }
}

void MainWindow::on_gih_bitsize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.chan[0].bitDepth=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.chan[0].bitDepth;
    uint32_t O =   dpxsequence.currentheader.chan[0].bitDepth;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::bitDepth);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::bitDepth);
    }
}

void MainWindow::on_gih_description_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetDescription(0,arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.chan[0].description;
    QString O =   dpxsequence.currentheader.chan[0].description;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::description);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::description);
    }
}
//***********************************************************************************************
//*********************Generic Orientation Header Changes ******************************************



void MainWindow::on_goh_xoffset_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetXOffset(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.xOffset;
    uint32_t O =   dpxsequence.currentheader.xOffset;
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::xOffset);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::xOffset);
    }

}

void MainWindow::on_goh_yoffset_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetYOffset(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.yOffset;
    uint32_t O =   dpxsequence.currentheader.yOffset;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::yOffset);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::yOffset);
    }


}

void MainWindow::on_goh_xcenter_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetXCenter(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.xCenter;
    float O =   dpxsequence.currentheader.xCenter;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::xCenter);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::xCenter);
    }
}

void MainWindow::on_goh_ycenter_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetYCenter(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.yCenter;
    float O =   dpxsequence.currentheader.yCenter;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::yCenter);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::yCenter);
    }
}
void MainWindow::on_goh_xoriginalsize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetXOriginalSize(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.xOriginalSize;
    uint32_t O =   dpxsequence.currentheader.xOriginalSize;
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::xOriginalSize);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::xOriginalSize);
    }
}

void MainWindow::on_goh_yoriginalsize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetYOriginalSize(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.yOriginalSize;
    uint32_t O =   dpxsequence.currentheader.yOriginalSize;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::yOriginalSize);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::yOriginalSize);
    }

}
void MainWindow::on_goh_yscannedsize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetYScannedSize(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.YScannedSize();
    float O =   dpxsequence.currentheader.YScannedSize();

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::yScannedSize);

    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::yScannedSize);
    }
}

void MainWindow::on_goh_xscannedsize_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetXScannedSize(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.XScannedSize();
    float O =   dpxsequence.currentheader.XScannedSize();

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::xScannedSize);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::xScannedSize);
    }
}
void MainWindow::on_goh_filename_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetSourceImageFileName(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.sourceImageFileName;
    QString O =   dpxsequence.currentheader.sourceImageFileName;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::sourceImageFileName);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::sourceImageFileName);
    }
}

void MainWindow::on_goh_timedate_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetSourceTimeDate(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.sourceTimeDate;
    QString O =   dpxsequence.currentheader.sourceTimeDate;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::sourceTimeDate);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::sourceTimeDate);
    }
}

void MainWindow::on_goh_inputdevice_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetInputDevice(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.inputDevice;
    QString O =   dpxsequence.currentheader.inputDevice;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::inputDevice);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::inputDevice);
    }
}

void MainWindow::on_goh_inputdevicesn_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetInputDeviceSerialNumber(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.inputDeviceSerialNumber;
    QString O =   dpxsequence.currentheader.inputDeviceSerialNumber;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::inputDeviceSerialNumber);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::inputDeviceSerialNumber);
    }
}
void MainWindow::on_goh_border0_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetBorder(0,arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.border[0];
    uint32_t O =   dpxsequence.currentheader.border[0];

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::border);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::border);
    }
}

void MainWindow::on_goh_border1_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetBorder(1,arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.border[1];
    uint32_t O =   dpxsequence.currentheader.border[1];

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::border);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::border);
    }
}

void MainWindow::on_goh_border2_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetBorder(2,arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.border[2];
    uint32_t O =   dpxsequence.currentheader.border[2];

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::border);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::border);
    }
}

void MainWindow::on_goh_border3_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetBorder(3,arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.border[3];
    uint32_t O =   dpxsequence.currentheader.border[3];

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::border);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::border);
    }
}

void MainWindow::on_goh_aspectratio0_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetAspectRatio(0,arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.aspectRatio[0];
    uint32_t O =   dpxsequence.currentheader.aspectRatio[0];
    ui->goh_aspect_label->setText( QString::number(float(dpxsequence.currentheader_edited.aspectRatio[0]/dpxsequence.currentheader_edited.aspectRatio[1]))+ ":1");

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::aspectRatio);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::aspectRatio);
    }
}

void MainWindow::on_goh_aspectratio1_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetAspectRatio(1,arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.aspectRatio[1];
    uint32_t O =   dpxsequence.currentheader.aspectRatio[1];
    ui->goh_aspect_label->setText( QString::number(float(dpxsequence.currentheader_edited.aspectRatio[0]/dpxsequence.currentheader_edited.aspectRatio[1]))+ ":1");

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::aspectRatio);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::aspectRatio);
    }
}
//***********************************************************************************************
//*********************Industry Film Header Changes ******************************************

void MainWindow::on_ifh_filmmanid_tf_textEdited(const QString &arg1)
{

}

void MainWindow::on_ifh_filmtype_tf_textEdited(const QString &arg1)
{

}

void MainWindow::on_ifh_offset_tf_textEdited(const QString &arg1)
{

}

void MainWindow::on_ifh_prefix_tf_textEdited(const QString &arg1)
{

}

void MainWindow::on_ifh_count_tf_textEdited(const QString &arg1)
{

}

void MainWindow::on_ifh_format_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetFormat(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.format;
    QString O =   dpxsequence.currentheader.format;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::format);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::format);
    }
}

void MainWindow::on_ifh_framepostion_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.framepositionedit = arg1.toInt();
    dpxsequence.framepositionorig = dpxsequence.currentframeindex;
    dpxsequence.currentheader_edited.SetFramePosition(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.framePosition;
    uint32_t O =   dpxsequence.currentheader.framePosition;
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::framePosition);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::framePosition);
    }
}

void MainWindow::on_ifh_sequencelength_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetSequenceLength(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.sequenceLength;
    uint32_t O =   dpxsequence.currentheader.sequenceLength;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::sequenceLength);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::sequenceLength);
    }
}

void MainWindow::on_ifh_heldcount_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetHeldCount(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.heldCount;
    uint32_t O =   dpxsequence.currentheader.heldCount;
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::heldCount);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::heldCount);
    }
}

void MainWindow::on_ifh_framerate_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetFrameRate(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.frameRate;
    float O =   dpxsequence.currentheader.frameRate;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::frameRate);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::frameRate);
    }
}

void MainWindow::on_ifh_shutterangle_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetShutterAngle(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.shutterAngle;
    float O =   dpxsequence.currentheader.shutterAngle;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::shutterAngle);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::shutterAngle);
    }
}

void MainWindow::on_ifh_frameid_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetFrameId(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.frameId;
    QString O =   dpxsequence.currentheader.frameId;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::frameId);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::frameId);
    }
}

void MainWindow::on_ifh_slate_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.currentheader_edited.SetSlateInfo(arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.slateInfo;
    QString O =   dpxsequence.currentheader.slateInfo;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::slateInfo);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::slateInfo);
    }
}
//***********************************************************************************************
//*********************Industry Television Header Changes ******************************************



void MainWindow::on_ith_timecode_tf_textEdited(const QString &arg1)
{
    // EDL
}

void MainWindow::on_ith_userbits_tf_textEdited(const QString &arg1)
{
    // EDL
}

void MainWindow::on_ith_interlace_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetInterlace(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.interlace;
    uint32_t O =   dpxsequence.currentheader.interlace
            ;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::interlace);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::interlace);
    }
}

void MainWindow::on_ith_fieldnumber_tf_textEdited(const QString &arg1)
{
    // EDL
}

void MainWindow::on_ith_videosignal_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.videoSignal=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.videoSignal;
    uint32_t O =   dpxsequence.currentheader.videoSignal;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::videoSignal);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::videoSignal);
    }
}

void MainWindow::on_ith_padding_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.zero=(arg1.toUInt());
    uint32_t E =   dpxsequence.currentheader_edited.zero;
    uint32_t O =   dpxsequence.currentheader.zero;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::zero);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::zero);
    }
}

void MainWindow::on_ith_hsamplerate_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetHorizontalSampleRate(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.horizontalSampleRate;
    float O =   dpxsequence.currentheader.horizontalSampleRate;
    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::horizontalSampleRate);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::horizontalSampleRate);
    }
}

void MainWindow::on_ith_vsamplerate_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetVerticalSampleRate(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.verticalSampleRate;
    float O =   dpxsequence.currentheader.verticalSampleRate;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::verticalSampleRate);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::verticalSampleRate);
    }
}

void MainWindow::on_ith_framerate_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetTemporalFrameRate(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.temporalFrameRate;
    float O =   dpxsequence.currentheader.temporalFrameRate;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::temporalFrameRate);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::temporalFrameRate);
    }
}

void MainWindow::on_ith_timeoffset_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetTimeOffset(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.timeOffset;
    float O =   dpxsequence.currentheader.timeOffset;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::timeOffset);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::timeOffset);
    }
}

void MainWindow::on_ith_gamma_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetGamma(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.gamma;
    float O =   dpxsequence.currentheader.gamma;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::vgamma);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::vgamma);
    }
}

void MainWindow::on_ith_blacklevel_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetBlackLevel(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.blackLevel;
    float O =   dpxsequence.currentheader.blackLevel;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::blackLevel);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::blackLevel);
    }
}

void MainWindow::on_ith_blackgain_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetBlackGain(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.blackGain;
    float O =   dpxsequence.currentheader.blackGain;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::blackGain);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::blackGain);
    }
}

void MainWindow::on_ith_breakpoint_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetBreakPoint(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.breakPoint;
    float O =   dpxsequence.currentheader.breakPoint;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::breakPoint);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::breakPoint);
    }
}

void MainWindow::on_ith_whitelevel_tf_textEdited(const QString &arg1)
{

    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetWhiteLevel(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.whiteLevel;
    float O =   dpxsequence.currentheader.whiteLevel;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::whiteLevel);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::whiteLevel);
    }
}

void MainWindow::on_ith_integrationtime_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    dpxsequence.currentheader_edited.SetIntegrationTimes(arg1.toFloat());
    float E =   dpxsequence.currentheader_edited.integrationTimes;
    float O =   dpxsequence.currentheader.integrationTimes;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::integrationTimes);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::integrationTimes);
    }
}

void MainWindow::on_ud_userid_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    dpxsequence.userid_edit =arg1;
    QString E =   dpxsequence.userid_edit;
    QString O =   dpxsequence.userid_current;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::userid);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::userid);
    }
}
void MainWindow::assignQLEValidators()
{
    auto di = new QIntValidator();
    auto df = new QDoubleValidator();
    auto db = new QIntValidator(0,255);

    di->setBottom(0);
    db->setBottom(0);
    ui->gfh_magic_tf->setValidator(di);
    ui->gfh_imageoffset_tf->setValidator(di);
    ui->gfh_filesize_tf->setValidator(di);
    ui->gfh_dittokey_tf->setValidator(di);
    ui->gfh_genericsize_tf->setValidator(di);
    ui->gfh_industrysize_tf->setValidator(di);
    ui->gfh_usersize_tf->setValidator(di);
    ui->gfh_encryptkey_tf->setValidator(di);
    ui->goh_xoffset_tf->setValidator(di);
    ui->goh_yoffset_tf->setValidator(di);
    ui->goh_xcenter_tf->setValidator(df);
    ui->goh_ycenter_tf->setValidator(df);
    ui->goh_xoriginalsize_tf->setValidator(di);
    ui->goh_yoriginalsize_tf->setValidator(di);
    ui->goh_xscannedsize_tf->setValidator(df);
    ui->goh_yscannedsize_tf->setValidator(df);
    ui->goh_border0_tf->setValidator(di);
    ui->goh_border1_tf->setValidator(di);
    ui->goh_border2_tf->setValidator(di);
    ui->goh_border3_tf->setValidator(di);
    ui->goh_aspectratio0_tf->setValidator(di);
    ui->goh_aspectratio1_tf->setValidator(di);

    ui->gih_orientation_tf->setValidator(di);
    ui->gih_elementcount_tf->setValidator(di);
    ui->gih_pixelsperline_tf->setValidator(di);
    ui->gih_linesperelement_tf->setValidator(di);
    ui->gih_datasign_tf->setValidator(di);
    ui->gih_lowdata_tf->setValidator(di);
    ui->gih_lowquantity_tf->setValidator(df);
    ui->gih_highdata_tf->setValidator(di);
    ui->gih_highquantity_tf->setValidator(df);
    ui->gih_descriptor_tf->setValidator(db);
    ui->gih_transfer_tf->setValidator(db);
    ui->gih_colormetric_tf->setValidator(db);
    ui->gih_bitsize_tf->setValidator(db);
    ui->gih_packing_tf->setValidator(di);
    ui->gih_encoding_tf->setValidator(di);
    ui->gih_dataoffset_tf->setValidator(di);
    ui->gih_eolpadding_tf->setValidator(di);
    ui->gih_eoipaddding_tf->setValidator(di);
    ui->ifh_framepostion_tf->setValidator(di);
    ui->ifh_sequencelength_tf->setValidator(di);
    ui->ifh_heldcount_tf->setValidator(di);
    ui->ifh_framerate_tf->setValidator(df);
    ui->ifh_shutterangle_tf->setValidator(df);
    ui->ith_interlace_tf->setValidator(db);
    ui->ith_fieldnumber_tf->setValidator(db);
    ui->ith_videosignal_tf->setValidator(db);
    ui->ith_padding_tf->setValidator(db);
    ui->ith_hsamplerate_tf->setValidator(df);
    ui->ith_vsamplerate_tf->setValidator(df);
    ui->ith_framerate_tf->setValidator(df);
    ui->ith_timeoffset_tf->setValidator(df);
    ui->ith_gamma_tf->setValidator(df);
    ui->ith_blacklevel_tf->setValidator(df);
    ui->ith_blackgain_tf->setValidator(df);
    ui->ith_breakpoint_tf->setValidator(df);
    ui->ith_whitelevel_tf->setValidator(df);
    ui->ith_integrationtime_tf->setValidator(df);

}

void MainWindow::on_SetTC_btn_clicked()
{
    // TCSetDialog TC();
    TCSetDialog * TC = new TCSetDialog();
    //  TC->exec();
    TC->ui_dataset("Enter Current Frame Timecode");
    int dialogCode =  TC->exec();

    // act on dialog return code
    if(dialogCode == QDialog::Accepted)
    { // YesButton clicked }
        char * tcc = new char[12];
        // tc=   TC->UE_Timecode.toStdString().;

        const char * tcsp  = TC->UE_Timecode.toStdString().c_str();
        dpxsequence.currentheader_edited.SetTimeCode(   tcsp);


        // dpxsequence.currentheader_edited.timeCode=tc;
        dpxsequence.currentheader_edited.TimeCode(tcc);

        ui->ith_timecode_label->setText( tcc);
        dpxsequence.timecodeeditstring = tcsp;
        dpxsequence.timecodeeditfnum = ui->Frame_spinBox->value();

        uint32_t E =   dpxsequence.currentheader_edited.timeCode;
        uint32_t O =   dpxsequence.currentheader.timeCode;


        if (E!=O)
        {
            //  setQLineEditTextColor(Qt::red,ui->ith_timecode_label);
            setQLineEditTextColor(Qt::red,ui->ith_timecode_tf);
            dpxsequence.changelist.insert(headerfield::timeCode);
        }
        else
        {
            // setQLineEditTextColor(Qt::black,edit);
            dpxsequence.changelist.remove(headerfield::timeCode);
            setQLineEditTextColor(Qt::red,ui->ith_timecode_tf);
        }

        update_UI_Fields();

    }
}

void MainWindow::on_gfh_autofilename_cb_stateChanged(int arg1)
{

    if(arg1==0)
    {

        dpxsequence.autofilename_edit =false;
        dpxsequence.changelist.remove(headerfield::fileName);
        QPalette palette;
        palette.setColor(QPalette::Text,Qt::GlobalColor::black);
        ui->gfh_filename_tf->setPalette(palette);
        update_UI_Fields();

    }
    else
    {

        dpxsequence.autofilename_edit=true;
        QPalette palette;
        dpxsequence.changelist.insert(headerfield::fileName);
        palette.setColor(QPalette::Text,Qt::GlobalColor::red);
        ui->gfh_filename_tf->setPalette(palette);
        update_UI_Fields();

    }


}

void MainWindow::on_viewhex_cb_stateChanged(int arg1)
{
    update_UI_Fields();
}

void MainWindow::on_UD_textedit_tf_textChanged()
{
    // QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    // dpxsequence.user
    //  dpxsequence.userheaderd
    strcpy(dpxsequence.userheaderdataedited.data()+32,ui->UD_textedit_tf->toPlainText().toStdString().c_str());
    dpxsequence.userdata_edit=ui->UD_textedit_tf->toPlainText().toLocal8Bit();
    QString E =     QString::fromUtf8(reinterpret_cast<const char*>(dpxsequence.userheaderdataedited.data()+32),dpxsequence.userheadersize-32);

    QString O =   QString::fromUtf8(reinterpret_cast<const char*>(dpxsequence.userheaderdata.data()+32),dpxsequence.userheadersize-32);;

    if (E!=O)
    {

        QPalette palette;
        palette.setColor(QPalette::Text,Qt::red);
        ui->UD_textedit_tf->setPalette(palette);
        dpxsequence.changelist.insert(headerfield::userdata);
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text,Qt::black);
        ui->UD_textedit_tf->setPalette(palette);
        dpxsequence.changelist.remove(headerfield::userdata);
    }
}

void MainWindow::on_SetUB_btn_clicked()
{
    // TCSetDialog TC();
    TCSetDialog * TC = new TCSetDialog();
    //  TC->exec();
    TC->ui_dataset("Enter Current Frame Timecode");
    int dialogCode =  TC->exec();

    // act on dialog return code
    if(dialogCode == QDialog::Accepted)
    { // YesButton clicked }
        char * tcc = new char[12];
        // tc=   TC->UE_Timecode.toStdString().;

        const char * tcsp  = TC->UE_Timecode.toStdString().c_str();
        dpxsequence.currentheader_edited.SetUserBits(   tcsp);


        // dpxsequence.currentheader_edited.timeCode=tc;
        dpxsequence.currentheader_edited.UserBits(tcc);

        ui->ith_userbits_label->setText( tcc);
        ui->ith_userbits_tf->setText( tcc);
        //   dpxsequence.timecodeeditstring = tcsp;
        //    dpxsequence.timecodeeditfnum = ui->Frame_spinBox->value();

        uint32_t E =   dpxsequence.currentheader_edited.userBits;
        uint32_t O =   dpxsequence.currentheader.userBits;


        if (E!=O)
        {
            //  setQLineEditTextColor(Qt::red,ui->ith_timecode_label);
            setQLineEditTextColor(Qt::red,ui->ith_userbits_tf);
            dpxsequence.changelist.insert(headerfield::userBits);
        }
        else
        {
            // setQLineEditTextColor(Qt::black,edit);
            dpxsequence.changelist.remove(headerfield::userBits);
            setQLineEditTextColor(Qt::red,ui->ith_userbits_tf);
        }


    }
}

void MainWindow::on_gfh_reserved_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    //  dpxsequence.currentheader_edited.reserved1(arg1.toStdString().c_str());
    EmptyString(dpxsequence.currentheader_edited.reserved1,104);
    strcpy(dpxsequence.currentheader_edited.reserved1,arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.reserved1;
    QString O =   dpxsequence.currentheader.reserved1;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::reserved1);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::reserved1);
    }
}

void MainWindow::on_gih_reserved_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    //  dpxsequence.currentheader_edited.reserved1(arg1.toStdString().c_str());
    EmptyString(dpxsequence.currentheader_edited.reserved2,52);
    strcpy(dpxsequence.currentheader_edited.reserved2,arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.reserved2;
    QString O =   dpxsequence.currentheader.reserved2;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::reserved2);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::reserved2);
    }
}

void MainWindow::on_goh_reserved_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    //  dpxsequence.currentheader_edited.reserved1(arg1.toStdString().c_str());
    EmptyString(dpxsequence.currentheader_edited.reserved3,20);
    strcpy(dpxsequence.currentheader_edited.reserved3,arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.reserved3;
    QString O =   dpxsequence.currentheader.reserved3;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::reserved3);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::reserved3);
    }
}

void MainWindow::on_ifh_reserved_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    //  dpxsequence.currentheader_edited.reserved1(arg1.toStdString().c_str());
    EmptyString(dpxsequence.currentheader_edited.reserved4,56);
    strcpy(dpxsequence.currentheader_edited.reserved4,arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.reserved4;
    QString O =   dpxsequence.currentheader.reserved4;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::reserved4);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::reserved4);
    }
}


void MainWindow::on_ith_reserved_tf_textEdited(const QString &arg1)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    //  dpxsequence.currentheader_edited.reserved1(arg1.toStdString().c_str());
    EmptyString(dpxsequence.currentheader_edited.reserved5,76);
    strcpy(dpxsequence.currentheader_edited.reserved5,arg1.toStdString().c_str());
    QString E =   dpxsequence.currentheader_edited.reserved5;
    QString O =   dpxsequence.currentheader.reserved5;

    if (E!=O)
    {
        setQLineEditTextColor(Qt::red,edit);
        dpxsequence.changelist.insert(headerfield::reserved5);
    }
    else
    {
        setQLineEditTextColor(Qt::black,edit);
        dpxsequence.changelist.remove(headerfield::reserved5);
    }
}

void MainWindow::on_setkeycode_btn_clicked()
{



    int hfeet=0;
    int hframes =0;


    QString qformat;

    KeycodeDialog * k = new KeycodeDialog();
    k->exec();
    if (k->KeycodeAccpeted)
    {

        dpxsequence.keycodeeditstring=  k->Keycode;

        if (dpxsequence.keycodeeditstring[0] == '0')
        {
            dpxsequence.keycodeperfspercount=64;
            if(k->perfperframe==0)
                dpxsequence.keycodeperfsperframe=4;
            else if(k->perfperframe==1)
                dpxsequence.keycodeperfsperframe=3;
            else if(k->perfperframe==2)
                dpxsequence.keycodeperfsperframe=2;
            else
                dpxsequence.keycodeperfsperframe=8;



            //     dpxsequence.keycodeframespercount=dpxsequence.keycodeperfspercount/ dpxsequence.keycodeperfsperframe;
        }
        else if (dpxsequence.keycodeeditstring[0] == '1')
        {
            dpxsequence.keycodeperfspercount=20;
            dpxsequence.keycodeframespercount=20;
            dpxsequence.keycodeperfsperframe=1;

        }
        else if (dpxsequence.keycodeeditstring[0] == '2')//65mm -- kodak changed to 120
        {
            dpxsequence.keycodeperfspercount=120;
            if(k->perfperframe==0)
                dpxsequence.keycodeperfsperframe=5;
            else if(k->perfperframe==1)
                dpxsequence.keycodeperfsperframe=15;

            //   dpxsequence.keycodeframespercount=dpxsequence.keycodeperfspercount/ dpxsequence.keycodeperfsperframe;


        }

        dpxsequence.keycodeuseframes=k->countframes;
        QString qkeycode = dpxsequence.keycodeeditstring;


        QString kcode = qkeycode.mid(0,10); // subString contains "is"
        QString kfeets = qkeycode.mid(12,4);
        QString kframes = qkeycode.mid(4,2);
        QString kc1 =  qkeycode.mid(0,4);
        QString kc2 =  qkeycode.mid(6,6);
        CurrentKeycode =qkeycode;
        hfeet = kfeets.toInt();
        hframes= kframes.toInt();


        if(  dpxsequence.keycodeuseframes)
            dpxsequence.keycodetotalcounts = (hfeet * dpxsequence.keycodeperfspercount ) + (hframes *dpxsequence.keycodeperfsperframe) ;
        else
            dpxsequence.keycodetotalcounts = (hfeet * dpxsequence.keycodeperfspercount ) + (hframes ) ;
        dpxsequence.keycodeeditfnum = dpxsequence.currentframeindex;
        dpxsequence.currentheader_edited.SetFileEdgeCode(CurrentKeycode.toStdString().c_str());

        setQLineEditTextColor(Qt::red,  ui->ifh_filmmanid_tf);
        setQLineEditTextColor(Qt::red,ui->ifh_filmtype_tf);
        setQLineEditTextColor(Qt::red,ui->ifh_offset_tf);
        setQLineEditTextColor(Qt::red, ui->ifh_prefix_tf);
        setQLineEditTextColor(Qt::red,ui->ifh_count_tf);
        ui->ifh_filmmanid_tf->setText( (dpxsequence.currentheader_edited.filmManufacturingIdCode));
        ui->ifh_filmtype_tf->setText((dpxsequence.currentheader_edited.filmType));
        ui->ifh_offset_tf->setText( (dpxsequence.currentheader_edited.perfsOffset));
        ui->ifh_prefix_tf->setText( (dpxsequence.currentheader_edited.prefix));
        ui->ifh_count_tf->setText( (dpxsequence.currentheader_edited.count));
        dpxsequence.changelist.insert(headerfield::keycode);

    }
    keycodewrite=false;
}
void MainWindow::progressbarupdate(int p ,int t)
{

    ui->progressBar->setValue(p);


}

void MainWindow::on_Setfposifh_btn_clicked()
{


}
