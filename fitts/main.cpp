



#include <freeglut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <qtwidgets/qapplication>

#include <qtcore/qtimer>

#include "mainwindow.h"
#include "HTMLManual.h"


int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	MainWindow mainwnd;
	mainwnd.show();
	return app.exec();
}


