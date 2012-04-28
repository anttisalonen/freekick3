#ifndef CLOCK_H
#define CLOCK_H

class Clock {
	public:
		Clock();
		double limitFPS(int fps);
	private:
		double getTime() const;
		double mLastTime;
		double mStatTime;
		int mFrames;
};

class Countdown {
	public:
		Countdown(float from);
		void doCountdown(float howmuch);
		bool checkAndRewind();
	private:
		float mFrom;
		float mNow;
};

#endif
