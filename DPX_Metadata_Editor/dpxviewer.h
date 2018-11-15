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
#ifndef DPXVIEWER_H
#define DPXVIEWER_H

#include <QDialog>
#include "QPixmap"
#include <QGraphicsScene>
namespace Ui {
class DPXviewer;
}

class DPXviewer : public QDialog
{
    Q_OBJECT

public:
    explicit DPXviewer(QWidget *parent = 0);
    ~DPXviewer();
    int dpx_image_width;
    int dpx_image_height;
   void ShowFrame(QImage* );



private:
    Ui::DPXviewer *ui;

    QGraphicsScene * scene;
};

#endif // DPXVIEWER_H
