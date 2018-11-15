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
#ifndef KEYCODEDIALOG_H
#define KEYCODEDIALOG_H

#include <QDialog>

namespace Ui {
class KeycodeDialog;
}

class KeycodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeycodeDialog(QWidget *parent = 0);
    ~KeycodeDialog();
    QString Keycode;
  bool KeycodeAccpeted;
  QString FilmMfg;
  int perfperframe;
  int perfspercount;
  bool countframes;
private slots:
    void on_buttonBox_accepted();


    void on_kkmfg_textEdited(const QString &arg1);

private:
    Ui::KeycodeDialog *ui;
};

#endif // KEYCODEDIALOG_H
