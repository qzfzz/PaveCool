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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QDebug>
#include <QMessageBox>
#include <QResizeEvent>
#include <QtMath>
#include <QPainter>
#include <QScreen>
#include <QTime>

// ALL THE GLOBAL DECLARATIONS

const int MAXDEC    = 6;
// Decimal places to use for different units
const int DEC_T     = 0;
const int DEC_IN    = 2;
const int DEC_MM    = 0;
const int DEC_V     = 0;
const int DEC_LAT   = 2; // Latitude
// Unit conversion factors
const double DEG_RAD    = 0.01745329;
const double IN_MM      = 25.4;
const double FT_M       = 0.3048;
const double MI_KM      = 1.609344;
const double LB_N       = 4.448222;
const double LB_KN      = 0.004448222;
const double TON_TONNE  = 0.9071847;
const double PSI_KPA    = 6.89475729;
const double PSI_MPA    = 0.00689475729;
const double PCF_KNM3   = 0.1570875;	// unit weight
const double PCF_KGM3   = 16.01846;     // bulk density
const double C_K		= 273.15;       // add
const double K_R		= 1.8;          // multiply
const double F_R		= 459.67;       // add
const double DAYS_WEEKS = 0.142466;
const double WEEKS      = 52.142857;
// increment for spin buttons
const double INC_IN     =  0.5;
const double INC_MM     = 10.0;
const double    PI          = 3.14159265358979323846;
const double    E3          = 1000.;
const double    E6          = 1000000.;
const double    EN3         = 0.001;
const double    EN6         = 0.000001;
// CFont orientation (check QFont methods)
const int       RIGHT       = 0;
const int       UP          = 900;
const int       LEFT        = 1800;
const int       DOWN        = 2700;
// text
const QString   SPACE       = " ";
const QString   PER         = ".  ";
const QString   COLON       = ":";
const QString   CR          = "\n";
const QString   ARIAL       = "Arial";
const QString   MS          = "MS Sans Serif";
const QString   TIMES       = "Times New Roman";
const QString   SYMBOL      = "Symbol";
const QString   WINGDING    = "Wingdings";
//registry
const int READREG           = 0;
const int SETREG            = 1;
const int NOFILE            = 2;
const int SETBOOL           = 3;
const int READBOOL          = 4;
const QString DEFAULT_DIR_KEY   = "defaultDir";
const QString RECENT_FILES_KEY  = "recentFileList";
const QString UNITS_SI_KEY      = "unitsSI";
const QString TIME_24H_KEY      = "time24Hour";
const QString DATE_ISO_KEY      = "dateISO";
// rectangle corners
const int       TL          = 0;
const int       TR          = 1;
const int       BR          = 2;
const int       BL          = 3;
// colors
const QColor    BLACK       = QColor(0,0,0);
const QColor    RED		    = QColor(255,0,0);
const QColor    DKRED	    = QColor(128,0,0);
const QColor    GREEN	    = QColor(0,255,0);
const QColor    DKGREEN	    = QColor(0,128,0);
const QColor    BLUE	    = QColor(0,0,255);
const QColor    DKBLUE	    = QColor(0,0,128);
const QColor    YELLOW	    = QColor(255,255,0);
const QColor    GOLD	    = QColor(128,128,0);
const QColor    PINK	    = QColor(255,0,255);
const QColor    PURPLE	    = QColor(128,0,218);
const QColor    TURQ	    = QColor(0,255,255);
const QColor    TEAL	    = QColor(0,128,128);
const QColor    DKTEAL	    = QColor(0,64,64);
const QColor    LTGRAY	    = QColor(224,224,224);
const QColor    GRAY	    = QColor(192,192,192);
const QColor    DKGRAY	    = QColor(128,128,128);
const QColor    WHITE	    = QColor(255,255,255);
const QColor    TRANSP	    = QColor(0,0,0,0);

extern bool setSpin(double oldval, double *newval, double incr);
extern int decl(bool si);
extern int nint (double x);
extern int pdfRect(QRectF *rect, double wpage, double top, double ht, int ncells, int n);
extern long nlong(double x);
extern double incl(bool si);
extern double convt(bool insi, double tin, bool outsi = true);
extern double convl(bool insi, double lin, bool outsi = true);
extern double convll(bool insi, double lin, bool outsi = true);
extern double convlll(bool insi, double lin, bool outsi = true);
extern double convf(bool insi, double fin, bool outsi = true);
extern double convp(bool insi, double pin, bool outsi = true);
extern double convd(bool insi, double din, bool outsi = true);
extern double rnd2dec(double value, int dec, bool insigfig = false);
extern QString txtt(bool si);
extern QString txtl(bool si);
extern QString txtll(bool si);
extern QString txtlll(bool si);
extern QString txtf(bool si);
extern QString txtff(bool si);
extern QString txtp(bool si);
extern QString txte(bool si);
extern QString txtu(bool si);
extern QString txtv(bool si);
extern QString trimright(QString txt, QString ttrim);
extern QString txtdbl(double value, int dec, bool sigfig = false, bool trimzero = false, bool comma = false);
extern QString txthrmin(int hour, int min, bool b24);
extern QTime TimeText(QString *time, bool b24);
// Note the important use of extern!

#endif // GLOBALS_H
