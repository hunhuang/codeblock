#include "threadReadingData.h"
void monitorMouseVelocity::run()
{
	POINT cursorPos, lastPos;
	GetCursorPos(&cursorPos);
	lastPos.x = cursorPos.x; lastPos.y = cursorPos.y;
	float velChange;
	const float time = 10;
	const float cons = 0.1;
	float tmpx, tmpy;
	QVector2D offsetDevice, offsetDisplay;
	float maxx = widget->width() / 2;
	float maxy = widget->height() / 2;
	float minx = -maxx; float miny = -maxy;
	float gain, velDevice, velDisplay;
	while (running)
	{
		GetCursorPos(&cursorPos);
		//if (widget->checkInsideWidget(QPoint(cursorPos.x, cursorPos.y))) continue;

		offsetDevice.setX(cursorPos.x - lastPos.x);
		offsetDevice.setY(-(cursorPos.y - lastPos.y));
		velDevice = (offsetDevice / time).length();
		gain = cons * velDevice* velDevice;
		offsetDisplay = gain * offsetDevice;
		velDisplay = (offsetDisplay / time).length();
		//printf("%d %d %d %d %d %d %4.4f \n", cursorPos.x, cursorPos.y, lastPos.x, lastPos.y, velDevice.x(), velDevice.y(), velDevice.length());

		//estimate next
		tmpx = widget->curMousex + offsetDisplay.x();
		tmpy = widget->curMousey + offsetDisplay.y();
		//check in bound
		widget->curMousex = tmpx > maxx ? maxx :
			(tmpx < minx ? minx : tmpx);
		widget->curMousey = tmpy> maxy ? maxy : (tmpy < miny ? miny : tmpy);

		/*int dx = widget->curMousex - lastx;
		int dy = widget->curMousey - lasty;*/
		//lastx = widget->curMousex; lasty = widget->curMousey;
		lastPos.x = cursorPos.x; lastPos.y = cursorPos.y; //copy

		widget->curDeviceVel = velDevice;
		widget->curMouseVel = velDisplay;
		//printf("%4.4f\n",widget->curMouseVel);
		//widget->repaint();
		msleep(time);

	}
};