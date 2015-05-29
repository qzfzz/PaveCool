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

#ifndef TDLG_H
#define TDLG_H

#include <QDialog>

namespace Ui {
class tdlg;
}

class tdlg : public QDialog
{
    Q_OBJECT

public:
    explicit tdlg(QWidget *parent = 0);
    ~tdlg();

    bool buser;
    int Tdef[2];
    int Tusr[2];
    int Tmin[2];
    int Tmax[2];
    QString tunit;

    void updateView();

private slots:
    void on_tsSpin_valueChanged(int arg1);
    void on_tsSpin_editingFinished();
    void on_tfSpin_valueChanged(int arg1);
    void on_tfSpin_editingFinished();
    void on_defButton_clicked();

private:
    Ui::tdlg *ui;

    bool bedit[2]; // indicates spinbox value has changed
    void checkval(int *CheckValue, int Min, int Max);
};

#endif // TDLG_H
