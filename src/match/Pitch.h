#ifndef PITCH_H
#define PITCH_H

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

