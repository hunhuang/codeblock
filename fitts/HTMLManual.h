


#ifndef HTMLManual_H
#define HTMLManual_H


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


class HTMLManual : public QMainWindow
{

	Q_OBJECT

public:

	HTMLManual();

	private slots:

	void exitApplication();

signals:

	void keyFPressed();

private:
// Widgets.
	QWidget*				wdgCentral;				// Central widget.
	QTextBrowser*			wdgText;
	// Menus.
	QMenu*					menuFile;

	// GUI menu actions.
	QAction*				actExit;
	// Layouts
	QGridLayout*			loGridCentral;			// Central widget layout.
	QGridLayout*			loGridPanel;			// Side panel layout.

	// ...
	QHBoxLayout*			loHBoxServer;

	QHBoxLayout*			loHBoxCommand;

	// ...
	QPoint					wndPos = QPoint(20, 20);		// Mainwindow position.
	QSize					wndSize = QSize(1024, 768);	// Mainwindow size.
	int						wndPanelWidth = 400;			// Mainwindow side panel width.

	// Misc GUI functions.
	void createActions();
	void createMenu();
	void createPanel();
	void setLayouts();

protected:

	void resizeEvent(QResizeEvent* e);
	void moveEvent(QMoveEvent* e);
	void closeEvent(QCloseEvent* e);
	bool onInitDialog();

};

#endif


