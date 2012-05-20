#ifndef PITCH_H
#define PITCH_H

#define GOAL_WIDTH   7.32
#define GOAL_WIDTH_2 3.66

#define GOAL_HEIGHT  2.44

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

