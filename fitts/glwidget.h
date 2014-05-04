


#ifndef GLWIDGET_H
#define GLWIDGET_H



#include <freeglut.h>

#include <qtcore/qstring>
#include <qtcore/qpoint>

#include <qtopengl/qtopengl>

#include <qtgui/qcolor>
#include <qtgui/qfont>

#include <string>

#include "qtlogo.h"
#include "fps.h"


class monitorMouseVelocity;

class GLWidget : public QGLWidget
{

	Q_OBJECT

private:

	// ...
	enum TestState {	READY,
						CIRCLE_01, CIRCLE_02, CIRCLE_03, CIRCLE_04, CIRCLE_05,
						CIRCLE_06, CIRCLE_07, CIRCLE_08, CIRCLE_09, CIRCLE_10,
						CIRCLE_11, CIRCLE_12, CIRCLE_13, CIRCLE_14, CIRCLE_15,
						CIRCLE_16, CIRCLE_17, CIRCLE_18, CIRCLE_19, CIRCLE_20,
						CIRCLE_21, CIRCLE_22, CIRCLE_23, CIRCLE_24, CIRCLE_25,
						END };

	// General windget info.
	FPS				fps;
	bool			dbgFlag = false;
	QPoint			mouWinPos;
	QPoint			mouGLPos;
	float			wndAR;
	QFont			font;
	QColor			fontColorA = QColor( 204.0f, 204.0f, 204.0f, 255.0f );
	QColor			fontColorB = QColor( 0.0f, 0.0f, 0.0f, 255.0f );
	QColor			bgColor = QColor( 51.0f, 51.0f, 51.0f, 255.0f );
	bool			flagGrabKeyboard = false;
	QCursor			cursorClick = QCursor(Qt::CrossCursor);
	bool			grabScreenshot = false;

	int				intCurrentWidgetWidth=0;
	int				intCurrentWidgetHeight=0;
	// Selection.
	QPoint			slcClick;
	GLfloat			slcColor[4]; // See: selection implementation using the back-buffer.
	unsigned int 	slcIdx;
	QPoint			slcCenter;

	// Click.
	bool			clickFlag = false;
	QPoint			clickGLPos;
	QColor			clickColor = fontColorA;

	// Fitts' circle index: from color (RED) to index.
	// * : Fitts' task circle index stored into RED channel (0.12 corresponds to 12th circle);
	// ** : Index 0 means background (i.e. no circle selected).
	QColor fittsColorNormal = QColor( 204.0f, 51.0f, 51.0f, 255.0f );
	QColor fittsColorSelected = QColor( 51.0f, 153.0f, 51.0f, 255.0f );
	QColor fittsColorMarked = QColor( 51.0f, 51.0f, 153.0f, 255.0f );
	unsigned int fittsNum = 25;
	const unsigned int fittsNumReset = 25;
	const unsigned int fittsNumMin = 2;
	const unsigned int fittsNumMax = 25;

	//fittsSize in pixel
	int fittsSize = 20;
	const int fittsSizeReset = 40;
	const int fittsSizeMin = 10;
	const int fittsSizeMax = 70;
	const int fittsIncrease = 10;
	const int cursorSize = 10;

	//fitsRadius in
	int fittsRadius = 200;
	const int fittsRadiusReset = 350;
	TestState fittsStatus = TestState::READY;
	unsigned int fittsMarkedIdx = fittsNum;

	void initFont();
	void drawCircle( float x, float y, float radius, int slices );
	//size now is measure in pixel
	void drawFittsTask2D( int number, int size, float radius, bool selection, bool debug );
	void drawText();
	void grabWidgetScreenshot();
	unsigned int fittsIdx2Pos( unsigned int i );
	unsigned int fittsPos2Idx( unsigned int p );

public:
	bool checkInsideWidget(QPoint mouGLPos);
    GLWidget( QWidget* parent = 0 );
    ~GLWidget();
	void exitWidget();
	void toggleDebugInfo();
	void resetSelection();
	void resetClick();
	void enableTracking();
	void disableTracking();
	int curMousex = 0;
	int curMousey = 0;
	float curDeviceVel;
	float curMouseVel;
	monitorMouseVelocity* threadMouseVel;
public slots:

	void timerIdle();

signals:

protected:

    void initializeGL();
	void updateGL();
    void paintGL();
    void resizeGL( int w, int h );
    void mousePressEvent( QMouseEvent* e );
	void mouseReleaseEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
	void wheelEvent( QWheelEvent* e );
	void keyPressEvent( QKeyEvent* e );
	void keyReleaseEvent( QKeyEvent* e );
	void enterEvent( QEvent* e );
	void leaveEvent( QEvent* e );

};
class vector2d
{
public:
	int x, y;
	void set(int xx, int yy){ x = xx; y = yy; };
	float length(){ return sqrt(x*x + y*y); };
	void scale(float factor){ x = x*factor; y = y*factor; };

};


#endif


