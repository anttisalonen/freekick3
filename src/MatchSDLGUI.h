#ifndef MATCHSDLGUI_H
#define MATCHSDLGUI_H

#include <SDL.h>

#include "MatchGUI.h"
#include "Texture.h"
#include "Clock.h"

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
		bool handleInput();
		static const char* GLErrorToString(GLenum err);
		Clock mClock;
		SDL_Surface* mScreen;
		std::shared_ptr<Texture> mPlayerTexture;
};

#endif


