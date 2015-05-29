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

#include "globals.h"

// ALL THE GLOBAL DEFINITIONS
bool setSpin(double oldval, double *newval, double incr)
{// resets the spinner if the start point is not a multiple of the increment
    if(qFabs(*newval - oldval) == incr) // indicates spinner was used instead of edit box
    {
        if(remainder(*newval, incr) != 0)
        {
            *newval = *newval > oldval ? qFloor(*newval/incr) * incr : qCeil(*newval/incr) * incr;
        }
        return true;
    }
    else return false;
}

int pdfRect(QRectF *rect, double wpage, double top, double ht, int ncells, int n)
{   // returns the top of the next line's rect
    // n is the horizontal cell index (zero-based)
    // top gets updated for next line
    double lt, wd;
    wd = wpage/ncells;
    lt = (double) n*wd;
    QRectF rectTmp = QRectF(lt, top, wd, ht);
    *rect = rectTmp;
    return rectTmp.bottom();
//    return rectTmp.y() + rectTmp.height();
}

int decl(bool si)
{// decimal places for length (mm or in)
    return si ? DEC_MM : DEC_IN;
}

int nint(double x) // provided by koahnig at qt-project.org
{// avoids binary rounding error in (int) conversion
    return x < 0 ? (int)ceil(x) : (int)floor(x);
}

long nlong(double x)
{// avoids binary rounding error in (long) conversion
    return x < 0 ? (long)ceil(x) : (long)floor(x);
}

double incl(bool si)
{// spin button increments for length (mm or in)
    return si ? INC_MM : INC_IN;
}
// The following facilitate conversions between stored and displayed data (SI or English units)
// insi and outsi refer to the input and output units (true = SI)

double convt(bool insi, double tin, bool outsi)
{// temperature °C or °F
    if((insi && outsi) || (!insi && !outsi)) return tin;
    else if(insi) return  tin * K_R + 32.;
    else return (tin - 32.) / K_R;
}

double convl(bool insi, double lin, bool outsi)
{// length mm or in
    if((insi && outsi) || (!insi && !outsi)) return lin;
    else if(insi) return lin / IN_MM;
    else return lin * IN_MM;
}

double convll(bool insi, double lin, bool outsi)
{// input length m or ft, output m
    if((insi && outsi) || (!insi && !outsi)) return lin;
    else if(insi) return lin / FT_M;
    else return lin * FT_M;
}

double convlll(bool insi, double lin, bool outsi)
{// length km or mi
    if((insi && outsi) || (!insi && !outsi)) return lin;
    else if(insi) return lin / MI_KM;
    else return lin * MI_KM;
}

double convf(bool insi, double fin, bool outsi)
{// force N (kN) or lbs (kips)
    if((insi && outsi) || (!insi && !outsi)) return fin;
    else if(insi) return fin / LB_N;
    else return fin * LB_N;
}

double convp(bool insi, double pin, bool outsi)
{// pressure kPa (MPa) or psi (ksi)
    if((insi && outsi) || (!insi && !outsi)) return pin;
    else if(insi) return pin / PSI_KPA;
    else return pin * PSI_KPA;
}

double convd(bool insi, double din, bool outsi)
{// density kg/m3 or pcf
    if((insi && outsi) || (!insi && !outsi)) return din;
    else if(insi) return din / PCF_KGM3;
    else return din * PCF_KGM3;
}

double rnd2dec(double value, int dec, bool sigfig)
{// RoundToDecimal written by Corey Spagnoli  1/14/98 adapted by Bruce Tanquist
    long   temp_value;
    double ret_value;
    double raise_to;
    double tolerance;
    double final_diff;
    bool bneg = value < 0;
    if(bneg) value = -value;
    // Significant figures option added by Bruce Tanquist 5/11/01
    if(sigfig)
    {
        dec = value == 0 ? 0 : dec - (1 + nint(log10(value)));
        if(dec > MAXDEC) dec = MAXDEC;
    }
    raise_to = pow(10.0, (double) dec);
    tolerance = 1.0 / raise_to;
    temp_value = (long)round(value * raise_to);
    ret_value = (double)temp_value / raise_to;
    final_diff = value - ret_value;
    if (final_diff < -tolerance || final_diff > tolerance) ret_value = value;
    value = ret_value;
    if(bneg) value = -value;
    return value;
}

QString txtt(bool si)
{
    return si ? "°C" : "°F";
}

QString txtl(bool si)
{
    return si ? QObject::QObject::tr("mm") : QObject::QObject::tr("in.");
}

QString txtll(bool si)
{
    return si ? QObject::tr("m") : QObject::tr("ft.");
}

QString txtlll(bool si)
{
    return si ? QObject::tr("km") : QObject::tr("mi.");
}

QString txtf(bool si)
{
    return si ? QObject::tr("N") : QObject::tr("lb.");
}

QString txtff(bool si)
{
    return si ? QObject::tr("kN") : QObject::tr("kips");
}

QString txtp(bool si)
{
    return si ? QObject::tr("kPa") : QObject::tr("psi");
}

QString txte(bool si)
{
    return si ? QObject::tr("MPa") : QObject::tr("ksi");
}

QString txtu(bool si)
{
    return si ? QObject::tr("µm") : QObject::tr("mils");
}

QString tontxt(bool si)
{
    return si ? QObject::tr("tonnes") : QObject::tr("tons");
}

QString txtv(bool si)
{
    return si ? "km/h" : "mph";
}

QString trimright(QString txt, QString ttrim)
{// trims all of the ttrim characters from the right side of txt
    while(txt.right(1) == ttrim) txt.resize(txt.size() - 1);
    return txt;
}

QString txtdbl(double value, int dec, bool sigfig, bool trimzero, bool comma)
{// convert a double to text with options for decimal place, significant digits, remove extra zeroes and add commas
    int  i, decimal;
    QString tNum;
    value = rnd2dec(value, dec, sigfig);
    // run conversion with precision 6 to make sure the decimal point is found
    tNum = QString::number(value, 'f', 6);
    decimal = tNum.indexOf(".");
    tNum = sigfig ? QString::number(value, 'e', dec) : QString::number(value, 'f', dec);
    tNum.remove(".");
    if(decimal > 0)
    {
        int lg = tNum.length();
        if(decimal > lg) for(i = 0; i < decimal - lg; i++) tNum.insert(lg+i, "0");
        tNum.insert(decimal, ".");
        if(trimzero) tNum = trimright(tNum, "0");
        tNum = trimright(tNum, ".");
    }
    else if(decimal == 0)
    {
        tNum.insert(0, ".");
        tNum.insert(0, "0");
        if(trimzero) tNum = trimright(tNum, "0");
    }
    else // decimal < 0
    {
        for(i = decimal; i < 0; i++) tNum.insert(0, "0");
        tNum.insert(0, ".");
        tNum.insert(0, "0");
        if(trimzero) tNum = trimright(tNum, "0");
    }
    if(comma)
    {
        int length = tNum.indexOf(".");
        if(length == -1) length = tNum.size();
        for(i = 1; i < 4; i++) if(i*3 < length) tNum.insert(length-i*3, ",");
    }
    return tNum;
}

QString txthrmin(int hour, int min, bool b24)
{// convert hour and minute strings to one time string with option for 12 or 24 hour format
    if(hour < 0 || hour > 23) hour = 0;
    if(min < 0 || min > 59) min = 0;
    QString tm;
    QString th;
    QString tmin = QString::number(min);
    QString tc = ":";
    QString tz = "0";
    if(b24) tm = "";
    else if(hour < 12) tm = " AM";
    else tm = " PM";
    if(min < 10) tmin = tc + tz + tmin;
    else tmin = tc + tmin;
    if(b24)
    {
        th = QString::number(hour);
        if(hour < 10) th = tz + th;
    }
    else
    {
        if(hour == 0) hour = 12;
        else if(hour > 12) hour -= 12;
        th = QString::number(hour);
    }
    return th + tmin + tm;
}

QTime TimeText(QString *time, bool b24)
{// interpret a string with numbers, am, pm, a, p and other characters to produce
 // a QTime variable and formatted time string with option for 12 or 24 hour format
    QString ttime = *time;
    QString th;		// hours
    QString tm = " PM";
    QString tmin;
    QString tc = ":";
    QString tz = "0";
    ttime.toLower();
    int nAdd = 12;	// add 12 hours to PM times
    int na = ttime.indexOf("a");
    ttime.remove("a");
    int np = ttime.indexOf("p");
    ttime.remove("p");
    int nz;	// indicates leading zero (military time)
    if(na > 0)
    {
        nAdd = 0;
        tm = " AM";
    }
    int nt = ttime.indexOf("m");
    ttime.remove("m");
    int nmin = 0;	// minutes
    // check to see if there are more or less than 2 characters after the colon
    nt = ttime.indexOf(":");
    if(nt > -1)
    {
        int nl = ttime.length();
        if(nl == nt+3) ttime.insert(nt+1, "0");	// insert 0 if only 1 char in minutes
        else if(nl > nt+3) ttime.remove(nt+3, nl-nt-3);	// remove extra characters after minutes
    }
    ttime.remove(" ");
    ttime.remove(".");
    ttime.remove(",");
    ttime.remove(";");
    ttime.remove(":");
    QString ttmp = ttime.at(0);
    nz = ttmp.toInt();	// zero indicates military time
    nt = ttime.toInt();
    if(nt >= 100)	// assume hours and minutes entered
    {
        tmin = ttime.right(2);
        nmin = tmin.toInt();
        nt /= 100;	// remove minutes
    }
    if(nt == 0 || nt == 24)
    {
        nt = 12;
        nAdd = -12;
        tm = " AM";
    }
    else if(nt > 24)
    {
        nt = 1;
    }
    else if(nt > 12)
    {
        nt -= 12;
        nAdd = 12;
    }
    else if(nt == 12)
    {
        if (na > 0) nAdd = -12;	// AM
        else nAdd = 0;
    }
    else if(na + np == 0	// no am or pm indicated
        && (nz == 0 || (nt < 12 && nt > 5)))// assume am if 6:00 to 11:59
    {
        nAdd = 0;
        tm = " AM";
    }
    if(nmin >= 60) nmin = 0;
    tmin = QString::number(nmin);
    if(nmin < 10) tmin = tc + tz + tmin;
    else tmin = tc + tmin;
    if(b24)
    {
        th = QString::number(nt+nAdd);
        tm = "";
        if(nt+nAdd < 10) th = tz + th;
    }
    else th = QString::number(nt);
    *time = th + tmin + tm;
    return QTime(nt + nAdd, nmin);
}
