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

#include "pavecool.h"
#include <QApplication>
#include <QTranslator>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    QString qmPath = "locale";
    QString loc = QLocale::system().name();
    loc = loc.left(2); // use only the language code
//    translator.load("pavecool_de", qmPath);
    translator.load("pavecool_" + loc, qmPath);
    a.installTranslator(&translator);

    a.setApplicationName(APP);
    a.setOrganizationName("MnDOT");
    a.setOrganizationDomain("dot.state.mn.us");
    a.setApplicationVersion("3000");
    PaveCool w;
    w.show();

    return a.exec();
}
