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

#include "helpdlg.h"
#include "ui_helpdlg.h"
#include "pavecool.h"

HelpDlg::HelpDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDlg)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->helpBrowser->setOpenExternalLinks(true);
    QString hloc = DEFHELP;
    QUrl src;
    hloc.replace("en", QLocale::system().name().left(2)); // use only the language code

    QFile hfile(hloc);
    if(hfile.exists()) src = QUrl(hloc);
    else
    {
        QMessageBox msg;
        msg.setText(tr("Help is not available in this language. Help will be displayed in English."));
        msg.exec();
        src = QUrl(DEFHELP);
    }
    ui->helpBrowser->setSource(src);
}

HelpDlg::~HelpDlg()
{
    delete ui;
}

void HelpDlg::on_backButton_clicked()
{
    ui->helpBrowser->backward();
}

void HelpDlg::on_forwardButton_clicked()
{
    ui->helpBrowser->forward();
}
