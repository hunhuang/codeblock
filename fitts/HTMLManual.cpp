


#include "HTMLManual.h"

#include <windows.h>

#include <qtnetwork/qhostinfo>

#include <qtcore/qtendian>
#include <qtcore/qevent>


HTMLManual::HTMLManual()
{
	// Set mainwindow icon and title.
	setWindowIcon(QIcon("data/qt_logo.png"));
	setWindowTitle(tr("Fitts' Task 2D"));
	(void)statusBar();
	
	// Create panels layouts actions and menus.
	createPanel();
	setLayouts();
	createActions();
	createMenu();
	// Configure central widget.
	wdgCentral = new QWidget;
	setCentralWidget(wdgCentral);
	wdgCentral->setLayout(loGridCentral);
	// Configure mainwindow position and size.
	move(wndPos.x(), wndPos.y());
	resize(wndSize.width(), wndSize.height());
	// Configure timer.
	
	// ...
	//showMaximized();
}


// Stot to manage the right mouse double click on the OpenGL widget.


void HTMLManual::exitApplication() { QApplication::exit(0); }

void HTMLManual::createActions()
{
	
	// Action: exit.
	actExit = new QAction(tr("E&xit"), this);
	actExit->setShortcut(tr("Ctrl+X"));
	connect(actExit, SIGNAL(triggered()), this, SLOT(exitApplication()));
	
}

void HTMLManual::createMenu()
{
	menuFile = menuBar()->addMenu(tr("&File"));
	menuFile->addAction(actExit);


}

void HTMLManual::createPanel()
{
	
	wdgText = new QTextBrowser(this);
	wdgText->setFixedWidth(wndPanelWidth);
	
}


void HTMLManual::setLayouts()
{
	

	// Server layout.
	loHBoxServer = new QHBoxLayout();
	loHBoxServer->addWidget(wdgText);
	// Command layout.
	loHBoxCommand = new QHBoxLayout();
	// Side panel layout.
	loGridPanel = new QGridLayout();
	loGridPanel->setContentsMargins(0, 0, 0, 0);
	loGridPanel->addLayout(loHBoxServer, 0, 0);
	loGridPanel->addLayout(loHBoxCommand, 1, 0);
	loGridPanel->setRowStretch(2, 0);
	
}


void HTMLManual::resizeEvent(QResizeEvent* e)
{
	wndSize.rwidth() = e->size().width();
	wndSize.rheight() = e->size().height();
}


void HTMLManual::moveEvent(QMoveEvent* e)
{
	wndPos.rx() = e->pos().x();
	wndPos.ry() = e->pos().y();
}


void HTMLManual::closeEvent(QCloseEvent* e)
{
	exitApplication();
}


bool HTMLManual::onInitDialog()
{
	QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
	QString hostName = QHostInfo::localHostName();
	qDebug() << hostName;
	
	return true;  // Return TRUE  unless you set the focus to a control.
}
