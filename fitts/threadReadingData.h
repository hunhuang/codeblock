#ifndef threadData
#define threadData

#include <qtcore/qstring>
#include <qtcore/qpoint>
#include <qtopengl/qtopengl>
#include "glwidget.h"

class GLWidget;
class monitorMouseVelocity : public QThread
{
	Q_OBJECT
public:
	monitorMouseVelocity(GLWidget * main1);
	bool running = false;

private:
	GLWidget * widget;
	int lastx, lasty;

	public slots:
	void run();
signals:
	void finished();
	void error(QString err);

};
inline monitorMouseVelocity::monitorMouseVelocity(GLWidget * main1){
	widget = main1;
	running = true;
};

#endif
