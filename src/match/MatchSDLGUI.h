#ifndef MATCHSDLGUI_H
#define MATCHSDLGUI_H

#include <boost/shared_ptr.hpp>
#include <map>

#include <SDL.h>
#include <SDL_ttf.h>

#include "common/Texture.h"
#include "common/Color.h"
#include "common/Rectangle.h"
#include "common/Vector3.h"
#include "common/FontConfig.h"

#include "match/MatchGUI.h"
#include "match/Clock.h"
#include "match/PlayerController.h"
#include "match/PlayerActions.h"

struct LineCoord {
	LineCoord(float x_, float y_)
		: x(x_), y(y_) { }
	float x;
	float y;
};

class MatchSDLGUI : public MatchGUI, public PlayerController {
	public:
		MatchSDLGUI(boost::shared_ptr<Match> match, bool observer, int teamnum, int playernum,
				int ticksPerSec, bool debug, bool randomise,
				bool disablegui);
		~MatchSDLGUI();
		bool play();
		boost::shared_ptr<PlayerAction> act(double time);
	private:
		void drawEnvironment();
		void drawTexts();
		void drawPlayers();
		void drawBall();
		void startFrame();
		void finishFrame();
		void loadTextures();
		void loadFont();
		bool handleInput(float frameTime);
		void handleInputState(float frameTime);
		void setPlayerController(double frameTime);
		static void drawSprite(const Common::Texture& t,
				const Common::Rectangle& vertcoords,
				const Common::Rectangle& texcoords, float depth);
		void drawText(float x, float y,
				const Common::FontConfig& f,
				bool screencoordinates, bool centered);
		AbsVector3 getMousePositionOnPitch() const;
		void setupPitchLines();
		void drawPitchLines();
		void drawGoals();
		const boost::shared_ptr<Common::Texture> playerTexture(const Player* p);
		std::pair<const Soccer::Kit, const Soccer::Kit> getKits() const;
		static Common::Color mapKitColor(const Soccer::Kit& kit, const Common::Color& c);
		bool kitConflict(const Soccer::Kit& kit0, const Soccer::Kit& kit1) const;
		Clock mClock;
		SDL_Surface* mScreen;
		boost::shared_ptr<Common::Texture> mPlayerTextureHome[12];
		boost::shared_ptr<Common::Texture> mPlayerTextureAway[12];
		boost::shared_ptr<Common::Texture> mPlayerShadowTexture;
		boost::shared_ptr<Common::Texture> mPitchTexture;
		boost::shared_ptr<Common::Texture> mBallTexture;
		boost::shared_ptr<Common::Texture> mBallShadowTexture;
		boost::shared_ptr<Common::Texture> mGoal1Texture;
		float mScaleLevel;
		float mScaleLevelVelocity;
		bool mFreeCamera;
		Common::Vector3 mCamera;
		Common::Vector3 mCameraVelocity;
		Common::Vector3 mPlayerControlVelocity;
		double mPlayerKickPower;
		double mPlayerKickPowerVelocity;
		TTF_Font* mFont;
		std::map<Common::FontConfig, boost::shared_ptr<Common::TextTexture>> mTextMap;
		bool mObserver;
		bool mMouseAim;
		int mControlledPlayerIndex;
		int mControlledTeamIndex;
		std::vector<std::vector<LineCoord>> mPitchLines;
		Countdown mPlayerSwitchTimer;
		bool mPaused;
		int mDebugDisplay;
		float mFixedFrameTime;
		bool mTackling;
		bool mHeading;
		bool mRandomise;
		bool mDisableGUI;
		bool mCamFollowsPlayer;
};

#endif


