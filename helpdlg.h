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

#ifndef HELPDLG_H
#define HELPDLG_H

#include <QDialog>
#include <QMessageBox>
#include <QPainter>
#include <QUrl>

namespace Ui {
class HelpDlg;
}

class HelpDlg : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDlg(QWidget *parent = 0);
    ~HelpDlg();

private slots:
    void on_backButton_clicked();
    void on_forwardButton_clicked();

private:
    Ui::HelpDlg *ui;
};

#endif // HELPDLG_H
