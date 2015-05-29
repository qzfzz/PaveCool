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

#ifndef PAVECOOL_H
#define PAVECOOL_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDataStream>
#include <QDate>
#include <QDesktopServices>
#include <QFileDevice>
#include <QFileInfo>
#include <QFileDialog>
#include <QImageReader>
#include <QList>
#include <QLocale>
#include <QPdfWriter>
#include <QCalendarWidget>
#include <QSaveFile>
#include <QSettings>
#include <QTextBrowser>
#include <QTime>
#include <QUrl>
#include <QVariant>

const QString   APP         = "PaveCool";
const QString   APPEXT      = "pcl";
const QString   YEAR        = "2015";
const QString   COPYRIGHT   = "©";
const QString   DEFHELP     = "./help/pchelp_en.htm";
const int       MONTH       = 5;

// air temperature °C
const int       TEMP        = 0; // index
const double    TA_MIN      = -40.0;
const double    TA_DEF      = 10.0;
const double    TA_MAX      = 48.9;
// existing surface temperature °C
const double    TS_MIN      = -40.0;
const double    TS_DEF      = 10.0;
const double    TS_MAX      = 73.9;
// delivery temperature °C
const double    TM_MIN      = 66.0;
const double    TM_DEF      = 135.0;
const double    TM_MAX      = 177.0;
// start temperatures for PG58+ & PG52-
const double    TSTART58    = 120.0;
const double    TSTART52    = 110.0;
//stop temperature °C
const double    TSTOP_MIN   = 60.0;
const double    TSTOP_DEF   = 79.4;
const double    TSTOP_MAX   = 171.0;
// lift thickness mm
const int       HT          = 1; // index
const double    H_MIN       = 12.7;
const double    H_DEF       = 76.2;
const double    H_MAX       = 254.0;
// wind speed km/h
const int       VEL         = 2; // index
const double    V_MIN       = 0.0;
const double    V_DEF       = 8.0;
const double    V_MAX       = 193.0;
// latitude °
const int       LAT         = 3; // index
const double    LAT_DEC     = 2;
const double    LAT_MIN     = -90.0; // Latitude °S
const double    LAT_DEF     = 45.0;
const double    LAT_MAX     = 90.0; // °N
// default combo selections
const int       SKY_DEF     = 0; // Clear
const int       GRAD_DEF    = 0; // Fine
const int       PGHI_DEF    = 4; // PG58-34
const int       PGLO_DEF    = 4;
const int       MAT_DEF     = 2; // Granular base
const int       W_DEF       = 0; // Dry
const int       STATE_DEF   = 0; // Unfrozen

const int       MAXLAYER    = 5;

/********************************************************
*														*
*  Remember:  If you change these values, the help file *
*  should be updated accordingly!                       *
*														*
*********************************************************/

const double    DG_K            = 2.000; //AC Material Property Constants...
const double    SMA_K           = 1.500; //DG = Dense Graded ---> SMA = Stone Matrix Asphalt
const double    DG_RHO          = 2000.0;
const double    SMA_RHO         = 2010.0;
const double    DG_CP           = 1000.0;
const double    SMA_CP          = 1010.0;

const double    AC_BASE_K       = 1.510; //Base material property constants
const double    PCC_K           = 0.920;
const double    CDF_K           = 2.000; //Coarse, Dry, Frozen base material
const double    CDT_K           = 1.160;
const double    CWF_K           = 3.550;
const double    CWT_K           = 1.220; //Note:  These material properties came from version 17
const double    FDF_K           = 1.400;
const double    FDT_K           = 1.350;
const double    FWF_K           = 2.380;
const double    FWT_K           = 1.870;

const double    AC_BASE_RHO     = 2000.0;
const double    PCC_RHO         = 2000.0;
const double    CDF_RHO         = 2000.0;
const double    CDT_RHO         = 2000.0;
const double    CWF_RHO         = 2000.0;
const double    CWT_RHO         = 2000.0;
const double    FDF_RHO         = 1800.0;
const double    FDT_RHO         = 1800.0;
const double    FWF_RHO         = 1800.0;
const double    FWT_RHO         = 1800.0;

const double    AC_BASE_CP      = 1025.0;
const double    PCC_CP          = 1090.0;
const double    CDF_CP          = 860.0;
const double    CDT_CP          = 960.0;
const double    CWF_CP          = 960.0;
const double    CWT_CP          = 1170.0;
const double    FDF_CP          = 960.0;
const double    FDT_CP          = 1170.0;
const double    FWF_CP          = 1170.0;
const double    FWT_CP          = 1590.0;

const int       SIZE            = 75; //max # of node points allowed
//const int       SIZE            = 520; //max # of node points allowed

const int       PG_CUTOFF       = 4; //PG CutOff grade for beginning compaction
const double    STOPTEMP        = 79.4;

namespace Ui {
class PaveCool;
}

class PaveCool : public QMainWindow
{
    Q_OBJECT

public:
    explicit PaveCool(QWidget *parent = 0);
    ~PaveCool();


private:
    bool bmodified;     // document was edited (warn on close or new)
    bool bdefault;      // defaults have been restored
    bool bopen;         // file was opened
    bool bspin;         // spinner was used
    bool bsi;           // substituted for bList[2] to make code more clear
    bool noreset;       // don't reset output when units or time format change
    bool bunit;         // indicates if units changed
    bool bedit[8];      // Indicates data was edited
    int nVersion;       // Converted from QApplication::GetApplicationVersion()
    double val[4];      // holds user-entered doubles
    double tstart, tstop;   // start & stop temperatures
    double sizemult;    // used to set size of dialog windows
    QString tval[5];    // holds user-entered strings
    QString tName;      // Main Window name
    QString tPath;      // current path
    QString tFile;      // current filename
    QString tTest;      // used for debug
    QString tDisclaimer;
    QString tGNU1, tGNU2, tGNU3;
    QMenu *fileMenu;
    QMenu *recentFilesMenu;
    enum { MaxRecentFiles = 6};
    QAction *pAction[MaxRecentFiles];
    QRect plotrect;
    QCalendarWidget *calendar;

    // Serialized variables
    QList<bool> bList;    // serialized bool
    QList<int> iList;     // serialized int
    QList<double> dList;  // serialized double
    QList<QString> sList; // serialized QString
    QList<QVariant> vList; // serialized Other types

    // Calculate variables
    QList<double> THMA;
    QDate qToday;
    QTime qNow;
    bool bcalc;
    double MaxWindSpeed;
    double MinWindSpeed;
    double MaxAirTemp, MinAirTemp;
    double MaxThickness, MinThickness;
    double MaxBaseTemp, MinBaseTemp;
    double MaxDelivTemp, MinDelivTemp;
    double MaxLatitude, MinLatitude;
    double AsphK;
    double AsphRho;
    double AsphCp;
    double BasK;
    double BasCp;
    double BasRho;
    double timeStart;
    double timeStop;
    QString tStop;

    void resizeEvent(QResizeEvent *e);
    void closeEvent(QCloseEvent *event);
    void pcSettings(const QString &file, const int &nmode);
    void checkval(double *CheckValue, double Min, double Max, const int &ntype);
    void SetDefaults(bool bkeepname = false);
    void updateView();
    void resetOutput();
    void openRecent();
    bool Open();
    bool askSave();
    bool Save(const QString &fileName);
    bool SaveAs(const QString &fileName);
    bool Write(const QString &fileName);
    bool Read(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString newWinTitle(const QString &tTitle);
    void paintEvent(QPaintEvent *);
    void calculate();

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave_PDF_triggered();
    void on_action1_triggered();
    void on_action2_triggered();
    void on_action3_triggered();
    void on_action4_triggered();
    void on_action5_triggered();
    void on_action6_triggered();
    void on_actionExit_triggered();
    void on_actionStart_Stop_Temperatures_triggered();
    void on_actionEnglish_triggered();
    void on_actionSI_triggered();
    void on_action12_Hour_triggered();
    void on_action24_Hour_triggered();
    void on_actionUS_triggered();
    void on_actionISO_triggered();
    void on_actionReset_Defaults_triggered();
    void on_actionHelp_Topics_triggered();
    void on_actionAbout_PaveCool_triggered();
    void on_projectEdit_textEdited(const QString &arg1);
    void on_dateEdit_dateChanged(const QDate &date);
    void on_dateEdit_editingFinished();
    void on_timeEdit_textEdited(const QString &arg1);
    void on_timeEdit_editingFinished();
    void on_timeButton_clicked();
    void on_taEdit_textEdited(const QString &arg1);
    void on_taEdit_editingFinished();
    void on_swEdit_textEdited(const QString &arg1);
    void on_swEdit_editingFinished();
    void on_skyCombo_currentIndexChanged(int index);
    void on_latEdit_textEdited(const QString &arg1);
    void on_latEdit_editingFinished();
    void on_gradCombo_currentIndexChanged(int index);
    void on_hiCombo_currentIndexChanged(int index);
    void on_loCombo_currentIndexChanged(int index);
    void on_hDSpin_valueChanged(double arg1);
    void on_hDSpin_editingFinished();
    void on_tmSpin_valueChanged(int arg1);
    void on_tmSpin_editingFinished();
    void on_matCombo_currentIndexChanged(int index);
    void on_wCombo_currentIndexChanged(int index);
    void on_stateCombo_currentIndexChanged(int index);
    void on_tsEdit_textEdited(const QString &arg1);
    void on_tsEdit_editingFinished();
    void on_calcButton_clicked();
    void on_discButton_clicked();
    void on_exportButton_clicked();

private:
    Ui::PaveCool *ui;

};

#endif // PAVECOOL_H
