#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <algorithm>

#include "Clock.h"

Clock::Clock()
	: mFrames(0)
{
	mLastTime = getTime();
	mStatTime = mLastTime;
}

double Clock::limitFPS(int fps)
{
	double newtime = getTime();
	double maxadv = 1.0f / fps;
	double diff = newtime - mLastTime;
	if(maxadv > diff) {
		usleep((maxadv - diff) * 1000000);
		mLastTime = getTime();
	}
	else {
		mLastTime = newtime;
	}
	mFrames++;
	if(newtime - mStatTime >= 2.0f) {
		std::cout << "FPS: " << mFrames / (newtime - mStatTime) << "\n";
		mStatTime = newtime;
		mFrames = 0;
	}
	return std::min(diff, maxadv);
}

double Clock::getTime() const
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)(tv.tv_usec / 1000000.0f);
}

Countdown::Countdown(float from)
	: mFrom(from),
	mNow(0)
{
}

void Countdown::doCountdown(float howmuch)
{
	if(mNow > 0.0f)
		mNow -= howmuch;
}

bool Countdown::checkAndRewind()
{
	if(mNow <= 0.0f) {
		mNow = mFrom;
		return true;
	}
	else {
		return false;
	}
}


