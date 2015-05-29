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
#include "ui_pavecool.h"
#include "globals.h"
#include "helpdlg.h"
#include "tdlg.h"
#include "about.h"

PaveCool::PaveCool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PaveCool)
{
    ui->setupUi(this);

    calendar = ui->dateEdit->calendarWidget();
    calendar->setLocale(QLocale::system());

    // translatables
    tDisclaimer = tr("Some asphalt mixtures will require compaction times and temperatures different from those recommended by this program. Good judgement must be exercised in order to ensure a properly compacted surface. Special considerations should be made for polymer modified asphalt binders and warm-mix asphalt (WMA). In these cases, manufacturer guidelines should supersede recommendations made by this program. Consult the Help file for further details. In no event will the Minnesota Department of Transportation, the University of Minnesota or their suppliers be liable for damages or expenses arising out of the use of this program.");

    int i;

    // Initialize lists
    bList.reserve(4);         // number of items in the list
    bList.append(false);      // 0 default start & stop temperatures, true = user-defined
    bList.append(false);      // 1 Display English units, true = SI
    bList.append(false);      // 2 12 hour time format, true = 24 hour
    bList.append(false);      // 3 US date format, true = ISO
    bList.append(false);      // 4 Calculate after opening file
    iList.reserve(7);
    iList.append(0);          // 0 cloud cover
    iList.append(0);          // 1 fine/dense or SMA
    iList.append(0);          // 2 high PG temperature
    iList.append(0);          // 3 low PG temperature
    iList.append(0);          // 4 existing surface type
    iList.append(0);          // 5 wet or dry
    iList.append(0);          // 6 frozen or not
    dList.reserve(8);
    dList.append(0.0);        // 0 lift thickness (mm)
    dList.append(0.0);        // 1 latitude (°)
    dList.append(0.0);        // 2 air temperature (°C)
    dList.append(0.0);        // 3 mix delivery temperature
    dList.append(0.0);        // 4 existing surface temperature
    dList.append(0.0);        // 5 user start temperature
    dList.append(0.0);        // 6 user stop temperature
    dList.append(0.0);        // 7 wind velocity (km/h)
    sList.reserve(1);
    sList.append("");        // 0 project title
    vList.reserve(2);
    vList.append(QDate());    // 0 start date
    vList.append(QTime());    // 1 start time

    pAction[0] = ui->action1;
    pAction[1] = ui->action2;
    pAction[2] = ui->action3;
    pAction[3] = ui->action4;
    pAction[4] = ui->action5;
    pAction[5] = ui->action6;

    for(i = 0; i < MaxRecentFiles; ++i)
    {
        pAction[i]->setVisible(false);
    }

    // Initialize other variables
    bdefault = false;
    bopen = false;
    bspin = false;
    noreset = false;
    bunit = false;
    bcalc = false;
    bList[0] = false;    // default start & stop temperatures
// The following are set in windows registry
//    bList[1] = bsi = false;     // Display English units
//    bList[2] = false;           // 12 hour time format
//    bList[3] = false;           // US date format
    dList[5] = TSTART58;          // Start compacting
    dList[6] = TSTOP_DEF;         // Stop compacting

    sizemult = 1.;
    nVersion = QApplication::applicationVersion().toInt();
    tFile = "";

    // initialize variables for temporary user data
    for(i = 0; i < 8; ++i) bedit[i] = false;
    for(i = 0; i < 4; ++i) val[i] = 0.0;
    for(i = 0; i < 5; ++i) tval[i] = "";

    SetDefaults();
    pcSettings("", READBOOL);
    bmodified = false;
}

PaveCool::~PaveCool()
{
    delete ui;
}

void PaveCool::resizeEvent(QResizeEvent *e)
{
    int i = e->size().height();
    int j = e->size().width();
    double ht = (double) i / (double) PaveCool::minimumHeight();
    double w = (double) j / (double) PaveCool::minimumWidth();
    if(w < ht) sizemult = w;
    else sizemult = ht;
    ht *= 8;
    w *= 8;
    if(w < 8) w = 8;
    if(w < ht) i = nint(w);
    else i = nint(ht);
    QFont newFont = ui->projectLabel->font();
    if(i != newFont.pointSize())
    {
        newFont.setPointSize(i);
        QApplication::setFont(newFont);
    }
    QMainWindow::resizeEvent(e);
}

void PaveCool::closeEvent(QCloseEvent *event)
{
    if(bmodified)
    {
        if(askSave())
        {
            pcSettings("", SETBOOL);
            event->accept();
        }
        else event->ignore();
    }
    else
    {
        pcSettings("", SETBOOL);
        event->accept();
    }
}

void PaveCool::pcSettings(const QString &fileName, const int &nmode)
{
    int i;
    QSettings settings;
    QStringList files = settings.value(RECENT_FILES_KEY).toStringList();

    switch (nmode)
    {
        case READREG:
        {
            tPath = settings.value(DEFAULT_DIR_KEY).toString();
        }
        break;
        case SETREG:
        {
            settings.setValue(DEFAULT_DIR_KEY, QFileInfo(fileName).absolutePath());
            files.removeAll(fileName);
            files.prepend(fileName);
            while (files.size() > MaxRecentFiles)
                files.removeLast();
            settings.setValue(RECENT_FILES_KEY, files);
        }
        break;
        case NOFILE:
        {
            files.removeAll(fileName);
            settings.setValue(RECENT_FILES_KEY, files);
        }
        break;
        case SETBOOL:
        {
            settings.setValue(UNITS_SI_KEY, bList[1] ? "1" : "0");
            settings.setValue(TIME_24H_KEY, bList[2] ? "1" : "0");
            settings.setValue(DATE_ISO_KEY, bList[3] ? "1" : "0");
        }
        case READBOOL:
        {
            bList[1] = bsi = settings.value(UNITS_SI_KEY).toInt();
            bList[2] = settings.value(TIME_24H_KEY).toInt();
            bList[3] = settings.value(DATE_ISO_KEY).toInt();
            updateView();
        }
        break;
    }
    // populate recent files in ui
    int nRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for(i = 0; i < nRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        pAction[i]->setText(text);
        pAction[i]->setData(files[i]);
        pAction[i]->setVisible(true);
    }
    for (i = nRecentFiles; i < MaxRecentFiles; ++i)
        pAction[i]->setVisible(false);
}

void PaveCool::on_actionNew_triggered()
{
// Create a new window
    pcSettings("", SETBOOL); // make same settings as open document
    PaveCool *other = new PaveCool;
    other->resize(this->size());
    other->move(this->pos() + QPoint(40,40));
    other->show();
}

void PaveCool::on_actionOpen_triggered()
{
    pcSettings("", READREG);
// File opens in a new window unless the old window is unmodified
    if(!tName.isEmpty())
    {
        PaveCool *other = new PaveCool;
        other->pcSettings("", READREG);
        if(other->Open())
        {
            other->resize(this->size());
            other->move(this->pos() + QPoint(40,40));
            other->show();
        }
        else other->destroy();
    }
    else Open();
}

void PaveCool::on_actionSave_triggered()
{
    if(Save(tFile))
        PaveCool::setWindowTitle(newWinTitle(tName));
}

void PaveCool::on_actionSave_As_triggered()
{
    if(SaveAs(tFile))
        PaveCool::setWindowTitle(newWinTitle(tName));
}

void PaveCool::on_actionSave_PDF_triggered()
{   // font: 72 pts/in; pdf: 1200 pixels/in
    pcSettings("", READREG);
    on_calcButton_clicked();
    QString ttmp = tName;
    QString newName = ttmp.replace(".pcl", ".pdf");
    if(newName.isEmpty()) newName = sList[0].isEmpty() ? QApplication::applicationName() + "1" : sList[0];
    newName = QFileDialog::getSaveFileName(this, tr("Save") + " PDF " + tr("Report"),
                           tPath + "/" + newName, "PDF " + tr("Files") + " (*.pdf)");
    if(!newName.isEmpty())
    {
        QPdfWriter ppdf(newName);
        ppdf.setPageSize(QPagedPaintDevice::Letter);
        ppdf.setPageMargins(QMarginsF(72,54,54,54)); // L 1", R,T,B 0.75"
        ppdf.setTitle(newWinTitle("") + " " + tr("Report"));
        ppdf.newPage();

        QPainter painter(&ppdf);

        QFont arial10("Arial");
        arial10.setPointSize(10);
        QFont arial12("Arial");
        arial12.setPointSize(14);
        QFont arial12b("Arial");
        arial12b.setPointSize(14);
        arial12b.setWeight(QFont::Bold);
        QFont arial14("Arial");
        arial14.setPointSize(14);
        arial14.setWeight(QFont::Bold);
        QFont arial18("Arial");
        arial18.setPointSize(18);
        arial18.setWeight(QFont::Bold);

        QTextOption tleft, tcenter, tright, tfile;
        tleft.setAlignment(Qt::AlignLeft);
        tcenter.setAlignment(Qt::AlignCenter);
        tright.setAlignment(Qt::AlignRight);
        tfile.setAlignment(Qt::AlignLeft);
        tfile.setAlignment(Qt::AlignBottom);
        tfile.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

        QPen pen;
        pen.setBrush(Qt::black);
        pen.setWidth(8);
        painter.setPen(pen);
        QRectF txrect;
        double top, bot, wid, ht;
        top = 0;
        wid = 8100; // 6.75"

        ht = 450;
        painter.setFont(arial18);
        top = pdfRect(&txrect,wid,top,ht,1,0);
        painter.drawText(txrect,ppdf.title(),tcenter);

        ht = 525;
        ttmp = "";
        if(sList[0] != "") ttmp = tr("Project") + ": " + sList[0];
        painter.setFont(arial12);
        top = pdfRect(&txrect,wid,top,ht,1,0);
        painter.drawText(txrect,ttmp,tleft);

        ht = 400;
        painter.setFont(arial14);
        bot = pdfRect(&txrect,wid,top,ht,3,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Date") + " & " + tr("Time"),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,3,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Start Rolling") + "*",tcenter);
        top = pdfRect(&txrect,wid,top,ht,3,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Stop Rolling") + "*",tcenter);

        bot = pdfRect(&txrect,wid,top,ht,3,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->dateEdit->text()
                + "    " + ui->timeEdit->text(),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,3,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->startEdit->text() + " " + tr("minutes") + " ("
                + txtdbl(convt(true,tstart,bsi),DEC_T)
                + " " + txtt(bsi) + ")",tcenter);
        top = pdfRect(&txrect,wid,top,ht,3,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->stopEdit->text() + " " + tr("minutes") + " ("
                + txtdbl(convt(true,tstop,bsi),DEC_T)
                + " " + txtt(bsi) + ")",tcenter);

        top += 100;
        ht = 400;
        painter.setFont(arial12b);
        bot = pdfRect(&txrect,wid,top,ht,4,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Mix Type"),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Binder Grade"),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Thickness"),tcenter);
        top = pdfRect(&txrect,wid,top,ht,4,3);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Delivery Temp."),tcenter);

        ht = 600;
        painter.setFont(arial12);
        bot = pdfRect(&txrect,wid,top,ht,4,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->gradCombo->currentText(),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,"PG " + ui->hiCombo->currentText()
                + ui->loCombo->currentText(),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->hDSpin->text() + " " + txtl(bsi),tcenter);
        top = pdfRect(&txrect,wid,top,ht,4,3);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->tmSpin->text() + " " + txtt(bsi),tcenter);

        top += 100;
        ht = 400;
        painter.setFont(arial12b);
        bot = pdfRect(&txrect,wid,top,ht,4,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Air Temp."),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Wind Speed"),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Sky"),tcenter);
        top = pdfRect(&txrect,wid,top,ht,4,3);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Latitude"),tcenter);

        painter.setFont(arial12);
        bot = pdfRect(&txrect,wid,top,ht,4,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->taEdit->text() + " " + txtt(bsi),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->swEdit->text() + " " + txtv(bsi),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->skyCombo->currentText(),tcenter);
        top = pdfRect(&txrect,wid,top,ht,4,3);
        painter.drawRect(txrect);
        QString tdir = " °";
        if(dList[1] > 0) tdir = " ° " + tr("North");
        else if(dList[1] < 0) tdir = " ° " + tr("South");
        painter.drawText(txrect,QString::number(fabs(dList[1])) + tdir,tcenter);

        top += 100;
        painter.setFont(arial12b);
        bot = pdfRect(&txrect,wid,top,ht,4,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Existing Surface"),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Moisture"),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("State"),tcenter);
        top = pdfRect(&txrect,wid,top,ht,4,3);
        painter.drawRect(txrect);
        painter.drawText(txrect,tr("Surface Temp."),tcenter);

        painter.setFont(arial12);
        bot = pdfRect(&txrect,wid,top,ht,4,0);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->matCombo->currentText(),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,1);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->wCombo->currentText(),tcenter);
        bot = pdfRect(&txrect,wid,top,ht,4,2);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->stateCombo->currentText(),tcenter);
        top = pdfRect(&txrect,wid,top,ht,4,3);
        painter.drawRect(txrect);
        painter.drawText(txrect,ui->tsEdit->text() + " " + txtt(bsi),tcenter);

        // Get screen cap of graph
        int w = PaveCool::width();
        int h = PaveCool::height();
        PaveCool::resize(PaveCool::minimumWidth(),PaveCool::minimumHeight());

        this->repaint(); // refresh plotrect
        plotrect.setTopLeft(plotrect.topLeft()+QPoint(2,2));
        QPixmap px = this->grab(plotrect);
        top += 20;
        painter.drawPixmap(200,top,5400,5400,px);

        painter.setFont(arial10);
        ttmp = "";
        if(tName != "") ttmp = tr("%1 File:").arg(APP) + "\n" + tName;
        ht = 1400;
        top += 3600;
        top = pdfRect(&txrect,wid,top,ht,4,3);
        painter.drawText(txrect,ttmp,tfile);

        top += 210;
        top = pdfRect(&txrect,wid,top,1400,1,0);
        painter.drawText(txrect,"*" + tDisclaimer,tleft);

        ht = 200;
        top += 100;
        top = pdfRect(&txrect,wid,top,ht,2,1);
        painter.drawText(txrect,tr("Report Created") + " " + QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss"),tright);

        PaveCool::resize(w,h);

        painter.end();

        //Open file with default PDF application
        QDesktopServices::openUrl((QUrl("file:/" + newName)));
    }
}

void PaveCool::on_action1_triggered()
{
    openRecent();
}

void PaveCool::on_action2_triggered()
{
    openRecent();
}

void PaveCool::on_action3_triggered()
{
    openRecent();
}

void PaveCool::on_action4_triggered()
{
    openRecent();
}

void PaveCool::on_action5_triggered()
{
    openRecent();
}

void PaveCool::on_action6_triggered()
{
    openRecent();
}

void PaveCool::on_actionExit_triggered()
{
    qApp->exit();
}

void PaveCool::on_actionStart_Stop_Temperatures_triggered()
{
    bool btmp = bList[0];
    tdlg dlg;
    dlg.setWindowTitle(tr("Start") + " & " + tr("Stop") + " " + tr("Temperatures"));
    int w = nint(dlg.minimumWidth()*sizemult);
    int h = nint(dlg.minimumHeight()*sizemult);
    dlg.resize(w,h);
    QFont dlgfont = PaveCool::font();
    dlg.setFont(dlgfont);
    double Ttmp = iList[2] > PG_CUTOFF ? TSTART52 : TSTART58;
    dlg.buser = bList[0];
    dlg.Tdef[0] = nint(rnd2dec(convt(true, Ttmp, bsi), 0));
    dlg.Tdef[1] = nint(rnd2dec(convt(true, TSTOP_DEF, bsi), 0));
    dlg.Tusr[0] = nint(rnd2dec(convt(true, dList[5], bsi), 0));
    dlg.Tusr[1] = nint(rnd2dec(convt(true, dList[6], bsi), 0));
    dlg.Tmin[0] = nint(rnd2dec(convt(true, TM_MIN, bsi), 0));
    dlg.Tmin[1] = nint(rnd2dec(convt(true, TSTOP_MIN, bsi), 0));
    dlg.Tmax[0] = nint(rnd2dec(convt(true, TM_MAX, bsi), 0));
    dlg.Tmax[1] = nint(rnd2dec(convt(true, TSTOP_MAX, bsi), 0));
    dlg.tunit = txtt(bsi);
    dlg.updateView();

    int nret = dlg.exec();
    if(nret == 1)
    {
        bList[0] = dlg.buser;
        double tmp1 = convt(bsi, (double) dlg.Tusr[0]);
        double tmp2 = convt(bsi, (double) dlg.Tusr[1]);
        if(bList[0] != btmp || convt(bsi, dList[5]) != tmp1
                || convt(bsi, dList[6]) != tmp2)
        {
            dList[5] = tmp1;
            dList[6] = tmp2;
            //pcSettings("", SETBOOL);
            bmodified = true;
            resetOutput();
        }
    }
    updateView();
}

void PaveCool::on_actionEnglish_triggered()
{
    bList[1] = bsi = false;
    noreset = true;
    bunit = true;
    updateView();
}

void PaveCool::on_actionSI_triggered()
{
    bList[1] = bsi = true;
    noreset = true;
    bunit = true;
    updateView();
}

void PaveCool::on_action12_Hour_triggered()
{
    bList[2] = false;
    noreset = true;
    updateView();
}

void PaveCool::on_action24_Hour_triggered()
{
    bList[2] = true;
    noreset = true;
    updateView();
}

void PaveCool::on_actionUS_triggered()
{
    bList[3] = false;
    noreset = true;
    updateView();
}

void PaveCool::on_actionISO_triggered()
{
    bList[3] = true;
    noreset = true;
    updateView();
}


void PaveCool::on_actionReset_Defaults_triggered()
{
    if(bmodified)
    {
        if(askSave())
        {
            bList[0] = false;             // true = user-defined temperatures
            bList[1] = bsi = false;       // true = display SI units, false = English units
            bList[2] = false;             // true = 24 hour time format
            bList[3] = false;             // true = ISO date format
            SetDefaults(true);
            pcSettings("", SETBOOL);
        }
    }
    else
    {
        bList[0] = false;             // true = user-defined temperatures
        bList[1] = bsi = false;       // true = display SI units, false = English units
        bList[2] = false;             // true = 24 hour time format
        bList[3] = false;             // true = ISO date format
        SetDefaults(true);
        pcSettings("", SETBOOL);
        bmodified = true;
    }
    resetOutput();
}

void PaveCool::on_actionHelp_Topics_triggered()
{// create a modeless help window
    QFile hfile(DEFHELP);
    if(!hfile.exists())
    {
        QMessageBox msg;
        msg.setText(tr("Help file could not be located."));
        msg.exec();
    }
    else
    {
        HelpDlg *hlp = new HelpDlg(this);
        hlp->setWindowTitle(APP + " " + tr("Help"));
        hlp->resize(PaveCool::size());
        hlp->show();
    }
}

void PaveCool::on_actionAbout_PaveCool_triggered()
{
    about dlg;
    dlg.setWindowTitle(tr("About") + " " + APP);
    int w = nint(dlg.minimumWidth()*sizemult);
    int h = nint(dlg.minimumHeight()*sizemult);
    dlg.resize(w,h);
    QFont dlgfont = PaveCool::font();
    dlg.setFont(dlgfont);
    dlg.exec();
}

void PaveCool::on_projectEdit_textEdited(const QString &arg1)
{
    sList[0] = arg1;
    bmodified = true;
}

void PaveCool::on_dateEdit_dateChanged(const QDate &date)
{
    vList[0] = date;
    bedit[7] = true;
}

void PaveCool::on_dateEdit_editingFinished()
{
    if(bedit[7])
    {
        resetOutput();
        bedit[7] = false;
    }
}

void PaveCool::on_timeEdit_textEdited(const QString &arg1)
{
    tval[0] = arg1;
    bedit[0] = true;
}

void PaveCool::on_timeEdit_editingFinished()
{
    if(bedit[0])
    {
        vList[1] = TimeText(&tval[0], bList[2]);
        ui->timeEdit->setText(tval[0]);
        bedit[0] = false;
        resetOutput();
    }
}

void PaveCool::on_timeButton_clicked()
{
    if(vList[0] != QDate::currentDate() ||
            vList[1] != QTime::currentTime())
    {
        vList[0] = QDate::currentDate();
        vList[1] = QTime::currentTime();
        resetOutput();
    }
//    updateView();
}

void PaveCool::on_taEdit_textEdited(const QString &arg1)
{
    tval[1] = arg1;
    bedit[1] = true;
}

void PaveCool::on_taEdit_editingFinished()
{
    if(bedit[1])
    {
        bedit[1] = false;
        double dTa = rnd2dec(tval[1].toDouble(), DEC_T);
        if(convt(bsi, dTa) != dList[2])
        {
            checkval(&dTa, TA_MIN, TA_MAX, TEMP);
            dList[2] = convt(bsi, dTa); // stored as °C
            ui->taEdit->setText(txtdbl(dTa, DEC_T));
            resetOutput();
        }
    }
}

void PaveCool::on_swEdit_textEdited(const QString &arg1)
{
    tval[2] = arg1;
    bedit[2] = true;
}

void PaveCool::on_swEdit_editingFinished()
{
    if(bedit[2])
    {
        bedit[2] = false;
        double dSw = rnd2dec(tval[2].toDouble(), DEC_V);
        if(convlll(bsi, dSw) != dList[7])
        {
            checkval(&dSw, V_MIN, V_MAX, VEL);
            dList[7] = convlll(bsi, dSw); // stored as km/h
            ui->swEdit->setText(txtdbl(dSw, DEC_V));
            resetOutput();
        }
    }
}

void PaveCool::on_skyCombo_currentIndexChanged(int index)
{
    iList[0] = index;
    resetOutput();
}

void PaveCool::on_latEdit_textEdited(const QString &arg1)
{
    bedit[3] = true;
    tval[3] = arg1;
}

void PaveCool::on_latEdit_editingFinished()
{
    if(bedit[3])
    {
        bedit[3] = false;
        double dL = rnd2dec(tval[3].toDouble(), LAT_DEC);
        if(dL != dList[1])
        {
            checkval(&dL, LAT_MIN, LAT_MAX, LAT);
            dList[1] = dL;
            ui->latEdit->setText(txtdbl(dL, LAT_DEC, false, true, false));
            resetOutput();
        }
    }
}

void PaveCool::on_gradCombo_currentIndexChanged(int index)
{
    iList[1] = index;
    resetOutput();
}

void PaveCool::on_hiCombo_currentIndexChanged(int index)
{
    iList[2] = index;
    if(!bopen && bList[0]) on_actionStart_Stop_Temperatures_triggered();
    resetOutput();
}

void PaveCool::on_loCombo_currentIndexChanged(int index)
{
    iList[3] = index;
    if(!bopen && bList[0]) on_actionStart_Stop_Temperatures_triggered();
    resetOutput();
}

void PaveCool::on_hDSpin_valueChanged(double arg1)
{
    val[0] = arg1;
    bedit[4] = true;
    if(bunit)
    {
        resetOutput();
        bunit = false;
    }
    if(setSpin(rnd2dec(convl(true, dList[0], bsi), decl(bsi)), &val[0], incl(bsi)))
    {// check to see if spinner was used and reset the value if necessary
        bspin = true;
        ui->hDSpin->clearFocus();
    }
}

void PaveCool::on_hDSpin_editingFinished()
{
    if(bedit[4])
    {
        bedit[4] = false;
        val[0] = rnd2dec(val[0], decl(bsi));
        if(convl(bsi, val[0]) != dList[0])
        {
            checkval(&val[0], H_MIN, H_MAX, HT);
            dList[0] = convl(bsi, val[0]);
            ui->hDSpin->setValue(val[0]);
            if(bspin)
            {
                ui->hDSpin->setFocus();
                bspin = false;
            }
            resetOutput();
        }
    }
}

void PaveCool::on_tmSpin_valueChanged(int arg1)
{
    val[1] = (double) arg1;
    bedit[5] = true;
    if(bunit)
    {
        resetOutput();
        bunit = false;
    }
}

void PaveCool::on_tmSpin_editingFinished()
{
    if(bedit[5])
    {
        bedit[5] = false;
        if(convt(bsi, val[1]) != dList[3])
        {
            checkval(&val[1], TM_MIN, TM_MAX, TEMP);
            dList[3] = convt(bsi, val[1]); // stored as °C
            ui->tmSpin->setValue(nint(val[1]));
            resetOutput();
        }
    }
}

void PaveCool::on_matCombo_currentIndexChanged(int index)
{
    if(!bopen)
    {
        iList[4] = index;
        updateView();
        resetOutput();
    }
}

void PaveCool::on_wCombo_currentIndexChanged(int index)
{
    if(index >= 0)
    {
        iList[5] = index;
        resetOutput();
    }
}

void PaveCool::on_stateCombo_currentIndexChanged(int index)
{
    if(index >= 0)
    {
        iList[6] = index;
        resetOutput();
    }
}

void PaveCool::on_tsEdit_textEdited(const QString &arg1)
{
    tval[4] = arg1;
    bedit[6] = true;
}

void PaveCool::on_tsEdit_editingFinished()
{
    if(bedit[6])
    {
        bedit[6] = false;
        double dTs = rnd2dec(tval[4].toDouble(), DEC_T);
        if(convt(bsi, dTs) != dList[4])
        {
            checkval(&dTs, TS_MIN, TS_MAX, TEMP);
            dList[4] = convt(bsi, dTs); // stored as °C
            ui->tsEdit->setText(txtdbl(dTs, DEC_T));
            resetOutput();
        }
    }
}

void PaveCool::on_calcButton_clicked()
{
    calculate();
    ui->startEdit->setText(txtdbl(timeStart, 0));
    ui->stopEdit->setText(tStop);
}

void PaveCool::on_discButton_clicked()
{
    QMessageBox msg;
    msg.setFont(PaveCool::font());
    msg.setWindowTitle(APP + " " + tr("Disclaimer"));
    msg.setText(tDisclaimer);
    msg.exec();
}

void PaveCool::on_exportButton_clicked()
{
    int i;
    pcSettings("", READREG);
    QString ttmp = tName;
    QString tfilter = "";
    QString newName = ttmp.replace(".pcl", "");
    if(newName.isEmpty()) newName = sList[0].isEmpty() ? QApplication::applicationName() + "1" : sList[0];
    newName = QFileDialog::getSaveFileName(this, tr("Export %1 Data").arg(APP), tPath + "/" + newName,
                                           "Excel (*.xls);;" + tr("Comma-Delimited Text") + " (*.csv);;"
                                           + tr("Tab-Delimited Text") + " (*.txt)", &tfilter);
    tfilter = tfilter.left(1); // E, C or T

    if(!newName.isEmpty())
    {
        QSaveFile txtFile(newName);
        if(!txtFile.open(QIODevice::WriteOnly))
        {
            QMessageBox msg;
            msg.setText(tr("Export file could not be opened."));
            msg.exec();
            return;
        }
        QTextStream out(&txtFile);

        QChar d = tfilter == "E" ? '\t' : ' ';	// delimiter for input summary
        QChar ds = tfilter == "C" ? ',' : '\t'; // delimiter for temperature data

        out << PaveCool::windowTitle() << " " << tr("Simulation Output") << "\n";
        out << sList[0] << "\n";
        out << tr("Report Created") << " " << QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss") << "\n\n";
        out << tr("Paving Start Time") << "\n";
        out << ui->dateEdit->text() << d << ui->timeEdit->text() << "\n\n";
        out << tr("Mix Properties") << "\n";
        out << tr("Mix") << ":" << d << ui->gradCombo->currentText() << "\n";
        out << tr("Binder") << ":" << d << "PG " << ui->hiCombo->currentText();
        out << ui->loCombo->currentText() << "\n";
        out << tr("Lift") << ":" << d << txtdbl(convl(true, dList[0], bsi), decl(bsi)) << d << txtl(bsi) << "\n";
        out << tr("Deliv.") << ":" << d << txtdbl(convt(true, dList[3], bsi),DEC_T) << d << txtt(bsi) << "\n\n";
        out << tr("Environmental Conditions") << "\n";
        out << tr("The following data represent average conditions during compaction.") << "\n";
        out << tr("Air") << ":" << d << ui->taEdit->text() << d << txtt(bsi) << "\n";
        out << tr("Wind") << ":" << d << ui->swEdit->text() << d << txtv(bsi) << "\n";
        out << tr("Sky") << ":" << d << ui->skyCombo->currentText() << "\n";
        ttmp = "°";
        if(dList[1] > 0) ttmp = "° " + tr("North");
        else if(dList[1] < 0) ttmp = "° " + tr("South");
        out << tr("Latitude") << ":" << d << QString::number(fabs(dList[1])) << d << ttmp << "\n\n";
        out << tr("Existing Surface") << "\n";
        out << tr("Material") << ":" << d << ui->matCombo->currentText();
        ttmp = iList[4] > 1 ? " " + ui->wCombo->currentText() : "";
        out << ttmp;
        ttmp = iList[4] > 1 ? " " + ui->stateCombo->currentText() : "";
        out << ttmp << "\n";
        out << tr("Temp.") << ":" << d << ui->tsEdit->text() << d << txtt(bsi) << "\n\n";
        out << tr("Compaction Recommendations") << "\n";
        out << tr("Start") << ":" << d << ui->startEdit->text() << d << tr("min.") << d;
        out << txtdbl(convt(true, tstart, bsi), DEC_T) << d << txtt(bsi) << "\n";
        out << tr("Stop") << ":" << d << ui->stopEdit->text() << d << tr("min.") << d;
        out << txtdbl(convt(true, tstop, bsi), DEC_T) << d << txtt(bsi) << "\n\n";
        if(tfilter == "T") out << tDisclaimer << "\n\n";
        else
        {// parse disclaimer for spreadsheet view

            QString tdisc = tDisclaimer;
            int indx = -1;
            while(tdisc.length() > 0)
            {
                indx = tdisc.indexOf(' ', 68); // max line width = 68
                if(indx > 0)
                {
                    ttmp = tdisc.left(indx);
                    tdisc.remove(0, ttmp.length()+1);
                }
                else
                {
                    ttmp = tdisc;
                    tdisc = "";
                }
                out << ttmp << "\n";
            }
        }
        out << "\n" << tr("Cooling Curve") << "\n";
        out << tr("Time") << ds << tr("Temp.") << "\n";
        out << tr("min.") << ds << txtt(bsi) << "\n";
        //Print data for t=0
        out << "0" << ds << txtdbl(convt(true, THMA[0], bsi), 1) << "\n";
        //now do rest of data file
        for(i = 1; i < THMA.size(); i++) //was i = 0
        {
            if(fmod(i,3)==0)
            {
                out << QString::number(i/3) << ds;
                out << txtdbl(convt(true, THMA[i], bsi), 1) << "\n";
            }
        }
        bool bcom = txtFile.commit();
        if(!bcom)
        {
            QMessageBox msg;
            msg.setText("Export failed.");
            msg.exec();
        }
        else QDesktopServices::openUrl((QUrl("file:/" + newName)));
    }
}

void PaveCool::openRecent()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action)
    {
        if(!tName.isEmpty())
        {
            PaveCool *other = new PaveCool;
            if(other->Read(action->data().toString()))
            {
                other->resize(this->size());
                other->move(this->pos() + QPoint(40,40));
                other->show();
            }
            else
            {
                other->destroy();
                pcSettings(action->data().toString(), NOFILE);
            }
        }
        else
            if(!Read(action->data().toString()))
                pcSettings(action->data().toString(), NOFILE);
    }
}

void PaveCool::checkval(double *CheckValue, double Min, double Max, const int &ntype)
{
    bool btrim = false;
    int dec = 0;
    QString twarn = "";
    QString tunit = "";
    QString tmsg = "";

    switch (ntype)
    {
        case TEMP:
        {
            dec = DEC_T;
            twarn = "tr(Temperature)";
            tunit = txtt(bsi);
            Min = rnd2dec(convt(true, Min, bsi), DEC_T);
            Max = rnd2dec(convt(true, Max, bsi), DEC_T);
        }
        break;
        case HT:
        {
            dec = decl(bsi);
            twarn = "tr(Lift Thickness)";
            tunit = txtl(bsi);
            Min = rnd2dec(convl(true, Min, bsi), decl(bsi));
            Max = rnd2dec(convl(true, Max, bsi), decl(bsi));
        }
        break;
        case VEL:
        {
            twarn = "tr(Wind speed)";
            dec = DEC_V;
            tunit = txtv(bsi);
            Min = rnd2dec(convlll(true, Min, bsi), DEC_V);
            Max = rnd2dec(convlll(true, Max, bsi), DEC_V);
        }
        break;
        case LAT:
        {
            btrim = true;
            twarn = tr("Latitude");
            dec = DEC_LAT;
            tunit = "°N";
        }
        break;
    }
    if(*CheckValue < Min)
    {
        tmsg = tr("%1 must be at least %2 %3")
                .arg(twarn).arg(txtdbl(Min, dec, false, btrim, false)).arg(tunit);
        *CheckValue = Min;
    }
    else if(*CheckValue > Max)
    {
        tmsg = tr("%1 must not exceed %2 %3")
                .arg(twarn).arg(txtdbl(Max, dec, false, btrim, false)).arg(tunit);
        *CheckValue = Max;
    }
    if(tmsg != "")
    {
        QMessageBox msgBox;
        msgBox.setText(tmsg);
        msgBox.exec();
    }
}

void PaveCool::SetDefaults(bool bkeepname)
{
    // Reset all values
    if(!bkeepname)
    {
        tName = "";
        tFile = "";
    }
// Serialized variables
    bList[0] = false;             // true = user-defined temperatures
    iList[0] = SKY_DEF;           // combo indexes
    iList[1] = GRAD_DEF;          // fine/dense or SMA
    iList[2] = PGHI_DEF;          // High PG temperature
    iList[3] = PGLO_DEF;          // Low PG temperature
    iList[4] = MAT_DEF;           // existing surface
    iList[5] = W_DEF;             // wet or dry
    iList[6] = STATE_DEF;         // frozen or not
    dList[0] = H_DEF;             // lift thickness
    dList[1] = LAT_DEF;           // Latitude
    dList[2] = TA_DEF;            // air temperature in °C
    dList[3] = TM_DEF;            // mix delivery temperature
    dList[4] = TS_DEF;            // existing surface temperature
    dList[7] = V_DEF;             // wind velocity in km/h
    sList[0] = "";
    vList[0] = QDate::currentDate();
    vList[1] = QTime::currentTime();
    updateView();
    bmodified = false;
}

void PaveCool::updateView()
{
    PaveCool::setWindowTitle(newWinTitle(tName));
    ui->actionEnglish->setChecked(!bsi);
    ui->actionSI->setChecked(bsi);
    ui->action12_Hour->setChecked(!bList[2]);
    ui->action24_Hour->setChecked(bList[2]);
    ui->actionUS->setChecked(!bList[3]);
    ui->actionISO->setChecked(bList[3]);
    QString dformat = bList[3] ? "yyyy-MM-dd" : "M/d/yyyy";
    ui->dateEdit->setDisplayFormat(dformat);
    ui->skyCombo->setCurrentIndex(iList[0]);
    ui->gradCombo->setCurrentIndex(iList[1]);
    ui->hiCombo->setCurrentIndex(iList[2]);
    ui->loCombo->setCurrentIndex(iList[3]);
    ui->matCombo->setCurrentIndex(iList[4]);
    if(iList[4] < 2)
    {// disable material condition buttons for asphalt and concrete
        ui->wCombo->setCurrentIndex(-1);
        ui->stateCombo->setCurrentIndex(-1);
        ui->wCombo->setDisabled(true);
        ui->stateCombo->setDisabled(true);
    }
    else
    {
        ui->wCombo->setDisabled(false);
        ui->stateCombo->setDisabled(false);
        ui->wCombo->setCurrentIndex(iList[5]);
        ui->stateCombo->setCurrentIndex(iList[6]);
    }
    ui->hDSpin->setDecimals(decl(bsi));
    ui->hDSpin->setSingleStep(incl(bsi));
    ui->hDSpin->setValue(rnd2dec(convl(true,dList[0], bsi), decl(bsi)));
    ui->latEdit->setText(txtdbl(dList[1], LAT_DEC, false, true, false));
    ui->taEdit->setText(txtdbl(convt(true, dList[2], bsi), DEC_T));
    ui->tmSpin->setValue((int) rnd2dec(convt(true, dList[3], bsi),0));
    ui->tsEdit->setText(txtdbl(convt(true, dList[4], bsi), DEC_T));
    ui->swEdit->setText(txtdbl(convlll(true, dList[7], bsi), DEC_V));
    ui->projectEdit->setText(sList[0]);
    ui->dateEdit->setDate(vList[0].toDate());
    tval[0] = txthrmin(vList[1].toTime().hour(), vList[1].toTime().minute(), bList[2]);
    ui->timeEdit->setText(tval[0]);
    ui->huLabel->setText(txtl(bsi));
    ui->tauLabel->setText(txtt(bsi));
    ui->tmuLabel->setText(txtt(bsi));
    ui->tsuLabel->setText(txtt(bsi));
    ui->vuLabel->setText(txtv(bsi));
    if(bList[4]) on_calcButton_clicked();
    if(bcalc) ui->exportButton->setDisabled(false);
    else ui->exportButton->setDisabled(true);
    //tooltips
    if(bList[3]) ui->dateEdit->setAccessibleName("Start Date: year, month, day");
    else ui->dateEdit->setAccessibleName("Start Date: month, day, year");
    bopen = false;
    repaint();
}

void PaveCool::resetOutput()
{
    if(!bopen)
    {
        bmodified = true;
        if(!noreset)
        {
            ui->startEdit->setText("");
            ui->stopEdit->setText("");
            ui->exportButton->setDisabled(true);
            bcalc = false;
            bList[4] = false;
            repaint();
        }
        noreset = false;
    }
}

bool PaveCool::Open()
{
    QString tOpenFile = QFileDialog::getOpenFileName(this, tr("Open %1 File").arg(APP),
                                          tPath, tr("%1 Files (*.%2)").arg(APP).arg(APPEXT));
    if (tOpenFile.isEmpty()) return false;
    else return Read(tOpenFile);
}

bool PaveCool::askSave()
{
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, QApplication::applicationName(),
        tr("The document has been modified.") + "\n"
        + tr("Do you want to save your changes?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if(ret == QMessageBox::Yes) return Save(tFile);
    else if(ret == QMessageBox::Cancel) return false;
    else return true;
}

bool PaveCool::Save(const QString &fileName)
{
    if(!fileName.isEmpty()) return Write(fileName);
    else return SaveAs(fileName);
}

bool PaveCool::SaveAs(const QString &fileName)
{
    pcSettings("", READREG);
    QString newName = fileName;
    if(newName.isEmpty()) newName = tName;
    if(newName.isEmpty()) newName = sList[0].isEmpty() ? QApplication::applicationName() + "1" : sList[0];
    newName = QFileDialog::getSaveFileName(this, tr("Save %1 File").arg(APP),
                                 tPath + "/" + newName, tr("%1 Files (*.%2)").arg(APP).arg(APPEXT));
    if(!newName.isEmpty()) return Write(newName);
    else return false;
}

bool PaveCool::Write(const QString &fileName)
{
    QSaveFile mFile(fileName);
    mFile.setFileName(fileName);
    mFile.setDirectWriteFallback(true);

    if(!mFile.open(QFile::WriteOnly))
    {
        QMessageBox msgBox;
        msgBox.setText(tr("File could not be saved."));
        msgBox.exec();
        return false;
    }
    QDataStream out(&mFile);

    // Save Qt version so data from old files can be updated if necessary
    out.setVersion(QDataStream::Qt_5_2);
    // Create header with verification number and version number
    out << qint32(0xC01DE57);
//    out << qint32(0xFADE2B); // verification test
    out << nVersion;

    out << bList; // QList of bool
    out << iList; // QList of int
    out << dList; // QList of double
    out << sList; // QList of QString
    out << vList; // QList of QVariant

    bool bcom = mFile.commit();
    if(bcom)
    {
        QFileInfo mInfo(fileName);
        tName = mInfo.fileName();
        tFile = fileName;
        updateView();
        bmodified = false;
        pcSettings(fileName, SETREG);
    }
    return bcom;
}

bool PaveCool::Read(const QString &fileName)
{
    int i;
    bopen = false;
    QFile mFile(fileName);
    if(!mFile.open(QFile::ReadOnly))
    {
        QMessageBox msgBox;
        msgBox.setText(tr("File not found."));
        msgBox.exec();
        return false;
    }
    QDataStream in(&mFile);

    // Verify header information
    qint32 verify, version;
    in >> verify;
    if(verify == 0xC01DE57)
    {
        // Get Qt version
        int qver = in.version(); // for later use

        in >> version;
        if(version < nVersion)
        {// or if(version < 3000), if(version >= 3009)...
            // update values if necessary
        }

        in >> bList; // QList of bool
        in >> iList; // QList of int
        in >> dList; // QList of double
        in >> sList; // QList of QString
        in >> vList; // QList of QVariant

        bsi = bList[1];
        mFile.close();
        QFileInfo mInfo(fileName);
        tName = mInfo.fileName();
        tFile = fileName;
        bopen = true;
        updateView();
        bmodified = false;
        pcSettings(fileName, SETREG);
        return true;
    }
    else
    {
        mFile.close();
        QFile txtFile(fileName);
        txtFile.open(QIODevice::ReadOnly);
        QTextStream in(&txtFile);
        in >> verify;
        if(verify == 0xB01DE57)
        {// text file exported from PaveCool 2.5
            int nbool;
            QList<int> nTime;
            nTime.reserve(5);
            for(i = 0; i < 5; ++i)
            {
                nTime.append(0);
            }
            for(i = 0; i < bList.size(); ++i)
            {
                in >> nbool;
                bList[i] = nbool;
            }
            bsi = bList[1];
            for(i = 0; i < iList.size(); ++i)
            {
                in >> iList[i];
            }
            for(i = 0; i < dList.size(); ++i)
            {
                in >> dList[i];
            }
            for(i = 0; i < nTime.size(); ++i)
            {
                in >> nTime[i];
            }
            int nlen;
            in >> nlen;
            char chr;
            in >> chr;
            in >> chr; // skip newline
            sList[0] = in.read(nlen);
            vList[0] = QDate(nTime[0], nTime[1], nTime[2]);
            vList[1] = QTime(nTime[3], nTime[4]);
            tName = strippedName(fileName);
            QMessageBox msgBox;
            msgBox.setText(tr("File imported successfully."));
            msgBox.exec();
            txtFile.close();
            bopen = true;
            updateView();
            return true;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Incorrect file format.%1Code %2").arg("\n").arg(QString::number(verify)));
            msgBox.exec();
            txtFile.close();
            return false;
        }
    }
}

QString PaveCool::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

QString PaveCool::newWinTitle(const QString &tTitle)
{
    QString ttmp = QApplication::applicationName() + " "
            + txtdbl((double) nint((double) (nVersion*.01))*.1, 1);
    if(!tTitle.isEmpty())
    {
        ttmp = ttmp + " - " + tTitle;
    }
    return ttmp;
}

void PaveCool::paintEvent(QPaintEvent *)
{
    int i;
    QPainter painter(this);
    QPen pen;
    pen.setBrush(Qt::blue);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::white);

    // adjust rectangle to fit inside group box
    QPoint mb(ui->menuBar->rect().bottomRight());
    QPoint tb(ui->mainToolBar->rect().bottomRight());
    int gx = ui->groupBox6->x();
    int gy = ui->groupBox6->y();
    int gw = ui->groupBox6->width();
    int gh = ui->groupBox6->height();
    gx += gw/100;
    gw -= gw/50;
    gy = mb.y() + tb.y() + gh/25 + gy;
    gh -= gh/20;
    //QRect gr(gx, gy, gw, gh);
    plotrect = QRect(gx, gy, gw, gh);
    painter.drawRect(plotrect);
    int ox = gx+gw/6;
    int oy = gy+gh-gh/6;
    int endx = gx+gw-gw/12;
    int endy = gy+gh/7;
    int lx = endx-ox;
    int ly = oy-endy;
    int label = 10*gh/379;
    if(label > 10*gw/350) label = 10*gw/350;
    painter.setFont(QFont("Arial", label));
    // x&y axis
    pen.setBrush(Qt::black);
    painter.setPen(pen);
    qreal ticx = (qreal) gw / 172.;
    qreal ticy = (qreal) gh / 175.;
    painter.drawLine(ox, oy, endx, oy);
    painter.drawLine(ox, oy, ox, endy);
    painter.drawText(ox+(int)floor(50*ticx), oy+(int)floor(20*ticy), tr("Time, minutes"));
    // Tick marks
    int nticx = 6;
    int nticy;
    int nt = 20;
    int tgraph, tmin, tmax, ntemp;
    painter.drawText(ox-(int)floor(15*ticx), oy-ly-(int)floor(10*ticy), tr("Mix Temperature, %1").arg(txtt(bsi)));
    nticy = bsi ? 10 : 9;
    tgraph = convt(true,60,bsi);
    tmin = 60;
    tmax = 160;
    ntemp = bsi ? 10 : 20;
    double dx = double(lx)/double(nticx);
    double dy = double(ly)/double(nticy);
    int tick;
    QString sp = "  ";
    for(i = 0; i <= nticx; ++i)
    {
        tick = (int)rnd2dec(ox+i*dx,0);
        painter.drawLine(tick,oy,tick,oy+(int)floor(2*ticy));
        painter.drawText(tick-(int)floor(3*ticx),oy+(int)floor(10*ticy),QString::number(i*nt));
    }
    for(i = 0; i <= nticy; ++i)
    {
        tick = (int)rnd2dec(oy-i*dy,0);
        painter.drawLine(ox,tick,ox-(int)floor(2*ticx),tick);
        if(tgraph+i*ntemp >= 100) sp = "";
        painter.drawText(ox-(int)floor(15*ticx),tick+(int)floor(2*ticy),sp+QString::number(tgraph+i*ntemp));
    }
    //minor ticks
    for(i = 0; i <= nticx*2; ++i)
    {
        tick = (int)rnd2dec(ox+i*dx/2,0);
        painter.drawLine(tick,oy,tick,oy+(int)floor(2*ticy));
    }
    for(i = 0; i <= nticx*10; ++i)
    {
        tick = (int)rnd2dec(ox+i*dx/10,0);
        painter.drawLine(tick,oy,tick,oy+(int)floor(ticy));
    }
    if(!bsi)
    {
        for(i = 0; i <= nticy*2; ++i)
        {
            tick = (int)rnd2dec(oy-i*dy/2,0);
            painter.drawLine(ox,tick,ox-(int)floor(2*ticx),tick);
        }
    }
    for(i = 0; i <= nticy*10; ++i)
    {
        tick = (int)rnd2dec(oy-i*dy/10,0);
        painter.drawLine(ox,tick,ox-(int)floor(ticx),tick);
    }
    double time = 0.;
    double ttotal = 120.;	// BT changed to static scales
    int oldx, oldy, newx, newy;
    oldx = (int)ox;
    oldy = (int)(oy-(dList[3]-tmin)*ly/(tmax-tmin));

    if(bcalc)
    {
        pen.setWidth(2);
        for(i = 0; i < THMA.size() && time <= ttotal*60; ++i)
        {
            time = double(i) * 20.;
            newx = (int)floor(ox+lx*(time/60.0)/ttotal);
            if(THMA[i] <= tmax)
            {
                pen.setBrush(QColor(nint(floor(105.+150.*(THMA[i]-tmin)/(tmax-tmin))),
                                    105,nint(floor(255-250.*(THMA[i]-tmin)/(tmax-tmin)))));
                painter.setPen(pen);
                newy = (int)floor(oy-(THMA[i]-tmin)*ly/(tmax-tmin));
                painter.drawLine(oldx,oldy,newx,newy);
                oldy = newy;
            }
            else oldy = (int)(oy-ly);
            oldx = newx;
        }
        // legend
        int length = 20;
        oldx = ox+(int)floor(58*ticx);
        oldy = oy-ly+(int)floor(3*ticy);
        for(int i = 1; i <= length; i++)
        {
            pen.setBrush(QColor(nint(floor(255.-150.*(double)i/(double)length)),
                                105,nint(floor(5.+250.*(double)i/(double)length))));
            painter.setPen(pen);
            newx = ox+(int)floor(58*ticx+i*ticx);
            newy = oy-ly+(int)floor(3*ticy);
            painter.drawLine(oldx,oldy,newx,newy);
            oldx = newx;
            oldy = newy;
        }

        if(bList[0]) tstart = dList[5]; // User start temp
        else tstart = iList[2] > PG_CUTOFF ? TSTART52 : TSTART58;
        tstop = bList[0]? dList[6] : STOPTEMP; // m_StopTempSI;

        pen.setBrush(Qt::green);
        painter.setPen(pen);
        painter.drawLine(ox, (int)floor(oy-(tstart-tmin)*ly/(tmax-tmin)),
                         (int)floor(ox+lx*timeStart/ttotal),
                         (int)floor(oy-(tstart-tmin)*ly/(tmax-tmin)));
        painter.drawLine((int)floor(ox+lx*timeStart/ttotal),
                         (int)floor(oy-(tstart-tmin)*ly/(tmax-tmin)),
                         (int)floor(ox+lx*timeStart/ttotal), int(oy));
        // legend
        painter.drawLine(ox+(int)floor(58*ticx),oy-ly+(int)floor(11*ticy),
                       ox+(int)floor(58*ticx+length*ticx), oy-ly+(int)floor(11*ticy));

        pen.setBrush(Qt::red);
        painter.setPen(pen);
        painter.drawLine(ox, (int)floor(oy-(tstop-tmin)*ly/(tmax-tmin)),
                         (int)floor(ox+lx*timeStop/ttotal),
                         (int)floor(oy-(tstop-tmin)*ly/(tmax-tmin)));
        painter.drawLine((int)floor(ox+lx*timeStop/ttotal),
                         (int)floor(oy-(tstop-tmin)*ly/(tmax-tmin)),
                         (int)floor(ox+lx*timeStop/ttotal), int(oy));
        // legend
        painter.drawLine(ox+(int)floor(58*ticx),oy-ly+(int)floor(19*ticy),
                       ox+(int)floor(58*ticx+length*ticx), oy-ly+(int)floor(19*ticy));

        // legend text
        pen.setBrush(Qt::black);
        painter.setPen(pen);
        painter.drawText(ox+(int)floor(80*ticx),oy-ly+(int)floor(5*ticy), tr("Cooling Curve"));
        painter.drawText(ox+(int)floor(80*ticx),oy-ly+(int)floor(13*ticy), tr("Start Rolling"));
        painter.drawText(ox+(int)floor(80*ticx),oy-ly+int(21*ticy), tr("Stop Rolling"));
    }
}

void PaveCool::calculate()
{
    bcalc = false;
    THMA.clear();
    qToday = QDate::currentDate();
    qNow = QTime::currentTime();

    double PctClouds = (double) iList[0] * 0.25;
    double LiftThick = dList[0];
    double Tair = dList[2];
    double BasT = dList[4];
    double tlift = dList[3];
    double Wind = dList[7];
    if(bList[0]) tstart = dList[5]; // User start temp
    else tstart = iList[2] > PG_CUTOFF ? TSTART52 : TSTART58;
    tstop = bList[0]? dList[6] : STOPTEMP; // m_StopTempSI;

    switch (iList[1]) // Mix Type
    {
    case 0:
        {
            AsphK	= DG_K;
            AsphCp	= DG_CP;
            AsphRho	= DG_RHO;
        }
        break;
    case 1:
        {
            AsphK	= SMA_K;
            AsphCp	= SMA_CP;
            AsphRho	= SMA_RHO;
        }
        break;
    default:
        break;
    }//end switch

    //FROM HERE TO THE END OF THE FUNCTION, EVERYTHING IS IN SI!!!

    //These arrays have been added so that the pavement system can be 'reverse numbered'
    int nn[MAXLAYER];			// Absolute number of last node in zone[LAYER]
    int zones;					// number of zones used, must be .LE. LAYER - 1
    double ap[SIZE],aw[SIZE];	// Coefficients
    double ae[SIZE],b[SIZE];	// Coefficients
    double T[SIZE];				// Temperature
    double Delx[SIZE];			// Node cell length
    double cp[SIZE];			// thermal properties vectors
    double rho[SIZE];
    double K[SIZE];

    int i,k;					// Counters
    int j;						// Counter for convergence check
    int Flag;					// Flag for checking if deform has been called
    int Nodes[MAXLAYER];		// number of nodes in each material adds up to nn[LAYER]

    double Told[SIZE];			// Old time step Temperature
    double Tconv[SIZE];         // used in residuals calculation

    double Delt;				// Time step
    double Tinit[MAXLAYER];		// Starting Temperaure of MAXLAYERs
    double cpinit[MAXLAYER];	// Specific heat of the material in each MAXLAYER
    double Lift[MAXLAYER];		// Starting thickness of the MAXLAYERs
    double Kinit[MAXLAYER];		// Inital thermal conductivity of MAXLAYERs
    double rhoinit[MAXLAYER];   // Initial density of LAYERs

    double delta;				// % compaction of lift - 0 to 100
    double h;                   // Atmoshperic properties
    double Sigma;				// Stephan-Boltzman Constant
    double Tsky;				// Sky temperature - 250K clouds - 100K clear night
    double Qsun;				// Heating performed by sun, 0 to as much as 388 W/m^2)
    int TotalDays;				// Number of days into year, used for Net Solar Flux

    double ptime;				// cooling time till compressed
    double stime;				// time to stop simulation, must be GE ptime
    double ttime;				// total time
    double Tsum;				// Total normailzed temp error, used for convergence
//    double omega;				// relaxation
    double epsilon = 0.95;		// emmisivity of asphalt
    double Depth;  				// depth into earth
    double Tave = 0.0;			// used to calculate total heat lost from lift
    int RtnTime = -1;           // Used when in fuction mode...When cooled to tstop
    double error;				// convergence check variable

    // set and convert the above variables into what we were reading in before Windows

    ptime = 20.0E5;             // not worried about compaction so never compact
    stime = 2.1 * 60 * 60;      // run the simulation out 2 hours, that should be enough
    delta = 0.0;                // if it does happen to compact, pass zero compaction

    Lift[1] = LiftThick/1000.0; //puts thickness into meters
    Tinit[1]	= tlift;

    Kinit[1] = AsphK;
    cpinit[1] = AsphCp;
    rhoinit[1] = AsphRho;

    switch (iList[4])           // existing surface
    {
    case 0:
        {//AC
            BasK	= AC_BASE_K;
            BasCp	= AC_BASE_CP;
            BasRho	= AC_BASE_RHO;
        }
        break;
    case 1:
        {//PCC
            BasK	= PCC_K;
            BasCp	= PCC_CP;
            BasRho	= PCC_RHO;
        }
        break;
    case 2:
        {//CA
            switch(iList[5]*2+iList[6])
            {
            case 0:
                {//dry unfrozen
                    BasK	= CDT_K;
                    BasCp	= CDT_CP;
                    BasRho	= CDT_RHO;
                }
                break;
            case 1:
                {//dry frozen
                    BasK	= CDF_K;
                    BasCp	= CDF_CP;
                    BasRho	= CDF_RHO;
                }
                break;
            case 2:
                {//wet unfrozen
                    BasK	= CWT_K;
                    BasCp	= CWT_CP;
                    BasRho	= CWT_RHO;
                }
                break;
            case 3:
                {//wet frozen
                    BasK	= CWF_K;
                    BasCp	= CWF_CP;
                    BasRho	= CWF_RHO;
                }
                break;
            }
        }
        break;
    case 3:
        {//FA
            switch(iList[5]*2+iList[6])
            {
            case 0:
                {//dry unfrozen
                    BasK	= FDT_K;
                    BasCp	= FDT_CP;
                    BasRho	= FDT_RHO;
                }
                break;
            case 1:
                {//dry frozen
                    BasK	= FDF_K;
                    BasCp	= FDF_CP;
                    BasRho	= FDF_RHO;
                }
                break;
            case 2:
                {//wet unfrozen
                    BasK	= FWT_K;
                    BasCp	= FWT_CP;
                    BasRho	= FWT_RHO;
                }
                break;
            case 3:
                {//wet frozen
                    BasK	= FWF_K;
                    BasCp	= FWF_CP;
                    BasRho	= FWF_RHO;
                }
                break;
            }
        }
        break;
    default:
        break;
    }

    Kinit[2]	= BasK;
    cpinit[2]	= BasCp;
    rhoinit[2]	= BasRho;
    Tinit[2]	= BasT;

    TotalDays = vList[0].toDate().dayOfYear();

    // calcualte net solar flux for clear sky
    // this function takes latitude,day of year, and hour of day and
    // calculated CLEAR sky solar flux, it returns zero if it is night

    double phi, t;
    int d;
    double theta,sigma,iflux,nsf;
    double psi,cZ,hp,beta,decl;
    double Pi,Ellip,Solar;
    Pi = 3.14159265359;

    phi = qFabs(dList[1]) * Pi / 180.0; // latitude
    d = TotalDays;
    t = (double)vList[1].toTime().hour() + (double)vList[1].toTime().minute()/60.0;

    Ellip = 0.0167238;          // Ellipticity of Earth's orbit
    Solar = 1377.0;				// W/m^2

    theta = 2*Pi*(d-2)/365.242;
    if (d == 1) theta = 2*Pi*(d+363.0)/365.242;
    psi = 2*Pi*(d-1)/356.242;
    sigma = 279.9348+1.914827*sin(psi)-0.079525*cos(psi)
          +0.019938*sin(2*psi)-0.001620*cos(2*psi)+psi*180.0/Pi;
    decl=asin(sin(0.409172)*sin(sigma*Pi/180));

    if(dList[1] < 0) decl *= -1.; 	// added in 2014 for southern hemisphere locations

    beta=0.12357*sin(psi)-0.004289*cos(psi)+0.153809*sin(2*psi)+0.060783*cos(2*psi);
    hp=(t-beta-12.0)*Pi/12.0;
    cZ=sin(phi)*sin(decl)+cos(phi)*cos(decl)*cos(hp);
    iflux=Solar*cZ*pow(1+Ellip*cos(theta),2)/(1-pow(Ellip,2));
    nsf=(0.828*iflux-47.4);
    if (nsf<=0.0) nsf=0.0;
    Qsun = nsf;

// if nsf is zero its night, set Tsky accordingly work in Kelvins
// see example Krieth & Black pg 321
    if (Qsun == 0)
        Tsky = (Tair+48)*(double)(PctClouds)/100.0+225.0;
    else
        Tsky = Tair+273.0;

// finally set Qsun according to %Clouds & don't forget absorbance= 0.85
    Qsun = 0.85*Qsun*(1.0 - PctClouds); //PctClouds all ready in decimal

    // fix the rest of the variables that we did not read in....
    zones = 4;   // number of zones used, must be .LE. Layer - 1

    Nodes[1] = nint(100 * Lift[1]);   // seems to give good resolution for problem
    if (Nodes[1] <6) Nodes[1]=6;        // never run with less!
    if (Nodes[1] >50) Nodes[1]=50;      // defined SIZE controls maximum

    Nodes[2] = 6;
    Lift[2] = 0.06;

    Nodes[3] = 6;				// number of nodes in region 3
    Lift[3] = 0.1;              // Lower base material properties, meters
    Tinit[3] = Tinit[2];		// Deg C
    Kinit[3] = Kinit[2];		// W/m*K
    cpinit[3] = cpinit[2];		// J/kg*K
    rhoinit[3] = rhoinit[2];	// kg/m^3

    Nodes[4] = 8;				// number of nodes in region 4
    Lift[4] = 0.2;              // zone 4 material properties, meters
    Tinit[4] = Tinit[2];		// Deg C
    Kinit[4] = Kinit[2];		// W/m*K
    cpinit[4] = cpinit[2];		// J/kg*K
    rhoinit[4] = rhoinit[2];	// kg/m^3

    Delt = 20.0;                // time step seconds
    Wind /= 3.6;                // Convert km/h to m/s

    // h = 7.4+26.6*pow(Wind/6.7,0.75);  Alford, et al - W/m^2*K
    // h = 4.89187*pow(Wind,0.8);   Luoma's calcs
    // if(h < 1.0) h=1.0;
    // Convective Cooling Coeff (W/m^2*K)
    h = 3.152*(1.3 + 0.62*pow(Wind*2.23,0.75));  // barber? from jordan thomas

    Sigma = 5.67e-08;			// Radiative cooling coefficient
    ttime = 0.0;				// initialize time
//    omega = 0.25;				// relaxation  not used
    error = 1.0e-5;             // allowed error
    Flag = 0;                   // set defomed flag to zero (has not been called)
    j = 0;                      // initialize counter
    nn[0] = 0;                  // initialized for loop, nn[0] not used otherwise
    for (i = 1; i <= zones; ++i)// calculate nn[i] - highest node number is a zone
    {
       nn[i] = nn[i-1] + Nodes[i];
    }
    /* Initialize T[i],cp[i],rho[i],K[i], and Delx[i], remember, node 0 is the
       special surface node and does not have a volume associated with it
       we set Delx[0] and cp[0] to zero to perform this w/o dividing by zero later on*/

    k = 1;
    for (i = 0; i<=nn[zones]; ++i)
    {
        if(i > nn[k]) ++k;      // if we are in the next zone, use those properties
        T[i] = Tinit[k];
        cp[i] = cpinit[k];
        rho[i] = rhoinit[k];
        K[i] = Kinit[k];
        Delx[i]=Lift[k]/(Nodes[k]);	// Initial thickness of CVFE cells
    }

    Delx[0] = 0.0;
    cp[0] = 0.0;

    for (i = 0; i<=nn[zones]; ++i)
    {
        Told[i] = T[i];			// map initial values into Told
    }

    Depth = 0.0;
    for (i = 0; i<=nn[zones]; ++i)     // print out initial node values
    {
        Depth = Depth + Delx[i]/2.0;
        if(i==0) Depth = 0.0;
        Depth = Depth + Delx[i]/2.0;
        if(i==0) Depth = 0.0;
    }

// Start of time step

     THMA.append(dList[3]); // m_DelivTempSI

ten: ttime += Delt;
// Map Temperature values for convergence check
twenty: for (i = 0; i<=nn[zones]; ++i)
    {
        Tconv[i] = T[i];		// map initial values into Tconv
    }

    // Start of iteration
    // Calculate Coefficients note - don't do end points
    for (i=1; i<=(nn[zones]-1); ++i)
    {  				// see Patanka pg46
        aw[i]=1.0/((Delx[i-1]/(2.0*K[i-1]))+(Delx[i]/(2.0*K[i])));
        ae[i]=1.0/((Delx[i+1]/(2.0*K[i+1]))+(Delx[i]/(2.0*K[i])));
        ap[i]=ae[i]+aw[i]+(rho[i]*cp[i]*Delx[i])/Delt;
         b[i]=(rho[i]*cp[i]*Delx[i])*Told[i]/Delt;
    }

    // Modify endpoints now...
    // Convective/radiative boundary condition on node 0 & 1 - see Patanka pg 27,80
    ae[0]=+(4.0/3.0)*K[1]*2.0/Delx[1];
    aw[0]=(1.0/3.0)/(Delx[1]/(2.0*K[1])+Delx[2]/(2.0*K[2]));
    ap[0]=ae[0]+h;
     b[0]=h*Tair+Qsun-epsilon*Sigma*(pow((T[0]+273.0),4)-pow(Tsky,4));

    // Modify node 1 coefficients for higher accuracy
    ae[1]=+(4.0/3.0)*ae[1];
    aw[1]=+(4.0/3.0)*K[1]*2.0/Delx[1];
    ap[1]=ae[1]+aw[1]+(rho[1]*cp[1]*Delx[1])/Delt;

    // insulated boundary condition
    ae[nn[zones]]=0.0;
    aw[nn[zones]]=1.0/((Delx[i-1]/2.0*K[i-1])+(Delx[i]/2.0*K[i]));
    ap[nn[zones]]=aw[nn[zones]]+(rho[i]*cp[i]*Delx[i])/(Delt);
     b[nn[zones]]=Told[nn[zones]]*(rho[i]*cp[i]*Delx[i])/(Delt);

    // Call Tri-Diagonal Matrix Solver
    // Note: all parameters used by solver
    // have been declared as global.

    /****
    * TDMA SOLVER - Tri diagonal matrix solver (Gauss elimination)
    *****/
    double p[SIZE],q[SIZE];
    double bot;

    p[0] = ae[0]/ap[0];
    q[0] = b[0]/ap[0];

    for (i=1; i<=(nn[zones]); ++i)
    {
        bot = ap[i] -aw[i] * p[i-1];
        p[i] = ae[i]/bot;
        q[i] = (b[i]+aw[i]*q[i-1])/bot;
    }
    T[nn[zones]]=q[nn[zones]];      // since p[nn[z]]=0 because ae[nn[z]]=0

    for (i=nn[zones]-1; i>= 0; --i)
    {
        T[i] = p[i]*T[i+1]+q[i];
    }
    //complete effort for second order accuracy, need to specially calculate
    // T[0] as documented on pg. 83 equation 5.44 of Patankar
    T[0] = (ae[0]*T[1] + aw[0]*(T[1]-T[2]) + b[0])/ap[0];

    // this check is needed to ensure convergence
    // because of non-linear source terms at boundary
    Tsum=0.0;
    ++j;                            // increment convergence counter
    for (i = 0; i <= ( nn[zones]); ++i)
    {//number of calls to TDMA
        Tsum=Tsum+(T[i]-Tconv[i])/(T[1]-Tair);
    }

    if (j>=100)                 // Check for infinite loop
    {//maximum number of iteration exceeded
        RtnTime = -2;
        goto end;
    }

    // Check for Convergence
    if (fabs(Tsum) > error ) goto twenty;
        j=0;                    // converged, reset counter

    // Swap Old for New for next time step
    for (i = 0; i <= (nn[zones]); ++i)
    {
        Told[i] = T[i];
    }

    // Check to see if it is time to deform grid and make sure you deform only once
    if ((ttime >= ptime) && (Flag == 0))
    {
         /******
         * deformation subroutine
         *****/
        /* Currently we deform up to nn[1] nodes by a specified amount.  We hope
           to be able to define a constitutive model based on node temperature,
           local stress from rolling, and previous density for actual deformation */

        int i;

        // delta =36.0;		 percent deformation,max 36% from Nic-Daines

        for (i=1; i<=(nn[1]); ++i)
        {
             Delx[i] = Delx[i]*(1.0 - delta/100.0);
              rho[i] =  rho[i]/(1.0 - delta/100.0);
                K[i] =    K[i]/(1.0 - delta/100.0);
        }

        Flag=1;					// deform has been called

       Depth = 0.0;
       for (i = 0; i<=nn[zones]; ++i)
       {
           Depth = Depth + Delx[i]/2.0;
           if (i==0) Depth = 0.0;
           Depth = Depth + Delx[i]/2.0;    // prints for center of control volume
           if (i==0) Depth = 0.0;
       }
    }

    //if (ttime == Delt) THMA[0] = Tinit[1];
    //if ((fmod(ttime,60) == 0) || (ttime == Delt))

    // calculate mean temperature in lift
    Tave = 0.0;
    for (i=1; i<= nn[1]; ++i)
    {
        Tave = Tave + T[i]/nn[1];
    }

    THMA.append(Tave);
    if(Tave >= tstart) timeStart = ttime/60.;
    else if(Tave >= tstop) timeStop = rnd2dec(ttime/60., 0);

    // don't want to run out to 12 hours, what we are really worried about
    // is if the lift has cooled below the cutoff required temperature, whatever
    // that is, for now we know that people care about magic numbers of 120 F
    // for the lowest temperature and 180 F for minimum rolling temp.

    if ((Tave < tstop) && (RtnTime == -1)) RtnTime = nint(ttime/60.0);
    if ((Tave < 60. ) && (ttime == Delt ))      //cooled so fast we did not run!
    {
        RtnTime = -2;
        goto end;
    }
    if (Tave < 60. ) goto end;  //cooled to 122 Deg F so stop!

    if ((ttime >= 5.0*60.0) && (ttime < 5.0*60.+Delt))
    {
        for (i = 0; i<=nn[zones]; ++i)     // print out initial node values
        {
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
        }
    }

    if ((ttime >= 10.0*60.0) && (ttime < 10.0*60+Delt))
    {
        for (i = 0; i<=nn[zones]; ++i)     // print out initial node values
        {
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
        }
    }

    if ((ttime >= 15.0*60.0) && (ttime < 15.0*60+Delt))
    {
        for (i = 0; i<=nn[zones]; ++i)     // print out initial node values
        {
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
        }
    }

    if ((ttime >= 20.0*60.0) && (ttime < 20.0*60+Delt))
    {
        for (i = 0; i<=nn[zones]; ++i)     // print out initial node values
        {
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
            Depth = Depth + Delx[i]/2.0;
            if(i==0) Depth = 0.0;
        }
    }

    if (ttime < stime) goto ten;

end:
    timeStop = rnd2dec(timeStop, 0);
    if(timeStop > 120.)
    {
        timeStop = 120.;
        tStop = ">120";
    }
    else
    {
        tStop = txtdbl(timeStop, 0);
    }

    bcalc = true;
    bList[4] = true; // calculate if file is saved & opened
    ui->exportButton->setDisabled(false);

    PaveCool::update(); // repaints window
}
