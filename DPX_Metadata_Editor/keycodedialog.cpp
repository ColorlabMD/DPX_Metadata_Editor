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

#include "keycodedialog.h"
#include "ui_keycodedialog.h"
#include <qvalidator.h>
KeycodeDialog::KeycodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeycodeDialog)
{
    ui->setupUi(this);
    auto di = new QIntValidator();
    ui->kkmfg->setValidator(di);
    ui->kkft->setValidator(di);
    ui->kkpf->setValidator(di);
    ui->kkct->setValidator(di);
    ui->kkpo->setValidator(di);

}

KeycodeDialog::~KeycodeDialog()
{
    delete ui;
}

void KeycodeDialog::on_buttonBox_accepted()
{


    Keycode = ui->kkmfg->text().rightJustified(2,'0') + ui->kkft->text().rightJustified(2,'0')+ ui->kkpo->text().rightJustified(2,'0') + ui->kkpf->text().rightJustified(6,'0') + ui->kkct->text().rightJustified(4,'0');
    perfperframe = ui->ppf_cb->currentIndex();
    if( Keycode.length() !=16)
    { Keycode = "error";
        countframes=ui->frames_rb->isChecked();
        KeycodeAccpeted=false;
    }

    else
        KeycodeAccpeted =true;

}

void KeycodeDialog::on_kkmfg_textEdited(const QString &arg1)
{
    QString MFG;
    QString FG;
    QStringList mm35= {"4 perf","3 perf","2 perf","8 perf"};
    QStringList mm65= {"5 perf","15 perf"};
    if (arg1.length()==2)
    {
        if (arg1[1]=='0')
            MFG = ("Orwo ");
        else if (arg1[1]=='1')
            MFG = ("Agfa ");
        else if (arg1[1]=='2')
            MFG = ("Kodak ");
        else if (arg1[1]=='3')
            MFG = ("Fuji ");
        else
            MFG = ("Unknown ");

        if (arg1[0]=='0')
        {
            FG = ("35mm ");
            ui->ppf_cb->clear();
            ui->ppf_cb->addItems(mm35);
        }
        else if (arg1[0]=='1')
        {
            FG = ("16mm ");
            ui->ppf_cb->clear();
            ui->ppf_cb->addItem("1 perf");
        }
        else if (arg1[0]=='2')
        {
            FG = ("65mm ");
            ui->ppf_cb->clear();
            ui->ppf_cb->addItems(mm65);
        }
        else
            FG ="Unknown";
        ui->mfglabel->setText(MFG+FG);


    }



}
