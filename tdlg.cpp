/****************************************************************************
**
** Copyright (C) 2015 Minnesota Department of Transportation
** Office of Materials & Road Research
** 1400 Gervais Avenue
** Saint Paul, Minnesota 55109-2044
** USA
** http://www.dot.state.mn.us/app/pavecool
**
**
** $QT_BEGIN_LICENSE:GPL$
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** If you did not receive a copy of the GNU General Public License
** see http://www.gnu.org/licenses
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tdlg.h"
#include "ui_tdlg.h"
#include "globals.h"

tdlg::tdlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tdlg)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    bedit[0] = bedit[1] = false;
}

tdlg::~tdlg()
{
    delete ui;
}

void tdlg::updateView()
{
    ui->tsuLabel->setText(tunit);
    ui->tfuLabel->setText(tunit);
    double tstart, tstop;
    if(buser)
    {
        tstart = Tusr[0];
        tstop = Tusr[1];
    }
    else
    {
        tstart = Tdef[0];
        tstop = Tdef[1];
    }
    ui->tsSpin->setValue((int) rnd2dec(tstart,0));
    ui->tfSpin->setValue((int) rnd2dec(tstop,0));
}

void tdlg::checkval(int *CheckValue, int Min, int Max)
{
    QString twarn;
    QString tmsg = "";

    twarn = tr("Temperature");

    if(*CheckValue < Min)
    {
        tmsg = tr("%1 must be at least %2 %3").arg(twarn).arg(QString::number(Min)).arg(tunit);
        *CheckValue = Min;
    }
    else if(*CheckValue > Max)
    {
        tmsg = tr("%1 must not exceed %2 %3").arg(twarn).arg(QString::number(Max)).arg(tunit);
        *CheckValue = Max;
    }
    if(tmsg != "")
    {
        QMessageBox msgBox;
        msgBox.setText(tmsg);
        msgBox.exec();
    }
}

void tdlg::on_tsSpin_valueChanged(int arg1)
{
    bedit[0] = true;
    Tusr[0] = arg1;
}

void tdlg::on_tsSpin_editingFinished()
{
    if(bedit[0])
    {
        bedit[0] = false;
        buser = true;
        checkval(&Tusr[0], Tmin[0], Tmax[0]);
        ui->tsSpin->setValue(nint(Tusr[0]));
    }
}

void tdlg::on_tfSpin_valueChanged(int arg1)
{
    bedit[1] = true;
    Tusr[1] = arg1;
}

void tdlg::on_tfSpin_editingFinished()
{
    if(bedit[1])
    {
        bedit[1] = false;
        buser = true;
        checkval(&Tusr[1], Tmin[1], Tmax[1]);
        ui->tfSpin->setValue(nint(Tusr[1]));
    }
}

void tdlg::on_defButton_clicked()
{
    buser = false;
    Tusr[0] = Tdef[0];
    Tusr[1] = Tdef[1];
    updateView();
}
