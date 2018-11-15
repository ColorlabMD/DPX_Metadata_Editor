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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DPX.h"
#include <QLineEdit>
#include "dpxviewer.h"
#include "dpxsequence.h"
#include "tcsetdialog.h"
static void EmptyString(char *str, const int len)
{
    for (int i = 0; i < len; i++)
        str[i] = '\0';
}
namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    DPXSequence  dpxsequence;
    ~MainWindow();
    void update_Read_DPX(int position);
    void update_UI_Fields();
    void showAbout();
    void reset_texfields();
    void  updateheader(int fnum);
    void setQLineEditBaseColor(Qt::GlobalColor, QLineEdit *iQLE);
    void setQLineEditTextColor(Qt::GlobalColor,QLineEdit *);
    void assignQLEValidators();
    void close_editmode ();
    void begin_editmode (int edit_type);
    void change_textfieldsforedit();
    int editmode;
    int markerinframe;
    int markeroutframe;
    bool keycodewrite;
    QString CurrentKeycode;
    int footformat;
    bool editsmade;
private slots:
    void on_openSequenc_BTN_clicked();

    void on_Frame_horizontalSlider_sliderMoved(int position);

    void on_Frame_spinBox_valueChanged(int arg1);



    void on_viewer_cb_clicked();

    void on_showviewer_btn_clicked();

    void on_writeheaders_btn_clicked();



    void on_SetInBtn_clicked();

    void on_SetOutBtn_clicked();



    void on_ValidateButton_clicked();

    void on_EnableEditCB_stateChanged(int arg1);

    void onTextEdit(const QString& text);

    void on_gfh_filename_tf_textEdited(const QString &arg1);

    void on_gfh_timedate_tf_textEdited(const QString &arg1);

    void on_gfh_creator_tf_textEdited(const QString &arg1);

    void on_gfh_project_tf_textEdited(const QString &arg1);

    void on_gfh_copyright_tf_textEdited(const QString &arg1);

    void on_gfh_encryptkey_tf_textEdited(const QString &arg1);

    void on_gfh_magic_tf_textEdited(const QString &arg1);

    void on_gfh_imageoffset_tf_textEdited(const QString &arg1);

    void on_gfh_version_tf_textEdited(const QString &arg1);

    void on_gfh_filesize_tf_textEdited(const QString &arg1);

    void on_gfh_dittokey_tf_textEdited(const QString &arg1);

    void on_gfh_genericsize_tf_textEdited(const QString &arg1);

    void on_gfh_industrysize_tf_textEdited(const QString &arg1);

    void on_gfh_usersize_tf_textEdited(const QString &arg1);

    void on_gih_orientation_tf_textEdited(const QString &arg1);

    void on_gih_elementcount_tf_textEdited(const QString &arg1);

    void on_gih_pixelsperline_tf_textEdited(const QString &arg1);

    void on_gih_linesperelement_tf_textEdited(const QString &arg1);

    void on_gih_datasign_tf_textEdited(const QString &arg1);

    void on_gih_packing_tf_textEdited(const QString &arg1);

    void on_gih_lowdata_tf_textEdited(const QString &arg1);

    void on_gih_lowquantity_tf_textEdited(const QString &arg1);

    void on_gih_highdata_tf_textEdited(const QString &arg1);

    void on_gih_highquantity_tf_textEdited(const QString &arg1);

    void on_gih_encoding_tf_textEdited(const QString &arg1);

    void on_gih_dataoffset_tf_textEdited(const QString &arg1);

    void on_gih_eolpadding_tf_textEdited(const QString &arg1);

    void on_gih_eoipaddding_tf_textEdited(const QString &arg1);

    void on_gih_descriptor_tf_textEdited(const QString &arg1);

    void on_gih_transfer_tf_textEdited(const QString &arg1);

    void on_gih_colormetric_tf_textEdited(const QString &arg1);

    void on_gih_bitsize_tf_textEdited(const QString &arg1);

    void on_gih_description_tf_textEdited(const QString &arg1);

    void on_goh_xoffset_tf_textEdited(const QString &arg1);

    void on_goh_yoffset_tf_textEdited(const QString &arg1);

    void on_goh_xcenter_tf_textEdited(const QString &arg1);

    void on_goh_ycenter_tf_textEdited(const QString &arg1);

    void on_goh_xoriginalsize_tf_textEdited(const QString &arg1);

    void on_goh_yoriginalsize_tf_textEdited(const QString &arg1);

    void on_goh_filename_tf_textEdited(const QString &arg1);

    void on_goh_timedate_tf_textEdited(const QString &arg1);

    void on_goh_inputdevice_tf_textEdited(const QString &arg1);

    void on_goh_inputdevicesn_tf_textEdited(const QString &arg1);

    void on_goh_border0_tf_textEdited(const QString &arg1);

    void on_goh_border1_tf_textEdited(const QString &arg1);

    void on_goh_border2_tf_textEdited(const QString &arg1);

    void on_goh_border3_tf_textEdited(const QString &arg1);

    void on_goh_aspectratio0_tf_textEdited(const QString &arg1);

    void on_goh_aspectratio1_tf_textEdited(const QString &arg1);

    void on_ifh_filmmanid_tf_textEdited(const QString &arg1);

    void on_ifh_filmtype_tf_textEdited(const QString &arg1);

    void on_ifh_offset_tf_textEdited(const QString &arg1);

    void on_ifh_prefix_tf_textEdited(const QString &arg1);

    void on_ifh_count_tf_textEdited(const QString &arg1);

    void on_ifh_format_tf_textEdited(const QString &arg1);

    void on_ifh_framepostion_tf_textEdited(const QString &arg1);

    void on_ifh_sequencelength_tf_textEdited(const QString &arg1);

    void on_ifh_heldcount_tf_textEdited(const QString &arg1);

    void on_ifh_framerate_tf_textEdited(const QString &arg1);

    void on_ifh_shutterangle_tf_textEdited(const QString &arg1);

    void on_ifh_frameid_tf_textEdited(const QString &arg1);

    void on_ifh_slate_tf_textEdited(const QString &arg1);

    void on_ith_timecode_tf_textEdited(const QString &arg1);

    void on_ith_userbits_tf_textEdited(const QString &arg1);

    void on_ith_interlace_tf_textEdited(const QString &arg1);

    void on_ith_fieldnumber_tf_textEdited(const QString &arg1);

    void on_ith_videosignal_tf_textEdited(const QString &arg1);

    void on_ith_padding_tf_textEdited(const QString &arg1);

    void on_ith_hsamplerate_tf_textEdited(const QString &arg1);

    void on_ith_vsamplerate_tf_textEdited(const QString &arg1);

    void on_ith_framerate_tf_textEdited(const QString &arg1);

    void on_ith_timeoffset_tf_textEdited(const QString &arg1);

    void on_ith_gamma_tf_textEdited(const QString &arg1);

    void on_ith_blacklevel_tf_textEdited(const QString &arg1);

    void on_ith_blackgain_tf_textEdited(const QString &arg1);

    void on_ith_breakpoint_tf_textEdited(const QString &arg1);

    void on_ith_whitelevel_tf_textEdited(const QString &arg1);

    void on_ith_integrationtime_tf_textEdited(const QString &arg1);

    void on_ud_userid_tf_textEdited(const QString &arg1);

    void on_EnableEditAllCB_stateChanged(int arg1);

    void on_SetTC_btn_clicked();

    void on_goh_xscannedsize_tf_textEdited(const QString &arg1);

    void on_goh_yscannedsize_tf_textEdited(const QString &arg1);

    void on_EnableEditAdvanced_CB_stateChanged(int arg1);


    void on_advedit_rb_clicked(bool checked);

    void on_alledit_rb_clicked(bool checked);

    void on_editbasic_rb_clicked(bool checked);

    void on_gfh_autofilename_cb_stateChanged(int arg1);

    void on_viewhex_cb_stateChanged(int arg1);

    void on_UD_textedit_tf_textChanged();

    void on_SetUB_btn_clicked();

    void on_gfh_reserved_tf_textEdited(const QString &arg1);

    void on_gih_reserved_tf_textEdited(const QString &arg1);

    void on_goh_reserved_tf_textEdited(const QString &arg1);

    void on_ifh_reserved_tf_textEdited(const QString &arg1);

    void progressbarupdate(int p,int t);

    void on_ith_reserved_tf_textEdited(const QString &arg1);

    void on_setkeycode_btn_clicked();



    void on_Setfposifh_btn_clicked();

private:
    Ui::MainWindow *ui;
    DPXviewer * dpx_window = NULL;
    QList<QLineEdit*> header_textfields;
    QList<QLineEdit*> header_textfields_normaledit;
    QList<QLineEdit*> header_textfields_advedit;
    QList<QLineEdit*> header_textfields_alledit;
};

#endif // MAINWINDOW_H
