#include <qtcore/qevent>
#include <qtcore/qbytearray>
#include <qtcore/qstringlist>

#include <qtgui/qfontdatabase>
#include <qtgui/qmouseevent>
#include <qtgui/qcursor>

#include <set>
#include <ctime>
#include <cassert>

#include "glwidget.h"
#include "threadReadingData.h"

#include <glut.h>

// ...
void GLWidget::initFont()
{
	int fontId = QFontDatabase::addApplicationFont("data/font/open_sans_condensed_light.ttf");
	QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
	font = QFont(fontFamily, 10, QFont::Normal);

}


// Draw 2D circle.
void GLWidget::drawCircle(float x, float y, float radius, int slices)
{
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (int n = 0; n <= slices; ++n) {
		float const t = 2.0f * M_PI * float(n) / float(slices);
		glVertex2f(x + std::sin(t) * radius, y + std::cos(t) * radius);
	}
	glEnd();
}


// Draw Fitts' task 2D.
void GLWidget::drawFittsTask2D(int number, int size, float radius, bool selection, bool debug)
{
	// Draw radius and circle size 2D lines.
	if (!selection && debug) {
		qglColor(fontColorA);
		glBegin(GL_LINES);
		// Radius.
		glVertex2f(0.0f, 0.0f);
		glVertex2f(0.0f, radius);
		// Size.
		glVertex2f(0.0f, radius);
		glVertex2f(size, radius);
		glEnd();
	}


	for (int i = 1; i <= number; ++i) {
		float const t = 2.0f * float(M_PI) * float(i) / float(number);
		float cx = std::sin(t) * fittsRadius;
		float cy = std::cos(t) * fittsRadius;
		if (selection) { glColor3f(float(i) / 255.0f, 1.0f, 1.0f); }
		else if (fittsIdx2Pos(slcIdx) == i) { qglColor(fittsColorSelected); } //draw the one selected
		else if (fittsIdx2Pos(fittsMarkedIdx) == i) { qglColor(fittsColorMarked); } //draw the one that is marked
		else { qglColor(fittsColorNormal); }
		drawCircle(cx, cy, size, 36);
	}
	// If no selection, draw debug info.
	if (!selection && debug) {
		qglColor(fontColorA);
		// Draw radius and size lines labels.
		renderText((width() / 2) + 10, height() / 2, "RADIUS", font);
		renderText((width() / 2) + (fittsSize * width() / 2) + 10, (height() / 2) - (int(radius * height()) / 2), "SIZE", font);
		// Draw 2D Fitts' task circle indices labels.
		for (int i = 1; i <= number; ++i) {
			float const t = 2.0f * float(M_PI) * float(i) / float(number);
			float cx = std::sin(t) * radius;
			float cy = std::cos(t) * radius;
			unsigned int px = (width() / 2) + (int(cx * width() / (2 * wndAR))) - 5;//7;
			unsigned int py = (height() / 2) - (int(cy * height()) / 2) + 5;//7;
			renderText(px, py, QString("%1").arg(fittsPos2Idx(i)), font);
		}
	}
}


// Display text on screen.
void GLWidget::drawText()
{
	qglColor(fontColorA);
	// FPS info.
	renderText(width() - 70, 30, QString("%1").arg(fps.getFps(), 0, 'f', 1), font);
	renderText(width() - 40, 30, QString("FPS"), font);
	// ...
	if (fittsStatus == TestState::READY) { qglColor(fontColorA); }
	else { qglColor(fontColorB); }
	// Selection info.
	renderText(20, 30, "SELECTED CIRCLE", font);
	//if( fittsColor2Idx[slcIdx] != 0 ) {
	if (slcIdx != 0) {
		renderText(20, 50, QString("Mouse pos XY:   %1, %2").arg(slcClick.x()).arg(slcClick.y()), font);
		renderText(20, 70, QString("Pixel OpenGL color RGBA:   %1, %2, %3, %4").arg(slcColor[0], 0, 'f', 3).arg(slcColor[1], 0, 'f', 3).arg(slcColor[2], 0, 'f', 3).arg(slcColor[3], 0, 'f', 3), font);
		//renderText( 20, 90, QString( "Circle index:   %1" ).arg( fittsColor2Idx[slcIdx] ), font );
		renderText(20, 90, QString("Circle index:   %1").arg(slcIdx), font);
		renderText(20, 110, QString("Circle center:   %1, %2").arg(slcCenter.x()).arg(slcCenter.y()), font);
	}
	else {
		renderText(20, 50, QString("Mouse pos XY:"), font);
		renderText(20, 70, QString("Pixel color RGBA:"), font);
		renderText(20, 90, QString("Circle index:"), font);
		renderText(20, 110, QString("Circle center:"), font);
	}
	renderText(20, 130, QString("Circle radius:   %1").arg((float)fittsSize, 0, 'f', 3), font);
	// Debug info.
	renderText(20, 200, "DEBUG MODE", font);
	if (dbgFlag) { renderText(20, 220, "Debug: ON", font); }
	else { renderText(20, 220, "Debug: OFF", font); }
	// Interface info.
	renderText(20, 300, "INTERFACE", font);
	renderText(20, 320, "D", font); renderText(50, 320, "Toggle debug info", font);
	renderText(20, 340, "F", font); renderText(50, 340, "Toggle fullscreen/windowed", font);
	renderText(20, 360, "R", font); renderText(50, 360, "Reset Fitts' 2D", font);
	renderText(20, 380, "=/-", font); renderText(50, 380, "Increase/decrease circle size", font);
	renderText(20, 400, "PgU", font); renderText(50, 400, "Increase circle number", font);
	renderText(20, 420, "PgD", font); renderText(50, 420, "Decrease circle number", font);
	renderText(20, 440, "Up", font); renderText(50, 440, "Move mouse cursor up", font);
	renderText(20, 460, "Right", font); renderText(50, 460, "Move mouse cursor right", font);
	renderText(20, 480, "Down", font); renderText(50, 480, "Move mouse cursor down", font);
	renderText(20, 500, "Left", font); renderText(50, 500, "Move mouse cursor left", font);
	renderText(20, 520, "X", font); renderText(50, 520, "Click mouse left button", font);
	renderText(20, 540, "G", font); renderText(50, 540, "Grab screenshot", font);
}


// ...
void GLWidget::grabWidgetScreenshot() {
	if (grabScreenshot) {
		QImage img;
		img = grabFrameBuffer(true);
		if (!img.isNull()) { bool res = img.save("data/screenshot.bmp", "BMP", -1); }
		grabScreenshot = false;
	}
}


// ...
unsigned int GLWidget::fittsIdx2Pos(unsigned int i)
{
	assert((i >= 0) && (i <= fittsNum));
	// NOTE: unsigned int expressions!
	if ((i % 2) == 0) { return i / 2; }
	else { return (i / 2) + 1 + (fittsNum / 2); }
}


// ...
unsigned int GLWidget::fittsPos2Idx(unsigned int p)
{
	assert((p >= 0) && (p <= fittsNum));
	// NOTE: unsigned int expressions!
	if (p <= (fittsNum / 2)) { return p * 2; }
	else { return ((p - (fittsNum / 2) - 1) * 2) + 1; }
}


// ...
GLWidget::GLWidget(QWidget* parent) : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::Rgba | QGL::DepthBuffer), parent)
{
	makeCurrent();
	initFont();
	resetSelection();

}




// ...
GLWidget::~GLWidget() { makeCurrent();

	threadMouseVel->running = false;
	Sleep(500);
}


// ...
void GLWidget::exitWidget() {}


// ...
void GLWidget::toggleDebugInfo() { dbgFlag = !dbgFlag; }


// Reset selection.
void GLWidget::resetSelection()
{
	slcClick = QPoint(0, 0);
	slcColor[0] = 0.0f;
	slcColor[1] = 0.0f;
	slcColor[2] = 0.0f;
	slcColor[3] = 0.0f;
	slcIdx = 0;
	slcCenter = QPoint(0, 0);
}


//  Reset click info.
void GLWidget::resetClick()
{
	clickGLPos = QPoint(0, 0);
	clickFlag = false;
}


// ...
void GLWidget::timerIdle() { repaint(); }


// ...
void GLWidget::initializeGL()
{
	//glutInit( &argc, argv ); // No GLUT initialization needed.

	// OpenGL initialization.
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0f);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}


// ...
void GLWidget::updateGL() { repaint(); }


// ...
void GLWidget::paintGL()
{
	// Update FPS counter.
	fps.add(clock());
	// Clearing the color and depth buffers.
	glClearColor(bgColor.red() / 255.0f, bgColor.green() / 255.0f, bgColor.blue() / 255.0f, bgColor.alpha() / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Viewport setup.
	glViewport(0, 0, (GLsizei)width(), (GLsizei)height());
	// 2D orthographic projection matrix setup.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluOrtho2D(-width()/2, width()/2,-height()/2, height()/2);
	// Modelview matrix setup.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslated(curMousex, curMousey, 0);
	glBegin(GL_LINES);
	{
		glVertex2d(-cursorSize, 0);
		glVertex2d(cursorSize, 0);
		glVertex2d(0, -cursorSize);
		glVertex2d(0, cursorSize);

	}
	glEnd();
	glPopMatrix();
	glColor3f(1, 1, 1);
	renderText(width() - 300, 30, QString("Cur device velocity"), font);
	renderText(width() - 50, 30, QString::number(curDeviceVel, 'f', 4), font);

	renderText(width() - 300, 20, QString("Cur display velocity"), font);
	renderText(width() - 50, 20, QString::number( curMouseVel,'f',4), font);

	renderText(width() - 300, 40, QString("Cur display position"), font);
	renderText(width() - 50, 40, QString("%1 %2").arg( curMousex, curMousey), font);

	// Draw 2D Fitts' task circles.
	drawFittsTask2D(fittsNum, fittsSize, fittsRadius, false, dbgFlag);

	// Mark last mouse click position on the screen coloring a single pixel.

	// In debug mode draw text info on the screen.
	if (dbgFlag) { drawText(); }
	// Double buffering active by default!
	grabWidgetScreenshot();
}


// ...
void GLWidget::resizeGL(int w, int h)
{
	////recalculate fittssize;
	////This is the first trial, try to resize the fittsSize according to the w and h,
	////however, because the drawCircle takes on radius on the x axis,
	////If we can draw an ecclipse. ==>might help
	//if (intCurrentWidgetHeight == 0) intCurrentWidgetHeight = h;
	//fittsSize = intCurrentWidgetHeight * fittsSize / h;
	wndAR = float(w) / float(h);
	//printf("%4.4f\n", fittsSize);
	/*intCurrentWidgetHeight = h;
	intCurrentWidgetWidth = w;*/
	repaint();
}

bool GLWidget::checkInsideWidget(QPoint mouGLPos)
{
	if ((mouGLPos.x() < 0) || (mouGLPos.y() < 0) || (mouGLPos.x() > width()) || (mouGLPos.y() > height())) { return false; }
	return true;
}

// ...
void GLWidget::mousePressEvent(QMouseEvent* e)
{
	// Check mouse button pressed: right and middle button not active.
	switch (e->button()) {
	case Qt::RightButton: { return; }
	case Qt::MiddleButton: { return; }
	default: { break; }
	}
	// Store window mouse position and convert it into OpenGL screen coordinates.
	mouWinPos = e->pos();
	mouGLPos.setX(e->pos().x());
	mouGLPos.setY(height() - e->pos().y());
	// Safety check: mouse OpenGL coordinates inside OpenGL context!
	printf("%d %d\n", mouGLPos.x(), mouGLPos.y());
	if (!checkInsideWidget(mouGLPos)) return;
	// Update click info.
	clickGLPos = mouGLPos;
	clickFlag = true;
	// Setup for the selection implementation using the back-buffer!
	// Blending disabled to ensure color consistency.
	glDisable(GL_BLEND);
	// Clearing the color and depth buffers.
	// NOTE: We don't need to call glDrawBuffer( GL_BACK ) since the back buffer is the default writing buffer in doublebuffering mode!
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Draw 2D Fitts' task circles: in selection mode each circle is rendered with a specific color to aid its identification.
	drawFittsTask2D(fittsNum, fittsSize, fittsRadius, true, false);
	// Set the back buffer for reading the color of the pixel located at mouse coordinates (OpenGL screen coordinate).
	GLfloat pixelRGBA[4];
	glReadBuffer(GL_BACK);
	glReadPixels(mouGLPos.x(), mouGLPos.y(), 1, 1, GL_RGBA, GL_FLOAT, pixelRGBA);
	// Convert the color into an pos (the 2D Fitts' task circle pos).
	unsigned int p = (unsigned int)(pixelRGBA[0] * 255.0f);
	// If the pos is different than 0: a circle has been selected, and we store selection info.
	if (p != 0) {
		slcIdx = fittsPos2Idx(p);
		// Store selection data.
		slcClick = mouGLPos;
		slcColor[0] = pixelRGBA[0];
		slcColor[1] = pixelRGBA[1];
		slcColor[2] = pixelRGBA[2];
		slcColor[3] = pixelRGBA[3];
		// Compute selected circle center.
		float const t = 2.0f * float(M_PI) * float(slcIdx) / float(fittsNum);
		float cx = std::sin(t) * fittsRadius;
		float cy = std::cos(t) * fittsRadius;
		unsigned int px = (cx);
		unsigned int py = (cy);
		slcCenter.setX(px);
		slcCenter.setY(py);
	}
	// If the index equals 0: no circle has been selected, and we reset the previous selection info.
	else { resetSelection(); }
	// Fitts' task 2D game logic.
	switch (fittsStatus) {
		// Start.
	case TestState::READY: {
							   if (slcIdx == fittsMarkedIdx) {
								   fittsStatus = TestState((unsigned int)fittsStatus + 1);
								   fittsMarkedIdx = (unsigned int)fittsStatus;
							   }
							   break; }
		// End.as
	case TestState::END: {
							 fittsStatus = TestState::READY;
							 fittsMarkedIdx = (fittsNum % 2 == 0) ? (fittsNum - 1) : fittsNum;
							 break; }
		// Game states during task completion.
	default: {
				 if (slcIdx == fittsMarkedIdx) {
					 fittsStatus = TestState((unsigned int)fittsStatus + 1);
					 if ((unsigned int)fittsStatus > fittsNum) { fittsStatus = TestState::END; }
					 else { fittsMarkedIdx = (unsigned int)fittsStatus; }
					 break;
				 }
	}
	}
	// Repaints the widget immediately.
	repaint();
}


// ...
void GLWidget::mouseReleaseEvent(QMouseEvent* e) {
	repaint(); }


// ...
void GLWidget::mouseMoveEvent(QMouseEvent* e) {

	repaint();
}


// ...
void GLWidget::wheelEvent(QWheelEvent* e) { repaint(); }


// ...
void GLWidget::keyPressEvent(QKeyEvent* e)
{
	// ...
	if (fittsStatus == TestState::READY) {
		// ...
		if (!flagGrabKeyboard) { return; }
		switch (e->key()) {
			// Toggle debug info.
		case Qt::Key_D: {
							threadMouseVel= new monitorMouseVelocity(this);
							threadMouseVel->start();

							dbgFlag = !dbgFlag; break; }
			// Toggle fullscreen.
			//case Qt::Key_F : {
			//	if(isFullScreen()) { showNormal(); }
			//	else { showFullScreen(); }
			//	//setWindowState( windowState() ^ Qt::WindowFullScreen );
			//	break; }
			// Grab screenshot.
		case Qt::Key_G: { grabScreenshot = true; break; }
			// Reset Fitts' 2D.
		case Qt::Key_R: {
							resetSelection();
							fittsNum = fittsNumReset;
							fittsSize = fittsSizeReset;
							fittsRadius = fittsRadiusReset;
							break; }
			// Increase circle size.
		case Qt::Key_Equal: {
								resetSelection();
								fittsSize += fittsIncrease;
								if (fittsSize > fittsSizeMax) { fittsSize = fittsSizeMax; }
								break; }
			// Decrease circle size.
		case Qt::Key_Minus: {
								resetSelection();
								fittsSize -= fittsIncrease;
								if (fittsSize < fittsSizeMin) { fittsSize = fittsSizeMin; }
								break; }
			// Move mouse cursor down.
		case Qt::Key_Down: {
							   POINT cursorPos;
							   GetCursorPos(&cursorPos);
							   SetCursorPos(cursorPos.x, cursorPos.y + 1);
							   curMousey--;
							   break; }
			// Move mouse cursor left.
		case Qt::Key_Left: {
							   POINT cursorPos;
							   GetCursorPos(&cursorPos);
							   SetCursorPos(cursorPos.x - 1, cursorPos.y);

							   curMousex--;


							   break; }
			// Move mouse cursor right.
		case Qt::Key_Right: {
								POINT cursorPos;
								GetCursorPos(&cursorPos);
								SetCursorPos(cursorPos.x + 1, cursorPos.y);
								curMousex++;
								break; }
			// Move mouse cursor up.
		case Qt::Key_Up: {
							 POINT cursorPos;
							 GetCursorPos(&cursorPos);
							 SetCursorPos(cursorPos.x, cursorPos.y - 1);
							 curMousey++;
							 break; }
			// Increase circle number.
		case Qt::Key_PageUp: {
								 resetSelection();
								 fittsNum++;
								 if (fittsNum > fittsNumMax) { fittsNum = fittsNumMax; }
								 fittsMarkedIdx = (fittsNum % 2 == 0) ? (fittsNum - 1) : fittsNum;

								 break; }
			// Decrease circle number.
		case Qt::Key_PageDown: {
								   resetSelection();
								   fittsNum--;
								   if (fittsNum < fittsNumMin) { fittsNum = fittsNumMin; }
								   fittsMarkedIdx = (fittsNum % 2 == 0) ? (fittsNum - 1) : fittsNum;

								   break; }
			// Click mouse left button.
		case Qt::Key_X: {
							POINT cursorPos;
							GetCursorPos(&cursorPos);
							QMouseEvent* me = new QMouseEvent(QEvent::MouseButtonPress, mapFromGlobal(QPoint(cursorPos.x, cursorPos.y)), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
							mousePressEvent(me);
							break; }
		default: { break; }
		}
	}
	repaint();
}


// ...
void GLWidget::keyReleaseEvent(QKeyEvent* e) {}


// ....
void GLWidget::enterEvent(QEvent * event) { enableTracking(); }


// ...
void GLWidget::leaveEvent(QEvent* e) { disableTracking(); }


// ...
void GLWidget::enableTracking()
{
	// If current cursor is not the click cursor, update mouse cursor (PUSH on the cursor stack).
	if (cursor().shape() != cursorClick.shape()) { setCursor(cursorClick); }
	// Activate mouse tracking even while not clicking.
	setMouseTracking(true);
	// Activate keyboard tracking.
	flagGrabKeyboard = true;
	grabKeyboard();
}


// ...
void GLWidget::disableTracking()
{
	// If current cursor is the click cursor, update mouse cursor (POP on the cursor stack).
	if (cursor().shape() == cursorClick.shape()) { unsetCursor(); }
	// Deactivate mouse tracking even while not clicking.
	setMouseTracking(false);
	// Deactivate keyboard tracking.
	flagGrabKeyboard = false;
	releaseKeyboard(); // Using a private flag since this call seems not working!
}


