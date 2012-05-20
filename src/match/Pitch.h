#ifndef PITCH_H
#define PITCH_H

#define GOAL_WIDTH   7.32
#define GOAL_WIDTH_2 3.66

#define GOAL_HEIGHT  2.44

#define GOAL_POST_RADIUS 0.4
#define GOAL_NET_RADIUS  0.2

#define BALL_RADIUS 0.3

class Pitch {
	public:
		Pitch(float width, float height);
		float getWidth() const;
		float getHeight() const;
	private:
		float mWidth;
		float mHeight;
};

#endif

