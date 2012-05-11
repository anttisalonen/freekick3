#ifndef MATCHSDLGUI_H
#define MATCHSDLGUI_H

#include <memory>
#include <map>

#include <SDL.h>
#include <SDL_ttf.h>

#include "common/Texture.h"
#include "common/Color.h"

#include "match/MatchGUI.h"
#include "match/Clock.h"
#include "Vector3.h"
#include "match/PlayerController.h"
#include "match/PlayerActions.h"

struct LineCoord {
	LineCoord(float x_, float y_)
		: x(x_), y(y_) { }
	float x;
	float y;
};

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

struct FontConfig {
	inline FontConfig(const char* str, const Color& c, float scale);
	inline bool operator==(const FontConfig& f) const;
	inline bool operator<(const FontConfig& f) const;
	std::string mText;
	Color mColor;
	float mScale;
};

FontConfig::FontConfig(const char* str, const Color& c, float scale)
	: mText(str),
	mColor(c),
	mScale(scale)
{
}

bool FontConfig::operator==(const FontConfig& f) const
{
	return mText == f.mText && mColor == f.mColor && mScale == f.mScale;
}

bool FontConfig::operator<(const FontConfig& f) const
{
	if(mText != f.mText)
		return mText < f.mText;
	if(!(mColor == f.mColor))
		return mColor < f.mColor;
	return mScale < f.mScale;
}

struct TextTexture {
	inline TextTexture(std::shared_ptr<Common::Texture> t, unsigned int w, unsigned int h);
	std::shared_ptr<Common::Texture> mTexture;
	unsigned int mWidth;
	unsigned int mHeight;
};

TextTexture::TextTexture(std::shared_ptr<Common::Texture> t, unsigned int w, unsigned int h)
	: mTexture(t),
	mWidth(w),
	mHeight(h)
{
}

class MatchSDLGUI : public MatchGUI, public PlayerController {
	public:
		MatchSDLGUI(std::shared_ptr<Match> match, int argc, char** argv);
		~MatchSDLGUI();
		void play();
		std::shared_ptr<PlayerAction> act(double time);
	private:
		void drawEnvironment();
		void drawPlayers();
		void drawBall();
		void startFrame();
		void finishFrame();
		void loadTextures();
		void loadFont();
		bool handleInput(float frameTime);
		void handleInputState(float frameTime);
		void setPlayerController();
		static void drawSprite(const Common::Texture& t,
				const Rectangle& vertcoords,
				const Rectangle& texcoords, float depth);
		void drawText(float x, float y,
				const FontConfig& f,
				bool screencoordinates, bool centered);
		AbsVector3 getMousePositionOnPitch() const;
		bool progressMatch(double frameTime);
		void setupPitchLines();
		void drawPitchLines();
		void drawGoals();
		Clock mClock;
		SDL_Surface* mScreen;
		std::shared_ptr<Common::Texture> mPlayerTextureHome;
		std::shared_ptr<Common::Texture> mPlayerTextureAway;
		std::shared_ptr<Common::Texture> mPitchTexture;
		std::shared_ptr<Common::Texture> mBallTexture;
		std::shared_ptr<Common::Texture> mGoal1Texture;
		float mScaleLevel;
		float mScaleLevelVelocity;
		bool mFreeCamera;
		Vector3 mCamera;
		Vector3 mCameraVelocity;
		Vector3 mPlayerControlVelocity;
		double mPlayerKickPower;
		double mPlayerKickPowerVelocity;
		TTF_Font* mFont;
		std::map<FontConfig, std::shared_ptr<TextTexture>> mTextMap;
		bool mObserver;
		bool mMouseAim;
		Countdown mHalfTimeTimer;
		int mControlledPlayerIndex;
		std::vector<std::vector<LineCoord>> mPitchLines;
};

#endif


