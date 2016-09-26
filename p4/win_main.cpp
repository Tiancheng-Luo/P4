#include <qpixmap.h>
#include <qevent.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qmenu.h>
#include "custom.h"
#include "table.h"
#include "file_vf.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_main.h"
#include "win_find.h"
#include "win_plot.h"
#include "win_about.h"
#include "p4application.h"

extern QString GetP4HelpPath( void );       // defined in main.cpp
extern void SetP4WindowTitle( QWidget *, QString );

void MakeButtonPixmaps( const QPalette & );

QPixmap * Pixmap_TriangleUp = NULL;
QPixmap * Pixmap_TriangleDown = NULL;

QStartDlg::QStartDlg( const QString & autofilename )
    : QWidget()
{
    // general initialization

    QString cap;
    cap = "Planar Polynomial Phase Portraits";
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    // define controls

    if( p4smallicon != NULL )
        setWindowIcon( *p4smallicon );

    btn_quit = new QPushButton( "&Quit", this );
#ifdef DOCK_FINDWINDOW
    MakeButtonPixmaps( btn_quit->palette() );
    btn_find = new QPushButton( "", this );

#ifdef AUTO_OPEN_FINDWINDOW
    btn_find->setIcon( QIcon(*Pixmap_TriangleUp) );
#else
    btn_find->setIcon( QIcon(*Pixmap_TriangleDown) );
#endif
    btn_find->setFixedSize(btn_find->sizeHint());
#else
    btn_find = new QPushButton( "&Find", this );
#endif
    btn_view = new QPushButton( "Vie&w", this );
    btn_plot = new QPushButton( "&Plot", this );
    btn_help = new QPushButton( "&Help", this );
    btn_about = new QPushButton( "Ab&out P4...", this );

    if( autofilename.length() == 0 )
        edt_name = new QLineEdit( DEFAULTFILENAME, this );
    else
        edt_name = new QLineEdit( ThisVF->filename = autofilename, this );
    QLabel * p4name = new QLabel( " &Name: ", this );
    p4name->setBuddy(edt_name);
    p4name->setFont( *(p4app->BoldFont) );

    edt_name->setSelection( 0, strlen(DEFAULTFILENAME) );
    edt_name->setCursorPosition( strlen(DEFAULTFILENAME) );

    QPushButton * btn_browse = new QPushButton( "&Browse", this );

#ifdef TOOLTIPS
    btn_quit->setToolTip( "Quit P4" );
#ifdef DOCK_FINDWINDOW
    btn_find->setToolTip( "Unfolds/hides the \"Find Singular Points\" window" );
#else
    btn_find->setToolTip( "Opens/closes the \"Find Singular Points\" window" );
#endif
    btn_view->setToolTip( "View results of the symbolic manipulator after evaluation" );
    btn_plot->setToolTip( "Draw singular points, orbits and separatrices" );
    btn_help->setToolTip( "Shows extensive help on the use of P4" );
    edt_name->setToolTip( "Enter the filename of the vector field here.\n"
                             "You do not need to add the extension (.inp).\n" );
    btn_browse->setToolTip( "Search for vector field files on your system" );
    btn_about->setToolTip( "Displays information about the program P4, its version and main settings" );
#endif

    // define placement of controls

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    QHBoxLayout * buttons = new QHBoxLayout();
#ifdef DOCK_FINDWINDOW
    buttons->addWidget( btn_quit );
    buttons->addWidget( btn_view );
    buttons->addWidget( btn_plot );
    buttons->addWidget( btn_help );
    buttons->addWidget( btn_find );
#else
    buttons->addWidget( btn_quit );
    buttons->addWidget( btn_find );
    buttons->addWidget( btn_view );
    buttons->addWidget( btn_plot );
    buttons->addWidget( btn_help );
#endif
    mainLayout->addLayout( buttons );

    QHBoxLayout * names = new QHBoxLayout();
    names->addWidget( p4name );
    names->addWidget( edt_name );
    names->addWidget( btn_browse );
    names->addWidget( btn_about );
    mainLayout->addLayout( names );
    
    setLayout( mainLayout );
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    // connections

    viewmenu = new QMenu( this );

    QAction * ActFin = new QAction("Fini&te", this);
    ActFin->setShortcut( Qt::ALT + Qt::Key_T );
    connect(ActFin, SIGNAL(triggered()), this, SLOT(OnViewFinite()));
    viewmenu->addAction( ActFin );

    QAction * ActInf = new QAction("&Infinite", this);
    ActInf->setShortcut( Qt::ALT + Qt::Key_I );
    connect(ActInf, SIGNAL(triggered()), this, SLOT(OnViewInfinite()));
    viewmenu->addAction( ActInf );
    
    btn_view->setMenu( viewmenu );

    QObject::connect( btn_quit, SIGNAL(clicked()), this, SLOT(OnQuit()) );
    QObject::connect( btn_find, SIGNAL(clicked()), this, SLOT(OnFind()) );
    QObject::connect( btn_plot, SIGNAL(clicked()), this, SLOT(OnPlot()) );
    QObject::connect( btn_help, SIGNAL(clicked()), this, SLOT(OnHelp()) );
    QObject::connect( btn_about, SIGNAL(clicked()), this, SLOT(OnAbout()) );
    QObject::connect( btn_browse, SIGNAL(clicked()), this, SLOT(OnBrowse()) );
    QObject::connect( edt_name, SIGNAL(textChanged(const QString &)), this,
                            SLOT(OnFilenameChange(const QString &)) );

    // setting focus

    edt_name->setFocus();

    // finishing

//  btn_plot->setDisabled( true );
//  viewmenu->setItemEnabled( 1, false );
//  viewmenu->setItemEnabled( 2, false );

    Help_Window = NULL;
    Find_Window = NULL;
    View_Infinite_Window = NULL;
    View_Finite_Window = NULL;
    Plot_Window = NULL;
#ifdef AUTO_OPEN_FINDWINDOW
    OnFind();
#else
    if( autofilename.length() != 0 )
        OnFind();
#endif

    SetP4WindowTitle( this, cap );
}

void QStartDlg::OnFind( void )
{
    // show find dialog

    if( Find_Window == NULL )
    {
        Find_Window = new QFindDlg( this );
        Find_Window->show();
        Find_Window->raise();
#ifdef DOCK_FINDWINDOW
        mainLayout->addWidget(Find_Window);
        btn_find->setIcon( QIcon(*Pixmap_TriangleUp) );
#endif
    }
    else
    {
#ifdef DOCK_FINDWINDOW
        delete Find_Window;
        Find_Window = NULL;
        btn_find->setIcon( QIcon(*Pixmap_TriangleDown) );
#else
        Find_Window->show();
        Find_Window->raise();
        btn_find->setIcon( QIcon(*Pixmap_TriangleUp) );
#endif
    }
}

void QStartDlg::OnHelp( void )
{
    // display help
    QTextBrowser * hlp;
    QString helpname;

    helpname = GetP4HelpPath();
    if( helpname.isNull() )
    {
        QMessageBox::critical( this, "P4",
           "Cannot determine P4 install location!\nPlease re-check installation.\n" );
        return;
    }

    helpname += QDir::separator();
    helpname += P4HELPFILE;

    if( QFile( helpname ).exists() == false )
    {
        QMessageBox::critical( this, "P4",
            "Cannot find P4 help files in install location!\nPlease re-check installation.\n" );
        return;
    }

    hlp = (QTextBrowser *)Help_Window;

    if( hlp == NULL )
    {
        hlp = new QTextBrowser();
    }

    hlp->setSource( QUrl::fromLocalFile( helpname ) );
    hlp->resize( 640, 480 );
    if( p4smallicon != NULL )
        hlp->setWindowIcon( *p4smallicon );

    SetP4WindowTitle( hlp, "P4 Help" );
    hlp->show();
    hlp->raise();

    Help_Window = hlp;
}

void QStartDlg::OnPlot( void )
{
    // show plot window

    if( Find_Window != NULL )
        Find_Window->GetDataFromDlg();

    VFResults.DeleteVF();                               // delete any previous result object
    if( !VFResults.ReadTables( ThisVF->getbarefilename() ) )    // read maple/reduce results
    {
        delete Plot_Window;
        Plot_Window = NULL;

        QMessageBox::critical( this, "P4",
                           "Cannot read computation results.\n"
                           "Please check the input-vector field and parameters!\n"
                            );

        return;
    }

    VFResults.SetupCoordinateTransformations();

    if( Plot_Window == NULL )
    {
        Plot_Window = new QPlotWnd( this );
    }

    Plot_Window->configure();                           // configure plot window
    Plot_Window->show();
    Plot_Window->raise();
    Plot_Window->AdjustHeight();
}

void QStartDlg::OnQuit( void )
{
    if( Plot_Window != NULL )
    {
        delete Plot_Window;
        Plot_Window = NULL;
    }
    if( Find_Window != NULL )
    {
        delete Find_Window;
        Find_Window = NULL;
    }
    if( Help_Window != NULL )
    {
        delete Help_Window;
        Help_Window = NULL;
    }
    if( View_Finite_Window != NULL )
    {
        delete View_Finite_Window;
        View_Finite_Window = NULL;
    }
    if( View_Infinite_Window != NULL )
    {
        delete View_Infinite_Window;
        View_Infinite_Window = NULL;
    }
    
    if( ThisVF != NULL )
    {
        if( ThisVF->ProcessText != NULL )
        {
            if( ThisVF->ProcessButton != NULL )
            {
                delete ThisVF->ProcessButton;
                ThisVF->ProcessButton = NULL;
            }
            delete ThisVF->ProcessText;
            ThisVF->ProcessText = NULL;
        }
    }

    close();
}

void QStartDlg::OnFilenameChange( const QString & fname )
{
    ThisVF->filename = fname;
}

void QStartDlg::Signal_Evaluating( void )
{
    // disable view button, disable plot button:

    btn_view->setEnabled(false);
    btn_plot->setEnabled(false);

    // Transfer signal to Find_Window:

    if( Find_Window != NULL )
    {
        Find_Window->Signal_Evaluating();
    }

    // Transfer signal to Plot_Window:

    if( Plot_Window != NULL )
        Plot_Window->Signal_Evaluating();
}

void QStartDlg::Signal_Evaluated( void )
{
    // enable view button, disable plot button:

    btn_view->setEnabled(true);
    btn_plot->setEnabled(true);

    // freshen view finite/infinite windows if they are open:

    if( View_Finite_Window != NULL )
    {
        QString fname;

        if( Find_Window != NULL )
            Find_Window->GetDataFromDlg();

        fname = ThisVF->getfilename_finresults();

        if( ThisVF->FileExists(fname)  )
        {
            View_Finite_Window = Showtext( View_Finite_Window, "View results at the finite region", fname );
        }
        else
        {
            if( View_Finite_Window != NULL )
            {
                ((QTextEdit *)View_Finite_Window)->clear();
                ((QTextEdit *)View_Finite_Window)->setCurrentFont( *(p4app->BoldCourierFont) );
                ((QTextEdit *)View_Finite_Window)->insertPlainText(
                    "\nA study at the finite region is not available!" );
            }
        }
    }

    if( View_Infinite_Window != NULL )
    {
        QString fname;

        if( Find_Window != NULL )
            Find_Window->GetDataFromDlg();

        fname = ThisVF->getfilename_infresults();
        if( ThisVF->FileExists(fname) )
        {
            View_Infinite_Window = Showtext( View_Infinite_Window, "View results at infinity", fname );
            if( ThisVF->typeofstudy == TYPEOFSTUDY_FIN || ThisVF->typeofstudy == TYPEOFSTUDY_ONE )
            {
                // mark: data invalid according to vf information

                View_Infinite_Window->setFont( *(p4app->CourierFont) );
            }
        }
        else
        {
            if( View_Infinite_Window != NULL )
            {
                ((QTextEdit *)View_Infinite_Window)->clear();
                ((QTextEdit *)View_Infinite_Window)->setCurrentFont( *(p4app->BoldCourierFont) );
                ((QTextEdit *)View_Infinite_Window)->insertPlainText(
                    "\nA study at infinity is not available!" );
            }
        }
    }

    // Transfer signal to Find_Window:

    if( Find_Window != NULL )
    {
        Find_Window->Signal_Evaluated();
    }

    // Transfer signal to Plot_Window:

    if( Plot_Window != NULL )
    {
        VFResults.DeleteVF();                               // delete any previous result object
        if( !VFResults.ReadTables( ThisVF->getbarefilename() ) ) // read maple/reduce results
        {
            QMessageBox::critical( this, "P4",
                       "Cannot read computation results.\n"
                       "Please check the input-vector field and parameters!\n"
                        );
        }
        VFResults.SetupCoordinateTransformations();
        Plot_Window->Signal_Evaluated();
    }

    // the vector field may be changed during evaluation.  In that
    // case, the flag ThisVF->changed is set, so the newly evaluated context
    // is immediately marked as "old".

    if( ThisVF->changed )
        Signal_Changed();
}

void QStartDlg::Signal_Saved( void )
{
    //
}

void QStartDlg::Signal_Loaded( void )
{
    //
}

void QStartDlg::Signal_Changed( void )
{
    if( View_Finite_Window != NULL )
    {
        View_Finite_Window->setFont( *(p4app->CourierFont) );
    }

    if( View_Infinite_Window != NULL )
    {
        View_Infinite_Window->setFont( *(p4app->CourierFont) );
    }
    if( Plot_Window != NULL )
    {
        Plot_Window->Signal_Changed();
    }
}

void QStartDlg::OnBrowse( void )
{
    QString result;
    
    result = QFileDialog::getOpenFileName(
           this,
           "Select vector field configuration file",
           edt_name->text(),   
           "Vector Field files (*.inp);;All Files (*.*)",
           0, QFileDialog::DontConfirmOverwrite );

    if( !(result.isNull()) )
    {
        edt_name->setText( result );        
    }
}

void QStartDlg::OnAbout( void )
{
    QP4AboutDlg * pdlg;
    pdlg = new QP4AboutDlg( this, 0 );
    pdlg->exec();
    delete pdlg;
}

void QStartDlg::OnViewFinite()
{
    // display help

    QString fname;

    if( Find_Window != NULL )
        Find_Window->GetDataFromDlg();

    fname = ThisVF->getfilename_finresults();

    if( ThisVF->FileExists(fname) == false )
    {
        if( ThisVF->typeofstudy == TYPEOFSTUDY_INF  )
        {
            QMessageBox::critical( this, "P4",
                                   "A study at the finite region was not requested!\n"
                                    );
            return;
        }
        QMessageBox::critical( this, "P4",
                               "Cannot open the result file.\nPlease re-evaluate, or check filename.\n"
                                );
        return;
    }

    View_Finite_Window = Showtext( View_Finite_Window, "View results at the finite region", fname );
    View_Finite_Window->show();
    View_Finite_Window->raise();

    if( ThisVF->typeofstudy == TYPEOFSTUDY_INF  )
    {
        // mark: data invalid according to vf information

        View_Finite_Window->setFont( *(p4app->CourierFont) );
        return;
    }
}

void QStartDlg::OnViewInfinite()
{
    QString fname;

    if( Find_Window != NULL )
        Find_Window->GetDataFromDlg();

    fname = ThisVF->getfilename_infresults();
    if( ThisVF->FileExists(fname) == false )
    {
        if( ThisVF->typeofstudy == TYPEOFSTUDY_FIN || ThisVF->typeofstudy == TYPEOFSTUDY_ONE )
        {
            QMessageBox::critical( this, "P4",
                                   "A study at infinity was not requested!\n"
                                    );
            return;
        }
        QMessageBox::critical( this, "P4",
                               "Cannot open the result file.\nPlease re-evaluate, or check filename.\n"
                                );

        return;
    }

    View_Infinite_Window = Showtext( View_Infinite_Window, "View results at infinity", fname );
    View_Infinite_Window->show();
    View_Infinite_Window->raise();
    if( ThisVF->typeofstudy == TYPEOFSTUDY_FIN || ThisVF->typeofstudy == TYPEOFSTUDY_ONE )
    {
        // mark: data invalid according to vf information

        View_Infinite_Window->setFont( *(p4app->CourierFont) );
        return;
    }
}

QWidget * QStartDlg::Showtext( QWidget * win, QString caption, QString fname )
{
    bool shown;
    QTextEdit * result;

    result = (QTextEdit *)win;
    if( result == NULL )
        result = new QTextEdit();
    else
        result->clear();

    if( p4smallicon != NULL )
        result->setWindowIcon( *p4smallicon );

    QFile f( fname );
    if ( !f.open( QIODevice::ReadOnly ) )
    {
        delete result;
        return NULL;
    }

    shown = false;
    if( result->isHidden() == false )
    {
        shown = true;
        result->hide();
    }
    result->setFont( *(p4app->CourierFont) );
    result->insertPlainText("");
    result->setReadOnly(true);

    QTextStream t( &f );

    while( !t.atEnd() )
    {
        QString s = t.readLine();
        result->append( s );
    }
    f.close();

    result->resize(640,480);

//  result->setCaption( caption );
    SetP4WindowTitle( result, caption );

    if( ThisVF->evaluated == false )
        result->setFont( *(p4app->CourierFont) );

    if( shown )
    {
        result->show();
        result->raise();
    }

    return result;
}

void QStartDlg::closeEvent( QCloseEvent* ce )
{
    int result;

    // This event get to process window-system close events.
    // A close event is subtly different from a hide event:
    // hide may often mean "iconify" but close means that the window is going away for good. 

    if( ThisVF == NULL )
    {
        ce->accept();
        return;
    }

    if( Find_Window != NULL )
        Find_Window->GetDataFromDlg();

    if( ThisVF->changed == false )
    {
        ce->accept();
        return;
    }

    result = QMessageBox::information( this, "P4",
                    "The vector field has been changed since "
                    "the last save.",
                    "&Save Now", "&Cancel", "&Leave Anyway",
                    0, 1 );

    if( result == 2 )
        ce->accept();
    else
    {
        if( result == 0 )
        {
            if( ThisVF->save() )
            {
                ce->accept();
                return;
            }
            QMessageBox::critical( this, "P4",
                                   "Unable to save the input vector field.\n"
                                   "Please check permissions on the write location.\n"
                                  );
        }
        ce->ignore();
    }
}

void QStartDlg::customEvent( QEvent * e )
{
    switch( (int)(e->type()) )
    {
    case TYPE_SIGNAL_EVALUATING:    Signal_Evaluating();    break;
    case TYPE_SIGNAL_EVALUATED:     Signal_Evaluated();     break;
    case TYPE_SIGNAL_CHANGED:       Signal_Changed();       break;
    case TYPE_SIGNAL_LOADED:        Signal_Loaded();        break;
    case TYPE_SIGNAL_SAVED:         Signal_Saved();         break;
    case TYPE_CLOSE_PLOTWINDOW:
        if( Plot_Window != NULL )
        {
            delete Plot_Window;
            Plot_Window = NULL;
        }
        break;
    default:
        QWidget::customEvent(e);
    }
}

void MakeButtonPixmaps( const QPalette & qcg )
{
    QPainter * p;
    QPolygon up(3);
    QPolygon down(3);

    Pixmap_TriangleUp = new QPixmap( 16, 16 );
    Pixmap_TriangleDown = new QPixmap( 16, 16 );

    down.setPoints( 3,   4,4,  12,4,   8,10  );
    up.setPoints( 3,    4,10,  12,10,  8, 4  );

    p = new QPainter();
    p->begin( Pixmap_TriangleUp );
    p->setBackground( qcg.color( QPalette::Normal, QPalette::Button ) );    
    p->eraseRect(0,0,16,16);
    p->setPen( qcg.color( QPalette::Normal, QPalette::ButtonText ) );
    p->setBrush( qcg.color( QPalette::Normal, QPalette::ButtonText ) );
    p->drawPolygon( up );
    p->end();

    p->begin( Pixmap_TriangleDown );
    p->setBackground( qcg.color( QPalette::Normal, QPalette::Button ) );    
    p->eraseRect(0,0,16,16);
    p->setPen( qcg.color( QPalette::Normal, QPalette::ButtonText ) );
    p->setBrush( qcg.color( QPalette::Normal, QPalette::ButtonText ) );
    p->drawPolygon( down );
    p->end();

    delete p;
}
