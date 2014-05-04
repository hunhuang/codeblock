


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <qtwidgets/qmainwindow>
#include <qtwidgets/qgridlayout>
#include <qtwidgets/qaction>
#include <qtwidgets/qmenubar>
#include <qtwidgets/qstatusbar>
#include <qtwidgets/qapplication>
#include <qtwidgets/qsizepolicy>
#include <qtwidgets/qtablewidget>
#include <qtwidgets/qtablewidgetitem>
#include <qtwidgets/qheaderview>
#include <qtwidgets/qlineedit>
#include <qtwidgets/qpushbutton>
#include <qtwidgets/qhboxlayout>
#include <qtwidgets/qcombobox>

#include <qtcore/qstring>
#include <qtcore/qevent>

#include "glwidget.h"




class MainWindow : public QMainWindow
{

	Q_OBJECT

public:

	// ...
	std::string		ndiServerState;
	int				numIPSelection;
	std::string		ndiClientState;
	QString			hostname;
	unsigned int	dwIP;
	std::string		command;
	std::string		performance;


    MainWindow();

	// ...
	//afx_msg void OnBnCConnect();
	//afx_msg void OnBnServerStart();
	//afx_msg void OnBnServerStop();
	//afx_msg void OnClickedRadio();
	//CIPAddressCtrl m_CtrlIP; // WIN common IP Address control.
	//afx_msg void OnBnCDisconnect();
	//afx_msg void OnTimer(UINT nIDEvent);
	//CStatic m_CtrlServerStatus; // WIN static control.
	//CStatic m_CtrlClientStatus;
	//afx_msg void OnBnSendCommand();
	//CStatic m_CtrlPerf;  // WIN static control.
	//CStatic m_CtrlServerText;
	//CStatic m_CtrlClientString;
	//CComboBox m_CtrlComboCommand;
	//CStatic m_CtrlFrame3d;
	//CStatic m_CtrlFrame6d;
	//CStatic m_CtrlFrameAnalog;
	//CStatic m_CtrlTime3d;
	//CStatic m_CtrlTime6d;
	//CStatic m_CtrlTimeAnalog;

private slots:

	void openExperiment();
	void exitApplication();
	void aboutApplication();
	void toggleFullScreen();
	void toggleDebug();
	void anotherMessage();
	void exportFile();
	void showHTML();
	void receive3D();
signals:

	void keyFPressed();

private:
	void runReceived3D();
	// NDI Real-Time C3D Client.

	unsigned int							numStateTimer;
	unsigned int							numDataTimer;
	int										numBytes;
	int										numFrames;
	//CListCtrl m_List3d; // MS list view control.
	//CListCtrl m_List6d; // MS list view control.
	//CListCtrl m_ListAnalog; // MS list view control.

	std::vector<float>						vecAnalog;
	unsigned int							uFrame3D;
	unsigned int							uFrame6D;
	unsigned int							uFrameAnalog;

	QTimer*		tmrOpenGL;

	// OpenGL widget.
	GLWidget*				wdgOpenGL;
	QSize					wdgOpenGLSizeMin = QSize( 800, 600 );
	QSize					wdgOpenGLSizeMax = QSize( 1600, 900 );
	Qt::WindowFlags			wdgOpenGLPrevWndFlags;
	QSize					wdgOpenGLPrevSize;
	bool					wdgOpenGLFullScreen = false;

	// Widgets.
    QWidget*				wdgCentral;				// Central widget.

	// Menus.
    QMenu*					menuFile;
	QMenu*					menuView;
    QMenu*					menuHelp;
	QTextBrowser*			tutor;
	// GUI menu actions.
	QAction*				actOpenExperiment;
    QAction*				actExit;
	QAction*				actFullScreenView;
	QAction*				actDebugView;
    QAction*				actAbout;
	QAction*				actExport;
	QAction*				actShowHTML;
	// Layouts
	QGridLayout*			loGridCentral;			// Central widget layout.
	QGridLayout*			loGridPanel;			// Side panel layout.

	// ...
	QTableWidget*			tblWdg3D;
	QTableWidget*			tblWdg6D;
	QTableWidget*			tblWdgAnalog;

	// ...
	QLineEdit*				edtServerIP;
	QPushButton*			btnConnectOnOff;
	QHBoxLayout*			loHBoxServer;

	// ....
	QComboBox*				boxComboCommand;
	QPushButton*			btnSendCommand;
	QHBoxLayout*			loHBoxCommand;

	// ...
	QPoint					wndPos = QPoint( 20, 20);		// Mainwindow position.
	QSize					wndSize = QSize( 1024, 768 );	// Mainwindow size.
	int						wndPanelWidth = 400;			// Mainwindow side panel width.

	// Misc GUI functions.
	void createActions();
	void createMenu();
	void createPanel();
	void setLayouts();

protected:

	void resizeEvent( QResizeEvent* e );
	void moveEvent( QMoveEvent* e );
	void closeEvent( QCloseEvent* e );

	// ...
	bool onInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	//afx_msg LRESULT OnPacket(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnPacketString(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnPacketProperties(WPARAM wParam, LPARAM lParam);
	//DECLARE_MESSAGE_MAP()
	void updateControls();

	void updateAnalogList(std::vector<float>& v);
	void setText(char* c, float v);

	bool eventFilter( QObject* t, QEvent* e );

};
class threadReceiveSignal : public QThread
{
	Q_OBJECT
public:
	threadReceiveSignal(MainWindow * main1);
private:
	MainWindow * main;
public slots:
	void run();

signals:
	void finished();
	void error(QString err);

};
inline threadReceiveSignal::threadReceiveSignal(MainWindow * main1){
	main = main1;
};

#endif


