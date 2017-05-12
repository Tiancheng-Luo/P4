/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "win_main.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"
#include "p4settings.h"
#include "win_about.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextBrowser>

static void makeButtonPixmaps(const QPalette &);

QStartDlg *g_p4stardlg = nullptr;

QPixmap *Pixmap_TriangleUp = nullptr;
QPixmap *Pixmap_TriangleDown = nullptr;

QStartDlg::QStartDlg(const QString &autofilename) : QWidget()
{
    // general initialization

    QString cap;
    cap = "Planar Polynomial Phase Portraits";
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    // define controls

    if (g_p4smallicon != nullptr)
        setWindowIcon(*g_p4smallicon);

    btn_quit = new QPushButton("&Quit", this);
#ifdef DOCK_FINDWINDOW
    makeButtonPixmaps(btn_quit->palette());
    btn_find = new QPushButton("", this);

#ifdef AUTO_OPEN_FINDWINDOW
    btn_find->setIcon(QIcon(*Pixmap_TriangleUp));
#else
    btn_find->setIcon(QIcon(*Pixmap_TriangleDown));
#endif
    btn_find->setFixedSize(btn_find->sizeHint());
#else
    btn_find = new QPushButton("&Find", this);
#endif
    btn_view = new QPushButton("Vie&w", this);
    btn_plot = new QPushButton("&Plot", this);
    btn_help = new QPushButton("&Help", this);
    btn_about = new QPushButton("Ab&out P4...", this);

    if (autofilename.length() == 0)
        edt_name = new QLineEdit(DEFAULTFILENAME, this);
    else
        edt_name = new QLineEdit(g_ThisVF->filename_ = autofilename, this);
    QLabel *p4name = new QLabel(" &Name: ", this);
    p4name->setBuddy(edt_name);
    p4name->setFont(*(g_p4app->boldFont_));

    edt_name->setSelection(0, strlen(DEFAULTFILENAME));
    edt_name->setCursorPosition(strlen(DEFAULTFILENAME));

    QPushButton *btn_browse = new QPushButton("&Browse", this);

#ifdef TOOLTIPS
    btn_quit->setToolTip("Quit P4");
#ifdef DOCK_FINDWINDOW
    btn_find->setToolTip("Unfolds/hides the \"Find Singular Points\" window");
#else
    btn_find->setToolTip("Opens/closes the \"Find Singular Points\" window");
#endif
    btn_view->setToolTip(
        "View results of the symbolic manipulator after evaluation");
    btn_plot->setToolTip("Draw singular points, orbits and separatrices");
    btn_help->setToolTip("Shows extensive help on the use of P4");
    edt_name->setToolTip("Enter the filename of the vector field here.\n"
                         "You do not need to add the extension (.inp).\n");
    btn_browse->setToolTip("Search for vector field files on your system");
    btn_about->setToolTip("Displays information about the program P4, its "
                          "version and main settings");
#endif

    // define placement of controls

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *buttons = new QHBoxLayout();
#ifdef DOCK_FINDWINDOW
    buttons->addWidget(btn_quit);
    buttons->addWidget(btn_view);
    buttons->addWidget(btn_plot);
    buttons->addWidget(btn_help);
    buttons->addWidget(btn_find);
#else
    buttons->addWidget(btn_quit);
    buttons->addWidget(btn_find);
    buttons->addWidget(btn_view);
    buttons->addWidget(btn_plot);
    buttons->addWidget(btn_help);
#endif
    mainLayout_->addLayout(buttons);

    QHBoxLayout *names = new QHBoxLayout();
    names->addWidget(p4name);
    names->addWidget(edt_name);
    names->addWidget(btn_browse);
    names->addWidget(btn_about);
    mainLayout_->addLayout(names);

    setLayout(mainLayout_);
    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);

    // connections

    viewmenu = new QMenu(this);

    QAction *ActFin = new QAction("Fini&te", this);
    ActFin->setShortcut(Qt::ALT + Qt::Key_T);
    connect(ActFin, SIGNAL(triggered()), this, SLOT(OnViewFinite()));
    viewmenu->addAction(ActFin);

    QAction *ActInf = new QAction("&Infinite", this);
    ActInf->setShortcut(Qt::ALT + Qt::Key_I);
    connect(ActInf, SIGNAL(triggered()), this, SLOT(OnViewInfinite()));
    viewmenu->addAction(ActInf);

    btn_view->setMenu(viewmenu);

    QObject::connect(btn_quit, SIGNAL(clicked()), this, SLOT(OnQuit()));
    QObject::connect(btn_find, SIGNAL(clicked()), this, SLOT(OnFind()));
    QObject::connect(btn_plot, SIGNAL(clicked()), this, SLOT(OnPlot()));
    QObject::connect(btn_help, SIGNAL(clicked()), this, SLOT(OnHelp()));
    QObject::connect(btn_about, SIGNAL(clicked()), this, SLOT(OnAbout()));
    QObject::connect(btn_browse, SIGNAL(clicked()), this, SLOT(OnBrowse()));
    QObject::connect(edt_name, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFilenameChange(const QString &)));

    // setting focus

    edt_name->setFocus();

    // finishing

    //  btn_plot->setDisabled( true );
    //  viewmenu->setItemEnabled( 1, false );
    //  viewmenu->setItemEnabled( 2, false );

    Help_Window = nullptr;
    Find_Window = nullptr;
    View_Infinite_Window = nullptr;
    View_Finite_Window = nullptr;
    Plot_Window = nullptr;
#ifdef AUTO_OPEN_FINDWINDOW
    OnFind();
#else
    if (autofilename.length() != 0)
        OnFind();
#endif

    setP4WindowTitle(this, cap);
}

void QStartDlg::OnFind(void)
{
    // show find dialog

    if (Find_Window == nullptr) {
        Find_Window = new QFindDlg(this);
        Find_Window->show();
        Find_Window->raise();
#ifdef DOCK_FINDWINDOW
        mainLayout_->addWidget(Find_Window);
        btn_find->setIcon(QIcon(*Pixmap_TriangleUp));
#endif
    } else {
#ifdef DOCK_FINDWINDOW
        delete Find_Window;
        Find_Window = nullptr;
        btn_find->setIcon(QIcon(*Pixmap_TriangleDown));
#else
        Find_Window->show();
        Find_Window->raise();
        btn_find->setIcon(QIcon(*Pixmap_TriangleUp));
#endif
    }
}

void QStartDlg::OnHelp(void)
{
    // display help
    QTextBrowser *hlp;
    QString helpname;

    helpname = getP4HelpPath();
    if (helpname.isNull()) {
        QMessageBox::critical(this, "P4", "Cannot determine P4 install "
                                          "location!\nPlease re-check "
                                          "installation.\n");
        return;
    }

    helpname += QDir::separator();
    helpname += P4HELPFILE;

    if (QFile(helpname).exists() == false) {
        QMessageBox::critical(this, "P4", "Cannot find P4 help files in "
                                          "install location!\nPlease re-check "
                                          "installation.\n");
        return;
    }

    hlp = (QTextBrowser *)Help_Window;

    if (hlp == nullptr) {
        hlp = new QTextBrowser();
    }

    hlp->setSource(QUrl::fromLocalFile(helpname));
    hlp->resize(640, 480);
    if (g_p4smallicon != nullptr)
        hlp->setWindowIcon(*g_p4smallicon);

    setP4WindowTitle(hlp, "P4 Help");
    hlp->show();
    hlp->raise();

    Help_Window = hlp;
}

void QStartDlg::OnPlot(void)
{
    // show plot window

    if (Find_Window != nullptr)
        Find_Window->getDataFromDlg();

    g_VFResults.deleteVF(); // delete any previous result object
    if (!g_VFResults.readTables(
            g_ThisVF->getbarefilename())) // read maple/reduce results
    {
        delete Plot_Window;
        Plot_Window = nullptr;

        QMessageBox::critical(
            this, "P4",
            "Cannot read computation results.\n"
            "Please check the input-vector field and parameters!\n");

        return;
    }
    if (!g_VFResults.readCurve(g_ThisVF->getbarefilename())) {
        // nothing, we simply don't have a curve for plotting
    }

    g_VFResults.setupCoordinateTransformations();

    if (Plot_Window == nullptr) {
        Plot_Window = new QPlotWnd(this);
    }

    Plot_Window->configure(); // configure plot window
    Plot_Window->show();
    Plot_Window->raise();
    Plot_Window->AdjustHeight();
}

void QStartDlg::OnQuit(void)
{
    if (Plot_Window != nullptr) {
        delete Plot_Window;
        Plot_Window = nullptr;
    }
    if (Find_Window != nullptr) {
        delete Find_Window;
        Find_Window = nullptr;
    }
    if (Help_Window != nullptr) {
        delete Help_Window;
        Help_Window = nullptr;
    }
    if (View_Finite_Window != nullptr) {
        delete View_Finite_Window;
        View_Finite_Window = nullptr;
    }
    if (View_Infinite_Window != nullptr) {
        delete View_Infinite_Window;
        View_Infinite_Window = nullptr;
    }

    if (g_ThisVF != nullptr) {
        delete g_ThisVF;
        g_ThisVF = nullptr;
    }

    close();
}

void QStartDlg::OnFilenameChange(const QString &fname)
{
    g_ThisVF->filename_ = fname;
}

void QStartDlg::signalEvaluating(void)
{
    // disable view button, disable plot button:

    btn_view->setEnabled(false);
    btn_plot->setEnabled(false);

    // Transfer signal to Find_Window:

    if (Find_Window != nullptr) {
        Find_Window->signalEvaluating();
    }

    // Transfer signal to Plot_Window:

    if (Plot_Window != nullptr)
        Plot_Window->signalEvaluating();
}

void QStartDlg::signalEvaluated(void)
{
    // enable view button, disable plot button:

    btn_view->setEnabled(true);
    btn_plot->setEnabled(true);

    // freshen view finite/infinite windows if they are open:

    if (View_Finite_Window != nullptr) {
        QString fname;

        if (Find_Window != nullptr)
            Find_Window->getDataFromDlg();

        fname = g_ThisVF->getfilename_finresults();

        if (g_ThisVF->fileExists(fname)) {
            View_Finite_Window = Showtext(
                View_Finite_Window, "View results at the finite region", fname);
        } else {
            if (View_Finite_Window != nullptr) {
                ((QTextEdit *)View_Finite_Window)->clear();
                ((QTextEdit *)View_Finite_Window)
                    ->setCurrentFont(*(g_p4app->boldCourierFont_));
                ((QTextEdit *)View_Finite_Window)
                    ->insertPlainText(
                        "\nA study at the finite region is not available!");
            }
        }
    }

    if (View_Infinite_Window != nullptr) {
        QString fname;

        if (Find_Window != nullptr)
            Find_Window->getDataFromDlg();

        fname = g_ThisVF->getfilename_infresults();
        if (g_ThisVF->fileExists(fname)) {
            View_Infinite_Window = Showtext(View_Infinite_Window,
                                            "View results at infinity", fname);
            if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
                g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
                // mark: data invalid according to vf information

                View_Infinite_Window->setFont(*(g_p4app->courierFont_));
            }
        } else {
            if (View_Infinite_Window != nullptr) {
                ((QTextEdit *)View_Infinite_Window)->clear();
                ((QTextEdit *)View_Infinite_Window)
                    ->setCurrentFont(*(g_p4app->boldCourierFont_));
                ((QTextEdit *)View_Infinite_Window)
                    ->insertPlainText(
                        "\nA study at infinity is not available!");
            }
        }
    }

    // Transfer signal to Find_Window:

    if (Find_Window != nullptr) {
        Find_Window->signalEvaluated();
    }

    // Transfer signal to Plot_Window:

    if (Plot_Window != nullptr) {
        g_VFResults.deleteVF(); // delete any previous result object
        if (!g_VFResults.readTables(
                g_ThisVF->getbarefilename())) // read maple/reduce results
        {
            QMessageBox::critical(
                this, "P4",
                "Cannot read computation results.\n"
                "Please check the input-vector field and parameters!\n");
        }
        g_VFResults.setupCoordinateTransformations();
        Plot_Window->signalEvaluated();
    }

    // the vector field may be changed during evaluation.  In that
    // case, the flag g_ThisVF->changed_ is set, so the newly evaluated context
    // is immediately marked as "old".

    if (g_ThisVF->changed_)
        signalChanged();
}

void QStartDlg::signalSaved(void)
{
    //
}

void QStartDlg::signalLoaded(void)
{
    //
}

void QStartDlg::signalChanged(void)
{
    if (View_Finite_Window != nullptr) {
        View_Finite_Window->setFont(*(g_p4app->courierFont_));
    }

    if (View_Infinite_Window != nullptr) {
        View_Infinite_Window->setFont(*(g_p4app->courierFont_));
    }
    if (Plot_Window != nullptr) {
        Plot_Window->signalChanged();
    }
}

void QStartDlg::OnBrowse(void)
{
    QString result;

    result = QFileDialog::getOpenFileName(
        this, "Select vector field configuration file", edt_name->text(),
        "Vector Field files (*.inp);;All Files (*.*)", 0,
        QFileDialog::DontConfirmOverwrite);

    if (!(result.isNull())) {
        edt_name->setText(result);
    }
}

void QStartDlg::OnAbout(void)
{
    QP4AboutDlg *pdlg;
    pdlg = new QP4AboutDlg(this, 0);
    pdlg->exec();
    delete pdlg;
    pdlg = nullptr;
}

void QStartDlg::OnViewFinite()
{
    // display help

    QString fname;

    if (Find_Window != nullptr)
        Find_Window->getDataFromDlg();

    fname = g_ThisVF->getfilename_finresults();

    if (g_ThisVF->fileExists(fname) == false) {
        if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_INF) {
            QMessageBox::critical(
                this, "P4",
                "A study at the finite region was not requested!\n");
            return;
        }
        QMessageBox::critical(this, "P4", "Cannot open the result "
                                          "file.\nPlease re-evaluate, or check "
                                          "filename.\n");
        return;
    }

    View_Finite_Window = Showtext(View_Finite_Window,
                                  "View results at the finite region", fname);
    View_Finite_Window->show();
    View_Finite_Window->raise();

    if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_INF) {
        // mark: data invalid according to vf information

        View_Finite_Window->setFont(*(g_p4app->courierFont_));
        return;
    }
}

void QStartDlg::OnViewInfinite()
{
    QString fname;

    if (Find_Window != nullptr)
        Find_Window->getDataFromDlg();

    fname = g_ThisVF->getfilename_infresults();
    if (g_ThisVF->fileExists(fname) == false) {
        if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
            g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
            QMessageBox::critical(this, "P4",
                                  "A study at infinity was not requested!\n");
            return;
        }
        QMessageBox::critical(this, "P4", "Cannot open the result "
                                          "file.\nPlease re-evaluate, or check "
                                          "filename.\n");

        return;
    }

    View_Infinite_Window =
        Showtext(View_Infinite_Window, "View results at infinity", fname);
    View_Infinite_Window->show();
    View_Infinite_Window->raise();
    if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
        g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
        // mark: data invalid according to vf information

        View_Infinite_Window->setFont(*(g_p4app->courierFont_));
        return;
    }
}

QWidget *QStartDlg::Showtext(QWidget *win, QString caption, QString fname)
{
    bool shown;
    QTextEdit *result;

    result = (QTextEdit *)win;
    if (result == nullptr)
        result = new QTextEdit();
    else
        result->clear();

    if (g_p4smallicon != nullptr)
        result->setWindowIcon(*g_p4smallicon);

    QFile f(fname);
    if (!f.open(QIODevice::ReadOnly)) {
        delete result;
        return nullptr;
    }

    shown = false;
    if (result->isHidden() == false) {
        shown = true;
        result->hide();
    }
    result->setFont(*(g_p4app->courierFont_));
    result->insertPlainText("");
    result->setReadOnly(true);

    QTextStream t(&f);

    while (!t.atEnd()) {
        QString s = t.readLine();
        result->append(s);
    }
    f.close();

    result->resize(640, 480);

    //  result->setCaption( caption );
    setP4WindowTitle(result, caption);

    if (g_ThisVF->evaluated_ == false)
        result->setFont(*(g_p4app->courierFont_));

    if (shown) {
        result->show();
        result->raise();
    }

    return result;
}

void QStartDlg::closeEvent(QCloseEvent *ce)
{
    int result;

    // This event get to process window-system close events.
    // A close event is subtly different from a hide event:
    // hide may often mean "iconify" but close means that the window is going
    // away for good.

    if (g_ThisVF == nullptr) {
        ce->accept();
        return;
    }

    if (Find_Window != nullptr)
        Find_Window->getDataFromDlg();

    if (g_ThisVF->changed_ == false) {
        ce->accept();
        return;
    }

    result = QMessageBox::information(
        this, "P4", "The vector field has been changed since "
                    "the last save.",
        "&Save Now", "&Cancel", "&Leave Anyway", 0, 1);

    if (result == 2)
        ce->accept();
    else {
        if (result == 0) {
            if (g_ThisVF->save()) {
                ce->accept();
                return;
            }
            QMessageBox::critical(
                this, "P4",
                "Unable to save the input vector field.\n"
                "Please check permissions on the write location.\n");
        }
        ce->ignore();
    }
}

void QStartDlg::customEvent(QEvent *e)
{
    switch ((int)(e->type())) {
    case TYPE_SIGNAL_EVALUATING:
        signalEvaluating();
        break;
    case TYPE_SIGNAL_EVALUATED:
        signalEvaluated();
        break;
    case TYPE_SIGNAL_CHANGED:
        signalChanged();
        break;
    case TYPE_SIGNAL_LOADED:
        signalLoaded();
        break;
    case TYPE_SIGNAL_SAVED:
        signalSaved();
        break;
    case TYPE_CLOSE_PLOTWINDOW:
        if (Plot_Window != nullptr) {
            delete Plot_Window;
            Plot_Window = nullptr;
        }
        break;
    default:
        QWidget::customEvent(e);
    }
}

void makeButtonPixmaps(const QPalette &qcg)
{
    QPainter *p;
    QPolygon up(3);
    QPolygon down(3);

    Pixmap_TriangleUp = new QPixmap(16, 16);
    Pixmap_TriangleDown = new QPixmap(16, 16);

    down.setPoints(3, 4, 4, 12, 4, 8, 10);
    up.setPoints(3, 4, 10, 12, 10, 8, 4);

    p = new QPainter();
    p->begin(Pixmap_TriangleUp);
    p->setBackground(qcg.color(QPalette::Normal, QPalette::Button));
    p->eraseRect(0, 0, 16, 16);
    p->setPen(qcg.color(QPalette::Normal, QPalette::ButtonText));
    p->setBrush(qcg.color(QPalette::Normal, QPalette::ButtonText));
    p->drawPolygon(up);
    p->end();

    p->begin(Pixmap_TriangleDown);
    p->setBackground(qcg.color(QPalette::Normal, QPalette::Button));
    p->eraseRect(0, 0, 16, 16);
    p->setPen(qcg.color(QPalette::Normal, QPalette::ButtonText));
    p->setBrush(qcg.color(QPalette::Normal, QPalette::ButtonText));
    p->drawPolygon(down);
    p->end();

    delete p;
    p = nullptr;
}
