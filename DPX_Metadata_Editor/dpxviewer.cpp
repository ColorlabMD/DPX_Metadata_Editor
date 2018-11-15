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


#include "dpxviewer.h"
#include "ui_dpxviewer.h"
#include "QPixmap"
DPXviewer::DPXviewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DPXviewer)
{
    ui->setupUi(this);
     scene = new QGraphicsScene();
     ui->graphicsView->setScene(scene);
     ui->graphicsView->fitInView( scene->sceneRect(), Qt::KeepAspectRatio );
 }
void DPXviewer::ShowFrame(QImage* dpximage)
{

   QPixmap  pixmap = QPixmap::fromImage(
        *dpximage
    );


    scene->clear();
    ui->graphicsView->fitInView(QRect(0,0,dpx_image_width,dpx_image_height  ), Qt::KeepAspectRatio );
     scene->addPixmap(pixmap);




}

DPXviewer::~DPXviewer()
{
    delete ui;
    delete scene;
}
