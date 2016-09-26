#include <qtoolbar.h>
#include <qdesktopwidget.h>
#include <qaction.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qprinter.h>
#include <qprintdialog.h>
#include "table.h"
#include "file_vf.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_zoom.h"
#include "win_sphere.h"
#include "win_main.h"
#include "p4application.h"
#include "win_plot.h"
#include "win_print.h"
#include "win_event.h"

extern QPrinter * p4printer;

extern void spherePlotLine( QWinSphere * sp, double * p1, double * p2, int color );
extern void spherePlotPoint( QWinSphere * sp, double * p, int color );
extern void SetP4WindowTitle( QWidget *, QString );

QZoomWnd::QZoomWnd( QPlotWnd * main, int id, double x1, double y1, double x2, double y2 )
    : QMainWindow()
{
    QToolBar * toolBar1;
    parent = main;
    zoomid = id;

//    QPalette palette;
//    palette.setColor(backgroundRole(), QXFIGCOLOR(CBACKGROUND) );
//    setPalette(palette);

    if( p4smallicon != NULL )
        setWindowIcon( *p4smallicon );

    toolBar1 = new QToolBar( "ZoomBar1", this );
    toolBar1->setMovable(false);

    ActClose = new QAction("Close", this);
    ActClose->setShortcut( Qt::ALT + Qt::Key_E );
    connect(ActClose, SIGNAL(triggered()), this, SLOT(OnBtnClose()));
    toolBar1->addAction( ActClose );

    ActRefresh = new QAction("Refresh", this);
    ActRefresh->setShortcut( Qt::ALT + Qt::Key_R );
    connect(ActRefresh, SIGNAL(triggered()), this, SLOT(OnBtnRefresh()));
    toolBar1->addAction( ActRefresh );

    ActPrint = new QAction("Print", this);
    ActPrint->setShortcut( Qt::ALT + Qt::Key_P );
    connect(ActPrint, SIGNAL(triggered()), this, SLOT(OnBtnPrint()));
    toolBar1->addAction( ActPrint );

#ifdef TOOLTIPS
    ActClose->setToolTip( "Closes the plot window, all subwindows and zoom window"  );
    ActRefresh->setToolTip( "Redraw the plot window" );
    ActPrint->setToolTip( "Opens the print window" );
#endif

    statusBar()->showMessage( "Ready" );
    addToolBar( Qt::TopToolBarArea, toolBar1 );

    sphere = new QWinSphere( this, statusBar(), true, x1, y1, x2, y2 );
    sphere->show();
    setCentralWidget(sphere );
    resize( NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW );
    sphere->SetupPlot();

//  if( ThisVF->evaluated )
        SetP4WindowTitle( this, "Phase Portrait - Zoom" );
//  else
//      SetP4WindowTitle( this, "Phase Portrait - Zoom (*)" );
}

QZoomWnd::~QZoomWnd()
{
    delete sphere;
    sphere = NULL;
}

void QZoomWnd::Signal_Changed( void )
{
//  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere->Signal_Changed();
}

void QZoomWnd::Signal_Evaluating( void )
{
//  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere->Signal_Evaluating();
}

void QZoomWnd::Signal_Evaluated( void )
{
//  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
}

void QZoomWnd::OnBtnClose( void )
{
    int * data = (int *)malloc( sizeof(int) );
    *data = zoomid;

    QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data );
    p4app->postEvent( parent, e1 );
}

bool QZoomWnd::close( void )
{
    int * data = (int *)malloc( sizeof(int) );
    *data = zoomid;

    QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data );
    p4app->postEvent( parent, e1 );

    return QMainWindow::close();
}

void QZoomWnd::OnBtnRefresh( void )
{
    parent->getDlgData();
    sphere->refresh();
}

void QZoomWnd::OnBtnPrint( void )
{
    int res;
    double lw, ss;
    QPrintDlg * pdlg;
    pdlg = new QPrintDlg( this, 0 );
    int result = pdlg->exec();
    res = pdlg->GetChosenResolution();
    lw = pdlg->GetChosenLineWidth();
    ss = pdlg->GetChosenSymbolSize();

    delete pdlg;

    if( result != P4PRINT_NONE )
    {
        if( result == P4PRINT_DEFAULT || result == -P4PRINT_DEFAULT )
        {
            p4printer->setResolution(res);
            QPrintDialog dialog(p4printer, this);
            if( !dialog.exec() )
                return;
            res = p4printer->resolution();
        }

        if( result < 0 )
            sphere->preparePrinting( -result, true, res, lw, ss );
        else
            sphere->preparePrinting( result, false, res, lw, ss );
        sphere->print();
        sphere->finishPrinting();
    }
}

void QZoomWnd::configure( void )
{
    statusBar()->showMessage( "Ready" );        // reset status bar
    plot_l = spherePlotLine;                // setup line/plot pointing to routines of the sphere window
    plot_p = spherePlotPoint;
    sphere->SetupPlot();                    // setup sphere window (define pixel transformations)
    sphere->update();
    // delete print window
    // delete xfig window
}

void QZoomWnd::customEvent( QEvent * _e )
{
    QP4Event * e;
    e = (QP4Event *)_e;

    if( e->type() == TYPE_OPENZOOMWINDOW ||
        e->type() == TYPE_ORBIT_EVENT ||
        e->type() == TYPE_SELECT_ORBIT ||
        e->type() == TYPE_SEP_EVENT ||
        e->type() == TYPE_SELECT_LCSECTION )
    {
        QP4Event * newe = new QP4Event( e->type(), e->data() );
        p4app->postEvent( parent, newe );
        return;
    }

    QMainWindow::customEvent(e);
}

void QZoomWnd::hideEvent ( QHideEvent * h )
{
    UNUSED(h);
    if( !isMinimized() )
    {
        int * data = (int *)malloc( sizeof(int) );
        *data = zoomid;

        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data );
        p4app->postEvent( parent, e1 );
    }
}

void QZoomWnd::AdjustHeight( void )
{
    int w, h, m;
    double deltaw, deltah;
    
    sphere->adjustToNewSize();
    
    w = width();
    h = height() + sphere->idealh - sphere->h;

    m = p4app->desktop()->height();
    m -= m/10;                      // occuppy at most 90% of the screen's height
    
    if( h > m )
    {
        deltah = (double)(h - m);
        deltaw = deltah;
        deltaw *= sphere->dx;
        deltaw /= sphere->dy;
        
        h -= (int)(deltah + 0.5);
        w -= (int)(deltaw + 0.5);
    }
    else if( sphere->idealh < MINHEIGHTPLOTWINDOW )
    {
        deltah = (double)(MINHEIGHTPLOTWINDOW - sphere->idealh);
        deltaw = deltah;
        deltaw *= sphere->dx;
        deltaw /= sphere->dy;
        
        h += (int)(deltah + 0.5);
        w += (int)(deltaw + 0.5);
        m = p4app->desktop()->width();
        m -= m/10;
        if( w > m ) w = m;            // occupy at most 90 % of the screen's width
    }
    resize( w, h );
    statusBar()->showMessage( "Ready." );
}

