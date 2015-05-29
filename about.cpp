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

#include "about.h"
#include "ui_about.h"
#include "pavecool.h"

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QString tver = QApplication::applicationVersion();
    tver.insert(1,".");


    int month = MONTH;

    //translatables
    QString devby = tr("%1 is free and open source software developed in 1998 at the University of Minnesota by:").arg(APP);
    QString upd = tr("Updated at MnDOT by %1").arg("Bruce Tanquist");
    QString dl = tr("Download %1 and the Final Report at:").arg(APP);
    QString info = tr("For more information, contact:");
    ui->aboutLabel->setTextFormat(Qt::RichText);
    ui->aboutLabel->setOpenExternalLinks(true);
    ui->aboutLabel->setWordWrap(true);
    ui->aboutLabel->setText("<b>PaveCool " + tr("Version") + " " + tver + " - "
                            + QDate::shortMonthName(month) + " " + YEAR
                            + "</b><br>" + COPYRIGHT + " 2001-" + YEAR
                            + " Minnesota Department of Transportation<br><br>"
                            + devby
                            + "<table width=\"100%\">"
                            "<tr><td>David Newcomb</td><td>Vaughan Voller<td></tr>"
                            "<tr><td>Bruce Tanquist</td><td>Jim Luoma<td></tr>"
                            "<tr><td>David Timm</td><td>Rachel De Sombre<td></tr></table>"
                            "<br><br>" + upd + "<br><br>" + dl
                            + "<br><a href=\"http://www.dot.state.mn.us/app/pavecool/\">"
                            "PaveCool Web Site</a><br><br>" + info
                            + "<br><a href=\"mailto:materialslab@state.mn.us?"
                            "subject=PaveCool\">MaterialsLab@state.mn.us</a>"
                            "<br><a href=\"mailto:bruce.tanquist@state.mn.us?"
                            "subject=PaveCool\">Bruce.Tanquist@state.mn.us</a>");
}

about::~about()
{
    delete ui;
}
