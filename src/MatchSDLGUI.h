#ifndef MATCHSDLGUI_H
#define MATCHSDLGUI_H

#include <SDL.h>

#include "MatchGUI.h"
#include "Texture.h"
#include "Clock.h"
#include "Vector3.h"

struct Rectangle {
	inline Rectangle(float x_, float y_, float w_, float h_);
	float x;
	float y;
	float w;
	float h;
};

Rectangle::Rectangle(float x_, float y_, float w_, float h_)
	: x(x_), y(y_), w(w_), h(h_)
{
}

class MatchSDLGUI : public MatchGUI {
	public:
		MatchSDLGUI(std::shared_ptr<Match> match);
		~MatchSDLGUI();
		void play();
	private:
		void drawEnvironment();
		void drawPlayers();
		void drawBall();
		void startFrame();
		void finishFrame();
		bool setupScreen();
		void loadTextures();
		bool handleInput(float frameTime);
		void handleInputState(float frameTime);
		static const char* GLErrorToString(GLenum err);
		static void drawSprite(const Texture& t,
				const Rectangle& vertcoords,
				const Rectangle& texcoords, float depth);
		Clock mClock;
		SDL_Surface* mScreen;
		std::shared_ptr<Texture> mPlayerTexture;
		std::shared_ptr<Texture> mPitchTexture;
		float mScaleLevel;
		float mScaleLevelVelocity;
		Vector3 mCamera;
		Vector3 mCameraVelocity;
};

#endif


