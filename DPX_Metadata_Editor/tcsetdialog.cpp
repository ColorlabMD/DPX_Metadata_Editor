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


#include "tcsetdialog.h"
#include "ui_tcsetdialog.h"
#include <QRegularExpressionValidator>
TCSetDialog::TCSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TCSetDialog)
{
    ui->setupUi(this);

    QRegExp tcre ("(\\d{1,2}\\:\\d{1,2}\\:\\d{1,2}\\:\\d{1,2})");
    QValidator *validator = new QRegExpValidator(tcre, this);


    ui->TC_le->setValidator(validator);

}

TCSetDialog::~TCSetDialog()
{
    delete ui;
}
void TCSetDialog::ui_dataset( QString arg1)
{
    UE_Timecode = ui->TC_le->text();
}
void TCSetDialog::on_TC_le_textChanged(const QString &arg1)
{
    UE_Timecode = ui->TC_le->text();
}
