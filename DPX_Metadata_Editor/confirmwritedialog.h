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
#ifndef CONFIRMWRITEDIALOG_H
#define CONFIRMWRITEDIALOG_H

#include <QDialog>

namespace Ui {
class confirmwriteDialog;
}

class confirmwriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit confirmwriteDialog
    (QWidget *parent = nullptr);
   void setdata(QString f);
    ~confirmwriteDialog();

private:
    Ui::confirmwriteDialog *ui;
};

#endif // COONFIRMWRITEDIALOG_H
