


#include "mainwindow.h"
#include "HTMLManual.h"
#include <QtWidgets\qfiledialog.h>
#include <windows.h>

#include <qtnetwork/qhostinfo>

#include <qtcore/qtendian>
#include <qtcore/qevent>
#include <iostream>
#include <fstream>
#include <sstream>
void threadReceiveSignal::run()
{
//	FILE * open;
//	open = fopen("loidata.txt", "r");
//	char line[200];
//	int frame, markers;
//	fgets(line, sizeof(line), open);
//
//	sscanf(line, "Number of frames: %d\n", &frame);
//	fgets(line, sizeof(line), open);
//	sscanf(line, "Number of markers: %d\n", &markers);
//	fgets(line, sizeof(line), open);
//	fgets(line, sizeof(line), open);
//	fgets(line, sizeof(line), open);
//
//	for (int i = 0; i < frame; i++)
//	{
//		fgets(line, sizeof(line), open);
//		main->vec3D.clear();
//		main->vec6D.clear();
//		float x, y, z;
//		int tmp;
//		std::istringstream iss((string(line)));
//
//		iss >> tmp;
//		for (int j = 0; j < markers; j++)
//		{
//			iss >> x >> y >> z;
//			NDIPosition3D point1{ x,y,z };
//			main->vec3D.push_back(point1);
//		}
//		main->update3DList(main->vec3D);
//
//		for (int j = 0; j < markers*2; j++)
//		{
//			NDIErrorTransformation neww;
//			neww.rotation1 = { 1, 1, 1 };
//			neww.error = i/100.0;
//			neww.translation = { i, i + 1, i + 2 };
//			main->vec6D.push_back(neww);
//		}
//		main->update6DList(main->vec6D);
//
//
//		msleep(200);
//	}
//
//	fclose(open);
}
void MainWindow::receive3D()
{
	threadReceiveSignal* thread = new threadReceiveSignal(this);
	thread->start();

}






MainWindow::MainWindow()
{
	// Set mainwindow icon and title.
	setWindowIcon(QIcon("data/qt_logo.png"));
	setWindowTitle(tr("Fitts' Task 2D"));
	(void)statusBar();
	// Configure OpenGL widget.
	wdgOpenGL = new GLWidget;
	wdgOpenGL->setFocusPolicy(Qt::ClickFocus);
	wdgOpenGL->setMinimumSize(wdgOpenGLSizeMin.width(), wdgOpenGLSizeMin.height());
	wdgOpenGL->setMaximumSize(wdgOpenGLSizeMax.width(), wdgOpenGLSizeMax.height());
	wdgOpenGL->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	// Event filter to manage events on the OpenGL widget here (mainwindow)!
	wdgOpenGL->installEventFilter(this);
	connect(this, SIGNAL(keyFPressed()), SLOT(toggleFullScreen()));
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
	tmrOpenGL = new QTimer((QObject*)wdgOpenGL);
	QTimer::connect(tmrOpenGL, SIGNAL(timeout()), (QObject*)wdgOpenGL, SLOT(timerIdle()));
	tmrOpenGL->start(1);
	// ...
	//showMaximized();
	tutor = new QTextBrowser();
	tutor->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	tutor->setWindowTitle("ARAKNES Sim - Tutorial");
	tutor->setSizePolicy(QSizePolicy::Fixed , QSizePolicy::Fixed);
	tutor->setFixedSize(800, 600);
	QStringList tu(QString("data/tutorial"));
	tutor->setSearchPaths(tu);

	tutor->setSource(QUrl("tutorial.html"));

}


// Stot to manage the right mouse double click on the OpenGL widget.
void MainWindow::toggleFullScreen()
{
	// Check if OpenGL widget is in fullscreen mode.
	if (wdgOpenGLFullScreen) {
		// Configure the OpenGL widget in normal mode.
		wdgOpenGL->setParent(this);
		wdgOpenGL->resize(wdgOpenGLPrevSize);
		wdgOpenGL->overrideWindowFlags(wdgOpenGLPrevWndFlags);
		wdgOpenGL->show();
		wdgOpenGLFullScreen = false;
		// Reset OpenGL widget (Fitts cirle selection and mouse click).
		wdgOpenGL->resetSelection();
		wdgOpenGL->resetClick();
		// Reset the layout.
		loGridCentral->addWidget(wdgOpenGL, 0, 0);
		// ...
		//wdgOpenGL->setFocus(Qt::PopupFocusReason);
	}
	else {
		// Configure the OpenGL widget in fullscreen mode.
		wdgOpenGLPrevWndFlags = wdgOpenGL->windowFlags();
		wdgOpenGLPrevSize = wdgOpenGL->size();
		wdgOpenGL->setParent(NULL);
		wdgOpenGL->setWindowFlags(wdgOpenGL->windowFlags() |
			Qt::CustomizeWindowHint |
			//Qt::WindowStaysOnTopHint |
			Qt::WindowMaximizeButtonHint |
			Qt::WindowCloseButtonHint);
		wdgOpenGL->setWindowState(wdgOpenGL->windowState() |
			Qt::WindowFullScreen);
		wdgOpenGL->show();
		wdgOpenGL->enableTracking();
		wdgOpenGLFullScreen = true;
		// Reset OpenGL widget (Fitts cirle selection and mouse click).
		wdgOpenGL->resetSelection();
		wdgOpenGL->resetClick();
		// ...
		//wdgOpenGL->setFocus(Qt::PopupFocusReason);
	}
}


// ...
void MainWindow::toggleDebug() { wdgOpenGL->toggleDebugInfo(); }


// ...
void MainWindow::openExperiment()
{
	//printf( "mainwindow::opening_mesh_file\n" );
	//stopSim();
	//simstartbtn->setEnabled(false);
	//QString tmpfilename = QFileDialog::getOpenFileName( this, "Choose a file to open", "../tetramesh/", "Meshes (*.mesh)" );
	//if( !( tmpfilename.isEmpty() ) )
	//{
	//	//QByteArray fn = tmpfilename.toAscii();
	//	QByteArray fn = tmpfilename.toLatin1();
	//	const char* file_name = fn.constData();
	//	if( meshtype == MainWindow::ParticleMesh )
	//	{
	//		//if(	glwidget->loadParticleMesh( file_name ) )
	//		//{
	//		//	// Update status bar.
	//		//	meshfilename = tmpfilename;
	//		//	statusBar()->showMessage( QString( "%1   (Particle)" ).arg(meshfilename) );
	//		//	// Update side bar.
	//		//	mesheedit->setValue( glwidget->getYoungModulus() );
	//		//	meshesedit->setValue( glwidget->getSurfaceYoungModulus() );
	//		//	meshrhoedit->setValue( glwidget->getDensity() );
	//		//	stvkbox->hide();
	//		//	particlebox->show();
	//		//	vertexiedit->setValue(0);
	//		//	// Update buttons.
	//		//	meshparamsbtn->setEnabled(true);
	//		//	simstartbtn->setEnabled(true);
	//		//	// Log action.
	//		//	printf( "mainwindow::particle_mesh_loaded\n" );
	//		//}
	//		//else
	//		//{
	//				statusBar()->showMessage( "Particle mesh loading failed.", 3000 );
	//				printf( "mainwindow::loading_mesh_failed\n" );
	//		//}
	//	}
	//	//else if( meshtype == MainWindow::StVKMesh )
	//	//{
	//	//	if(	glwidget->loadStVKMesh( file_name ) )
	//	//	{
	//	//		// Update status bar.
	//	//		meshfilename = tmpfilename;
	//	//		statusBar()->showMessage( QString( "%1   (StVK)" ).arg(meshfilename) );
	//	//		// Update side bar.
	//	//		mesheedit->setValue( glwidget->getYoungModulus() );
	//	//		meshesedit->setValue( glwidget->getSurfaceYoungModulus() );
	//	//		meshrhoedit->setValue( glwidget->getDensity() );
	//	//		stvkbox->show();
	//	//		particlebox->hide();
	//	//		vertexiedit->setValue(0);
	//	//		// Update buttons.
	//	//		meshparamsbtn->setEnabled(true);
	//	//		simstartbtn->setEnabled(true);
	//	//		// Log action.
	//	//		printf( "mainwindow::stvk_mesh_loaded\n" );
	//	//	}
	//	//	else
	//	//	{
	//	//		statusBar()->showMessage( "StVK mesh loading failed.", 3000 );
	//	//		printf( "mainwindow::loading_mesh_failed\n" );
	//	//	}
	//	//}
	//	//else
	//	//{
	//	//	statusBar()->showMessage( "Mesh type undefined.", 3000 );
	//	//	printf( "mainwindow::opening_mesh_type_undefined\n" );
	//	//}
	//}
	//else {
	//	statusBar()->showMessage( "File open failed.", 3000 );
	//	printf( "mainwindow::opening_mesh_file_failed\n" ); }
}


void MainWindow::exitApplication() { wdgOpenGL->exitWidget(); QApplication::exit(0); }


//void MainWindow::modeFullScreen()
//{
//	onDoubleClicked();
//	//showFullScreen();
//	//menuBar()->hide();
//	//statusBar()->hide();
//}


//void MainWindow::modeWindowed()
//{
//	//onDoubleClicked();
//	//showNormal();
//	//move( wndPos.x(), wndPos.y() );
//	//resize( wndSize.width(), wndSize.height() );
//	//menuBar()->show();
//	//statusBar()->show();
//}


void MainWindow::aboutApplication() {
	QMessageBox::about(this, tr("LOI HUYNH AND TURINI"),
		tr("HEY THERE"));
}

void MainWindow::exportFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
		"/export.txt",
		tr("Text File (*.txt)"));
	FILE * pFile;
	pFile = fopen(qPrintable( fileName) , "w");
	fputs("Products of OERC", pFile);
	fclose(pFile);


}

void MainWindow::createActions()
{
	// Action: open experiment.
	actOpenExperiment = new QAction(tr("Open &Experiment"), this);
	actOpenExperiment->setShortcut(tr("Ctrl+E"));
	connect(actOpenExperiment, SIGNAL(triggered()), this, SLOT(openExperiment()));
	// Action: export
	actExport = new QAction(tr("Export to text File"), this);
	actExport->setShortcut(tr("Ctrl+Q"));
	connect(actExport, SIGNAL(triggered()), this, SLOT(exportFile()));

	// Action: exit.
	actExit = new QAction(tr("E&xit"), this);
	actExit->setShortcut(tr("Ctrl+X"));
	connect(actExit, SIGNAL(triggered()), this, SLOT(exitApplication()));
	// Action: fullscreen mode.
	actFullScreenView = new QAction(tr("&Full Screen Mode"), this);
	actFullScreenView->setShortcut(tr("Ctrl+F"));
	connect(actFullScreenView, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
	// Action: view debug info.
	actDebugView = new QAction(tr("&Debug Info"), this);
	actDebugView->setShortcut(tr("Ctrl+D"));
	connect(actDebugView, SIGNAL(triggered()), this, SLOT(toggleDebug()));
	// Action: about.
	actAbout = new QAction(tr("&About"), this);
	connect(actAbout, SIGNAL(triggered()), this, SLOT(aboutApplication()));
	actShowHTML = new QAction(tr("&Show HTML Manual"), this);
	connect(actShowHTML, SIGNAL(triggered()), this, SLOT(showHTML()));
	connect( btnConnectOnOff , SIGNAL(clicked()), this, SLOT(receive3D()));


}
void MainWindow::showHTML()
{

	tutor->show();

}

void MainWindow::createMenu()
{
	menuFile = menuBar()->addMenu(tr("&File"));
	menuFile->addAction(actOpenExperiment);
	menuFile->addAction(actExport);
	menuFile->addSeparator();
	menuFile->addAction(actExit);

	menuView = menuBar()->addMenu(tr("&View"));
	menuView->addAction(actFullScreenView);
	menuView->addAction(actDebugView);
	menuHelp = menuBar()->addMenu(tr("&Help"));
	menuHelp->addAction(actAbout);
	menuHelp->addAction(actShowHTML);


}

void MainWindow::anotherMessage()
{
	QMessageBox::about(this, tr("TMP"),
		tr("HEY THERE1"));
}
void MainWindow::createPanel()
{
	//// General mesh params box widget.
	//meshbox = new QGroupBox;
	//meshbox->setTitle("General Mesh Parameters");
	//meshbox->setFixedWidth(wndsidebarwidth);
	//// Particle mesh params box widget.
	//particlebox = new QGroupBox;
	//particlebox->setTitle("Particle Mesh Parameters");
	//particlebox->setFixedWidth(wndsidebarwidth);
	//// StVK mesh params box widget.
	//stvkbox = new QGroupBox;
	//stvkbox->setTitle("StVK Mesh Parameters");
	//stvkbox->setFixedWidth(wndsidebarwidth);
	//stvkbox->hide();
	//// Vertex params box widget.
	//vertexbox = new QGroupBox;
	//vertexbox->setTitle("Vertex Parameters");
	//vertexbox->setFixedWidth(wndsidebarwidth);
	//// Edge params box widget.
	//edgebox = new QGroupBox;
	//edgebox->setTitle("Edge Parameters");
	//edgebox->setFixedWidth(wndsidebarwidth);
	//// Mesh E widgets.
	//meshelbl = new QLabel("E [Pa] [uN/mm^2] [0, +inf)   ");
	//mesheedit = new QDoubleSpinBox();
	//mesheedit->setDecimals(3);
	//mesheedit->setSingleStep(1.0);
	//mesheedit->setMaximum(999999.0);
	//mesheedit->setEnabled(false);
	//// Mesh E-s (surface) widgets.
	//mesheslbl = new QLabel("E-surf [Pa] [uN/mm^2] [0, +inf)   ");
	//meshesedit = new QDoubleSpinBox();
	//meshesedit->setDecimals(3);
	//meshesedit->setSingleStep(1.0);
	//meshesedit->setMaximum(999999.0);
	//meshesedit->setEnabled(false);
	//// Mesh rho widgets.
	//meshrholbl = new QLabel("Rho [g/mm^3] (0, +inf)   ");
	//meshrhoedit = new QDoubleSpinBox();
	//meshrhoedit->setDecimals(3);
	//meshrhoedit->setSingleStep(0.001);
	//meshrhoedit->setMaximum(999999.999);
	//meshrhoedit->setEnabled(false);
	//// Particle mesh k damp widgets.
	//particlekdamplbl = new QLabel("K Damp [uN*sec/mm] (?, ?)   ");
	//particlekdampedit = new QDoubleSpinBox();
	//particlekdampedit->setDecimals(3);
	//particlekdampedit->setSingleStep(0.1);
	//particlekdampedit->setMaximum(999999.999);
	//particlekdampedit->setEnabled(false);
	//// StVK nu widgets.
	//stvknulbl = new QLabel("Nu [0, 0.5]   ");
	//stvknuedit = new QDoubleSpinBox();
	//stvknuedit->setDecimals(3);
	//stvknuedit->setSingleStep(0.001);
	//stvknuedit->setEnabled(false);
	//// StVK alpha widgets.
	//stvkalphalbl = new QLabel("Alpha [Hz] (0, +inf)   ");
	//stvkalphaedit = new QDoubleSpinBox();
	//stvkalphaedit->setDecimals(3);
	//stvkalphaedit->setSingleStep(0.001);
	//stvkalphaedit->setEnabled(false);
	//// StVK beta widgets.
	//stvkbetalbl = new QLabel("Beta [sec] (0, +inf)   ");
	//stvkbetaedit = new QDoubleSpinBox();
	//stvkbetaedit->setDecimals(3);
	//stvkbetaedit->setSingleStep(0.001);
	//stvkbetaedit->setEnabled(false);
	//// StVK PCG max-iter widgets.
	//stvkpcgiterlbl = new QLabel("PCG max iter   ");
	//stvkpcgiteredit = new QSpinBox();
	//stvkpcgiteredit->setEnabled(false);
	//// StVK PCG error widgets.
	//stvkpcgerrlbl = new QLabel("PCG error   ");
	//stvkpcgerredit = new QDoubleSpinBox();
	//stvkpcgerredit->setDecimals(3);
	//stvkpcgerredit->setSingleStep(0.001);
	//stvkpcgerredit->setEnabled(false);
	//// Vertex index widgets.
	//vertexilbl = new QLabel("Index [0, #V-1]   ");
	//vertexiedit = new QSpinBox();
	//vertexiedit->setMaximum(99999);
	//// Vertex position widgets.
	//vertexplbl = new QLabel("Position [mm]   ");
	//vertexpxlbl = new QLabel("X");
	//vertexpylbl = new QLabel("Y");
	//vertexpzlbl = new QLabel("Z");
	//// Vertex mass widgets.
	//vertexmlbl = new QLabel("Mass [g] [0, +inf)   ");
	//vertexmedit = new QDoubleSpinBox();
	//vertexmedit->setDecimals(1);
	//vertexmedit->setSingleStep(1.0);
	//vertexmedit->setMaximum(100000.0);
	//vertexmedit->setEnabled(false);
	//// Vertex velocity widgets.
	//vertexvlbl = new QLabel("Velocity [mm/sec]   ");
	//vertexvxlbl = new QLabel("X");
	//vertexvylbl = new QLabel("Y");
	//vertexvzlbl = new QLabel("Z");
	//// Vertex fixed widgets.
	//vertexfixlbl = new QLabel("Fixed   ");
	//vertexfixcheck = new QCheckBox();
	//vertexfixbtn = new QPushButton("Apply");
	//vertexfixbtn->setEnabled(false);
	//// Edge index widgets.
	//edgeilbl = new QLabel("Index [0, #E-1]   ");
	//edgeiedit = new QSpinBox();
	//edgeiedit->setMaximum(99999);
	//// Edge k elongation widgets.
	//edgekelonglbl = new QLabel("K Elong [uN/mm] (?, ?)   ");
	//edgekelongedit = new QDoubleSpinBox();
	//edgekelongedit->setDecimals(1);
	//edgekelongedit->setSingleStep(0.1);
	//edgekelongedit->setMaximum(9999999.9);
	//edgekelongedit->setEnabled(false);
	//// Simulation buttons widgets.
	//meshparamsbtn = new QPushButton("Apply Params");
	//meshparamsbtn->setEnabled(false);
	//simstartbtn = new QPushButton("Start Sim");
	//simstartbtn->setEnabled(false);
	//simstopbtn = new QPushButton("Stop Sim");
	//simstopbtn->setEnabled(false);

	// ...
	edtServerIP = new QLineEdit(this);
	edtServerIP->setInputMask(QString("000.000.000.000;_"));
	edtServerIP->setFixedWidth(wndPanelWidth * 2 / 3);
	// ...
	btnConnectOnOff = new QPushButton(QString("Connect to Server"), this);
	btnConnectOnOff->setFixedWidth(wndPanelWidth / 3);

	// ...
	boxComboCommand = new QComboBox(this);
	boxComboCommand->addItem(QString("Bye"));
	boxComboCommand->addItem(QString("Send Current Frame"));
	boxComboCommand->addItem(QString("Send Current Frame 3D"));
	boxComboCommand->addItem(QString("Send Current Frame 6D"));
	boxComboCommand->addItem(QString("Send Parameters 3D 6D"));
	boxComboCommand->addItem(QString("Send Parameters All"));
	boxComboCommand->addItem(QString("Set Byte Order Big-Endian"));
	boxComboCommand->addItem(QString("Set Byte Order Little-Endian"));
	boxComboCommand->addItem(QString("Stream Frames All-Frames"));
	boxComboCommand->addItem(QString("Stream Frames All-Frames Force"));
	boxComboCommand->addItem(QString("Stream Frames Frequency:20 3D Analog"));
	boxComboCommand->addItem(QString("Stream Frames Frequency-Divisor:4 6D"));
	boxComboCommand->addItem(QString("Stream Frames Stop"));
	boxComboCommand->addItem(QString("Version 1.0"));
	boxComboCommand->setFixedWidth(wndPanelWidth * 2 / 3);
	// ...
	btnSendCommand = new QPushButton(QString("Send Command"), this);
	btnSendCommand->setFixedWidth(wndPanelWidth / 3);
	// ...
	tblWdg3D = new QTableWidget(2, 4, this);
	tblWdg3D->setFixedWidth(wndPanelWidth);
	QStringList tblWdg3DLabels;
	tblWdg3DLabels << "#" << "X" << "Y" << "Z";
	tblWdg3D->setHorizontalHeaderLabels(tblWdg3DLabels);
	//tblWdg3D->horizontalScrollBar()->setDisabled(true);
	tblWdg3D->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	//setColumnWidth(0, 80);
	//setColumnWidth(1, 40);
	//setColumnWidth(2, 20);
	tblWdg3D->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	// ...
	tblWdg6D = new QTableWidget(2, 8, this);
	tblWdg6D->setFixedWidth(wndPanelWidth);
	QStringList tblWdg6DLabels;
	tblWdg6DLabels << "#" << "Rx" << "Ry" << "Rz" << "X" << "Y" << "Z" << "Err";
	tblWdg6D->setHorizontalHeaderLabels(tblWdg6DLabels);
	//tblWdg6D->horizontalScrollBar()->setDisabled(true);
	tblWdg6D->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	tblWdg6D->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	// ...
	tblWdgAnalog = new QTableWidget(2, 2, this);
	tblWdgAnalog->setFixedWidth(wndPanelWidth);
	QStringList tblWdgAnalogLabels;
	tblWdgAnalogLabels << "#" << "Volt";
	tblWdgAnalog->setHorizontalHeaderLabels(tblWdgAnalogLabels);
	tblWdgAnalog->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	tblWdgAnalog->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void MainWindow::setLayouts()
{
	//// Mesh E layout.
	//meshelayout = new QHBoxLayout();
	//meshelayout->addWidget(meshelbl);
	//meshelayout->addWidget(mesheedit);
	//// Mesh E-s (surface) layout.
	//mesheslayout = new QHBoxLayout();
	//mesheslayout->addWidget(mesheslbl);
	//mesheslayout->addWidget(meshesedit);
	//// Mesh rho layout.
	//meshrholayout = new QHBoxLayout();
	//meshrholayout->addWidget(meshrholbl);
	//meshrholayout->addWidget(meshrhoedit);
	//// Mesh params layout.
	//meshboxlayout = new QVBoxLayout();
	//meshboxlayout->addLayout(meshelayout);
	//meshboxlayout->addLayout(mesheslayout);
	//meshboxlayout->addLayout(meshrholayout);
	//meshbox->setLayout(meshboxlayout);
	//// Particle mesh k damp layout.
	//particlekdamplayout = new QHBoxLayout();
	//particlekdamplayout->addWidget(particlekdamplbl);
	//particlekdamplayout->addWidget(particlekdampedit);
	//// Mesh params layout.
	//QVBoxLayout* particleboxlayout = new QVBoxLayout();
	//particleboxlayout->addLayout(particlekdamplayout);
	//particlebox->setLayout(particleboxlayout);
	//// StVK nu layout.
	//stvknulayout = new QHBoxLayout();
	//stvknulayout->addWidget(stvknulbl);
	//stvknulayout->addWidget(stvknuedit);
	//// StVK alpha layout.
	//stvkalphalayout = new QHBoxLayout();
	//stvkalphalayout->addWidget(stvkalphalbl);
	//stvkalphalayout->addWidget(stvkalphaedit);
	//// StVK beta layout.
	//stvkbetalayout = new QHBoxLayout();
	//stvkbetalayout->addWidget(stvkbetalbl);
	//stvkbetalayout->addWidget(stvkbetaedit);
	//// StVK PCG max-iter layout.
	//stvkpcgiterlayout = new QHBoxLayout();
	//stvkpcgiterlayout->addWidget(stvkpcgiterlbl);
	//stvkpcgiterlayout->addWidget(stvkpcgiteredit);
	//// StVK PCG error layout.
	//stvkpcgerrlayout = new QHBoxLayout();
	//stvkpcgerrlayout->addWidget(stvkpcgerrlbl);
	//stvkpcgerrlayout->addWidget(stvkpcgerredit);
	//// Vertex index layout.
	//vertexilayout = new QHBoxLayout();
	//vertexilayout->addWidget(vertexilbl);
	//vertexilayout->addWidget(vertexiedit);
	//// Vertex position layout.
	//vertexplayout = new QHBoxLayout();
	//vertexplayout->addWidget(vertexplbl);
	//vertexplayout->addWidget(vertexpxlbl);
	//vertexplayout->addWidget(vertexpylbl);
	//vertexplayout->addWidget(vertexpzlbl);
	//// Vertex mass layout.
	//vertexmlayout = new QHBoxLayout();
	//vertexmlayout->addWidget(vertexmlbl);
	//vertexmlayout->addWidget(vertexmedit);
	//// Vertex velocity layout.
	//vertexvlayout = new QHBoxLayout();
	//vertexvlayout->addWidget(vertexvlbl);
	//vertexvlayout->addWidget(vertexvxlbl);
	//vertexvlayout->addWidget(vertexvylbl);
	//vertexvlayout->addWidget(vertexvzlbl);
	//// Vertex fixed layout.
	//vertexfixlayout = new QHBoxLayout();
	//vertexfixlayout->addWidget(vertexfixlbl);
	//vertexfixlayout->addWidget(vertexfixcheck);
	//vertexfixlayout->addWidget(vertexfixbtn);
	//// Edge index layout.
	//edgeilayout = new QHBoxLayout();
	//edgeilayout->addWidget(edgeilbl);
	//edgeilayout->addWidget(edgeiedit);
	//// Edge k elongation layout.
	//edgekelonglayout = new QHBoxLayout();
	//edgekelonglayout->addWidget(edgekelonglbl);
	//edgekelonglayout->addWidget(edgekelongedit);
	//// StVK params layout.
	//stvkboxlayout = new QVBoxLayout();
	//stvkboxlayout->addLayout(stvknulayout);
	//stvkboxlayout->addLayout(stvkalphalayout);
	//stvkboxlayout->addLayout(stvkbetalayout);
	//stvkboxlayout->addLayout(stvkpcgiterlayout);
	//stvkboxlayout->addLayout(stvkpcgerrlayout);
	//stvkbox->setLayout(stvkboxlayout);
	//// Vertex params layout.
	//vertexboxlayout = new QVBoxLayout();
	//vertexboxlayout->addLayout(vertexilayout);
	//vertexboxlayout->addLayout(vertexplayout);
	//vertexboxlayout->addLayout(vertexmlayout);
	//vertexboxlayout->addLayout(vertexvlayout);
	//vertexboxlayout->addLayout(vertexfixlayout);
	//vertexbox->setLayout(vertexboxlayout);
	//// Edge params layout.
	//edgeboxlayout = new QVBoxLayout();
	//edgeboxlayout->addLayout(edgeilayout);
	//edgeboxlayout->addLayout(edgekelonglayout);
	//edgebox->setLayout(edgeboxlayout);

	// Server layout.
	loHBoxServer = new QHBoxLayout();
	loHBoxServer->addWidget(edtServerIP);
	loHBoxServer->addWidget(btnConnectOnOff);
	// Command layout.
	loHBoxCommand = new QHBoxLayout();
	loHBoxCommand->addWidget(boxComboCommand);
	loHBoxCommand->addWidget(btnSendCommand);
	// Side panel layout.
	loGridPanel = new QGridLayout();
	loGridPanel->setContentsMargins(0, 0, 0, 0);
	loGridPanel->addLayout(loHBoxServer, 0, 0);
	loGridPanel->addLayout(loHBoxCommand, 1, 0);
	loGridPanel->setRowStretch(2, 0);
	loGridPanel->addWidget(tblWdg3D, 3, 0);
	loGridPanel->addWidget(tblWdg6D, 4, 0);
	loGridPanel->addWidget(tblWdgAnalog, 5, 0);

	//panelLayout->addWidget( meshbox, 0, 0 );
	//panelLayout->addWidget( particlebox, 1, 0 );
	//panelLayout->addWidget( stvkbox, 1, 0 );
	//panelLayout->addWidget( meshparamsbtn, 2, 0 );
	//panelLayout->setRowStretch( 3, 1 );
	//panelLayout->addWidget( vertexbox, 4, 0 );
	//panelLayout->addWidget( edgebox, 5, 0 );
	//panelLayout->setRowStretch( 6, 1 );
	//panelLayout->addWidget( simstartbtn, 7, 0 );
	//panelLayout->addWidget( simstopbtn, 8, 0 );

	// Central widget layout.
	loGridCentral = new QGridLayout();
	loGridCentral->setContentsMargins(5, 5, 5, 5);
	loGridCentral->addWidget(wdgOpenGL, 0, 0);
	loGridCentral->addLayout(loGridPanel, 0, 1);
}


void MainWindow::resizeEvent(QResizeEvent* e)
{
	wndSize.rwidth() = e->size().width();
	wndSize.rheight() = e->size().height();
	resize(wndSize.rwidth(), wndSize.rheight());
}


void MainWindow::moveEvent(QMoveEvent* e)
{
	wndPos.rx() = e->pos().x();
	wndPos.ry() = e->pos().y();
}


void MainWindow::closeEvent(QCloseEvent* e)
{
	exitApplication();
}


bool MainWindow::onInitDialog()
{

	return true;  // Return TRUE  unless you set the focus to a control.
}


// Updates client / server controls.
void MainWindow::updateControls()
{
	QString sTmp = "Server is down";
	static unsigned int lastUpdate = 0;
	unsigned int now = 0;
	unsigned int elapsed = 0;

	//// m_sServerState = sTmp;
	//m_CtrlServerStatus.SetWindowText(sTmp.GetBuffer(sTmp.GetLength()));
	//sTmp.ReleaseBuffer();

	// Client part.

}



// Updates analog values in analog list.
void MainWindow::updateAnalogList(std::vector<float>& v)
{
	char szText[100];
	int i;
	int numDataItems = (int)vecAnalog.size();
	if (numDataItems >= 0) {
		//int numRows = m_ListAnalog.GetItemCount();
		int numRows = tblWdgAnalog->rowCount();
		if (numRows != numDataItems) {
			// Adjust number if list items based on new data.
			if (numRows < numDataItems) {
				for (i = numRows; i< numDataItems; i++) {
					//m_ListAnalog.InsertItem(LVIF_TEXT | LVIF_STATE, i, "", 0, LVIS_SELECTED, 0, 0);
					// Marker: #, Volt.
					QTableWidgetItem* item0 = new QTableWidgetItem(tr("Item #"));
					QTableWidgetItem* item1 = new QTableWidgetItem(tr("Item Volt"));
					tblWdgAnalog->insertRow(i);
					tblWdgAnalog->setItem(i, 0, item0);
					tblWdgAnalog->setItem(i, 1, item1);
				}
			}
			else {
				if (numRows > 0) {
					for (i = numRows - 1; i >= numDataItems; i--) {
						//m_ListAnalog.DeleteItem(i);
						tblWdgAnalog->removeRow(i);
					}
				}
			}
		}
	}
	for (i = 0; i < numDataItems; i++) {
		//sprintf(szText, "%d", i + 1);
		//m_ListAnalog.SetItemText(i, 0, szText);
		//SetText(szText, vAnalog[i]);
		//m_ListAnalog.SetItemText(i, 1, szText);
	}
}


// ...
void MainWindow::setText(char* c, float v) {}


// Event filter to manage events on the OpenGL widget here (mainwindow)!
bool MainWindow::eventFilter(QObject* t, QEvent* e)
{
	if (t == wdgOpenGL) {
		if (e->type() == QEvent::KeyPress) {
			QKeyEvent* ke = static_cast<QKeyEvent*>(e);
			if (ke->key() == Qt::Key_F) {
				emit keyFPressed();
				return QMainWindow::eventFilter(t, e);
			}
		}
	}
	return QMainWindow::eventFilter(t, e);
}


