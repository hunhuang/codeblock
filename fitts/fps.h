


#ifndef FPS_H
#define FPS_H


#include <time.h>
#include <algorithm>

enum 
{
	FPS_AVGNUM=32,						// Update FPS every FPS_AVGNUM frames
	FPS_GRAN=512						// Update FPS every FPS_GRAN millisecs (to avoid flickering)
};


class FPS
{
private:
	int times[FPS_AVGNUM];			// Array to store frame times
	int pos;									// Array index (Round Robin) to store current frame time
	float lastavg;							// Last average frame time
	int lastShowTime;					// Last clock time
	int lastClock;							// Last Clock Time

public:

	// Constructor
	FPS()
	{
		pos=0;
		lastavg=100;
		lastShowTime=0;
		lastClock=0;
		for(int i=0; i<FPS_AVGNUM; ++i)			times[i]=0;
	}

	// Destructor
	~FPS() {}

	// Add current frame time in millisecs
	void  add(DWORD ELAPSED_TIME)
	{
		if (ELAPSED_TIME<=0)		return;
		addFrameTime((int)(ELAPSED_TIME-lastClock));
		lastClock=int(ELAPSED_TIME);
	}

	// Get FPS
	float getFps() 
	{
		return 1000.0f/lastavg;
	}

private:
	
	// Get last average frame time
	float lastFrameTime() {return lastavg;}

	// Add current frame time in millisecs
	void  addFrameTime(int lastFrameTime)
	{
		times[pos%FPS_AVGNUM]=lastFrameTime;
		++pos;
		if(lastShowTime<clock()-FPS_GRAN) 
		{
			lastShowTime=clock();
			float sum=0;
			for(int i=0;i<FPS_AVGNUM;++i)		sum+=times[i];
			lastavg = float(sum)/float(std::min(int(FPS_AVGNUM),pos));					
		}
	}
};


#endif


