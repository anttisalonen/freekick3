#include <assert.h>
#include <string.h>

#include <stdexcept>
#include <sstream>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/SDL_utils.h"
#include "common/Math.h"

#include "match/MatchSDLGUI.h"
#include "match/MatchHelpers.h"
#include "match/ai/PlayerAIController.h"
#include "match/ai/AIHelpers.h"

using namespace Common;

static const int screenWidth = 800;
static const int screenHeight = 600;

static const float pitchHeight = 0.0f;
static const float pitchLineHeight = 0.1f;
static const float goalHeight = 0.2f;
static const float ballShadowHeight = 0.85f;
static const float ballHeight = 0.9f;
static const float playerShadowHeight = 0.95f;
static const float playerHeight = 1.0f;
static const float textHeight = 5.0f;

MatchSDLGUI::MatchSDLGUI(boost::shared_ptr<Match> match, bool observer, int teamnum, int playernum,
		int ticksPerSec, bool debug, bool randomise, bool disablegui)
	: MatchGUI(match),
	PlayerController(mMatch->getPlayer(0, 9)),
	mScaleLevel(11.5f),
	mScaleLevelVelocity(0.0f),
	mFreeCamera(false),
	mPlayerKickPower(0.0f),
	mPlayerKickPowerVelocity(0.0f),
	mFont(nullptr),
	mObserver(observer),
	mMouseAim(false),
	mControlledPlayerIndex(playernum - 1),
	mControlledTeamIndex(teamnum - 1),
	mPlayerSwitchTimer(0.2f),
	mPaused(false),
	mDebugDisplay(0),
	mFixedFrameTime(0.0f),
	mTackling(false),
	mHeading(false),
	mRandomise(randomise),
	mDisableGUI(disablegui),
	mCamFollowsPlayer(true)
{
	if(ticksPerSec) {
		mFixedFrameTime = 1.0f / ticksPerSec;
	}
	if(mDisableGUI) {
		return;
	}

	mScreen = SDL_utils::initSDL(screenWidth, screenHeight, "Freekick 3 match");

	loadTextures();
	loadFont();

	SDL_utils::setupOrthoScreen(screenWidth, screenHeight);
	setupPitchLines();

	setPlayer(mMatch->getPlayer(mControlledTeamIndex, mControlledPlayerIndex == -1 ?
				9 : mControlledPlayerIndex));
	if(!mObserver) {
		setPlayerController(0.0f);
	}

	if(debug) {
		if(mObserver) {
			mFreeCamera = true;
		}
		mDebugDisplay = 2;
	}

}

MatchSDLGUI::~MatchSDLGUI()
{
	if(mFont)
		TTF_CloseFont(mFont);
	TTF_Quit();
	SDL_Quit();
}

void MatchSDLGUI::loadFont()
{
	mFont = TTF_OpenFont("share/DejaVuSans.ttf", 12);
	if(!mFont) {
		fprintf(stderr, "Could not open font: %s\n", TTF_GetError());
		throw std::runtime_error("Loading font");
	}
}

bool MatchSDLGUI::play()
{
	double prevTime = Clock::getTime();
	while(1) {
		double newTime = Clock::getTime();
		double frameTime = mFixedFrameTime ? mFixedFrameTime : newTime - prevTime;
		if(!mPaused && mFixedFrameTime && mRandomise) {
			double add = rand() / (double)RAND_MAX;
			add -= 0.5f;
			add *= 0.01f * mFixedFrameTime;
			frameTime += add;
		} else {
			frameTime *= 1.3f;
		}
		prevTime = newTime;

		if(!mPaused) {
			mMatch->update(frameTime);
			if(!mDisableGUI && !mObserver)
				setPlayerController(frameTime);
		}

		if(!mDisableGUI && handleInput(frameTime))
			break;
		if(!mDisableGUI) {
			startFrame();
			drawEnvironment();
			drawBall();
			drawPlayers();
			drawTexts();
			finishFrame();
		}

		if(!mPaused) {
			if(!progressMatch(frameTime))
				break;
		}
	}
	if(mMatch->matchOver()) {
		Soccer::MatchResult mres(mMatch->getScore(1), mMatch->getScore(0),
				mMatch->getPenaltyShootout().getScore(true),
				mMatch->getPenaltyShootout().getScore(false));
		mMatch->setResult(mres);
		return true;
	}
	return false;
}

void MatchSDLGUI::drawEnvironment()
{
	float pwidth = mMatch->getPitchWidth();
	float pheight = mMatch->getPitchHeight();
	drawSprite(*mPitchTexture, Rectangle((-mCamera.x - pwidth) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y - pheight) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * pwidth * 4.0f,
				mScaleLevel * pheight * 4.0f),
			Rectangle(0, 0, 22, 22), pitchHeight);
	drawPitchLines();
	drawGoals();

	if(mDebugDisplay > 1) {
		const Team* t = mMatch->getTeam(mDebugDisplay == 2 ? 0 : 1);
		for(int j = -pheight * 0.5f + 8; j < pheight * 0.5 - 8; j += 8) {
			for(int i = -pwidth * 0.5f + 8; i < pwidth * 0.5 - 8; i += 8) {
				float score1 = 2.0f * t->getShotScoreAt(Vector3(i, j, 0));
				float score2 = 2.0f * t->getPassScoreAt(Vector3(i, j, 0));
				glDisable(GL_TEXTURE_2D);
				glPointSize(10.0f);
				glBegin(GL_POINTS);
				glColor3f(std::min(1.0f, score1), 0.0f, 0.0f);
				glVertex3f((-mCamera.x + i) * mScaleLevel + screenWidth * 0.5f - 5.0f,
						(-mCamera.y + j) * mScaleLevel + screenHeight * 0.5f,
						textHeight);
				glColor3f(0.0f, 0.0f, std::min(1.0f, score2));
				glVertex3f((-mCamera.x + i) * mScaleLevel + screenWidth * 0.5f + 5.0f,
						(-mCamera.y + j) * mScaleLevel + screenHeight * 0.5f,
						textHeight);
				glEnd();
				glColor3f(1.0f, 1.0f, 1.0f);
				glEnable(GL_TEXTURE_2D);
			}
		}
	}
}

void MatchSDLGUI::drawTexts()
{
	bool penaltyshootout = mMatch->getPenaltyShootout().getScore(true) ||
		mMatch->getPenaltyShootout().getScore(false) ||
		mMatch->getMatchHalf() == MatchHalf::PenaltyShootout;

	std::stringstream result;
	result << mMatch->getTeam(0)->getName() << " " << mMatch->getScore(true) <<
		" - " << mMatch->getScore(false) << " " << mMatch->getTeam(1)->getName();
	if(penaltyshootout) {
		result << " (" << mMatch->getPenaltyShootout().getScore(true) <<
			" - " << mMatch->getPenaltyShootout().getScore(false) << ")";
	} else {
		if(mMatch->getAwayGoals()) {
			result << " (" << mMatch->getAggregateScore(true) <<
				" - " << mMatch->getAggregateScore(false) << ")";
		}
	}

	drawText(90, screenHeight - 30, FontConfig(result.str().c_str(), Color(255, 255, 255), 1.5f), true, false);

	if(mMatch->getMatchHalf() != MatchHalf::Finished && mMatch->getMatchHalf() != MatchHalf::PenaltyShootout) {
		char timebuf[128];
		int min = int(mMatch->getTime());

		if(mMatch->getMatchHalf() >= MatchHalf::HalfTimePauseBegin) {
			min += 45;
			if(mMatch->getMatchHalf() >= MatchHalf::FullTimePauseBegin) {
				min += 45;
				if(mMatch->getMatchHalf() >= MatchHalf::ExtraTimeSecondHalf) {
					min += 15;
				}
			}
		}
		sprintf(timebuf, "%d min.", min);
		drawText(10, screenHeight - 30, FontConfig(timebuf, Color(255, 255, 255), 1.5f), true, false);
	}

	{
		const Player* playerincontrol = mMatch->getReferee()->getPlayerInControl();
		if(playerincontrol) {
			std::string plname = Soccer::Player::getShorterName(*playerincontrol);
			char plbuf[128];
			snprintf(plbuf, 127, "%d %s", playerincontrol->getShirtNumber(),
					plname.c_str());
			plbuf[127] = '\0';

			drawText(10, screenHeight - 50, FontConfig(plbuf, Color(255, 255, 255), 1.0f), true, false);
		}
	}

	if(mPaused) {
		drawText(screenWidth / 2, screenHeight / 2, FontConfig("Paused", Color(255, 255, 255), 2.0f),
				true, true);
	}
	else if((mMatch->getPlayState() == PlayState::OutKickoff &&
				playing(mMatch->getMatchHalf()) &&
					(mMatch->getScore(true) + mMatch->getScore(false) != 0)) ||
		(!playing(mMatch->getMatchHalf()) && mMatch->getMatchHalf() != MatchHalf::NotStarted)) {
		std::vector<std::string> scorers[2];
		unsigned int i = 0;
		for(auto side : mMatch->getGoalInfos()) {
			assert(i < 2);
			std::stringstream ss;
			if(i == 0) {
				ss << mMatch->getTeam(i)->getName() << "   " << mMatch->getScore(i == 0);
				if(penaltyshootout) {
					ss << " (" << mMatch->getPenaltyShootout().getScore(i == 0) << ")";
				}
			} else {
				if(penaltyshootout) {
					ss << "(" << mMatch->getPenaltyShootout().getScore(i == 0) << ") ";
				}
				ss << mMatch->getScore(i == 0) << "   " << mMatch->getTeam(i)->getName();
			}
			scorers[i].push_back(ss.str());
			for(auto goalinfo : side) {
				scorers[i].push_back(goalinfo.getShortScorerName() + std::string(" ") + goalinfo.getScoreTime());
			}
			i++;
		}

		int x = screenWidth * 0.35f;
		for(auto slist : scorers) {
			int y = screenHeight * 0.5f;
			for(auto s : slist) {
				drawText(x, y, FontConfig(s.c_str(), Color(255, 255, 255), 2.0f),
						true, true);
				y -= 40;
			}
			x = screenWidth * 0.7f;
		}
	}
}

const boost::shared_ptr<Texture> MatchSDLGUI::playerTexture(const Player* p)
{
	/* Mapping:
	 * 0  => Stand, direction north
	 * 1  => Stand, direction west
	 * 2  => Stand, direction south
	 * 3  => Stand, direction east
	 * 4  => Fallen, direction north (unused)
	 * 5  => Fallen, direction west
	 * 6  => Fallen, direction south (unused)
	 * 7  => Fallen, direction east (unused)
	 * 8  => Tackling, direction north
	 * 9  => Tackling, direction west
	 * 10 => Tackling, direction south
	 * 11 => Tackling, direction east
	 * 12 => Run, direction north, frame 1
	 * 13 => Run, direction north, frame 2
	 * 14 => Run, direction west, frame 1
	 * 15 => Run, direction west, frame 2
	 * 16 => Run, direction south, frame 1
	 * 17 => Run, direction south, frame 2
	 * 18 => Run, direction east, frame 1
	 * 19 => Run, direction east, frame 2
	 * 20 => Diving, facing north, diving west, frame 1
	 * 21 => Diving, facing north, diving west, frame 2
	 * 22 => Diving, facing north, diving east, frame 1
	 * 23 => Diving, facing north, diving east, frame 2
	 * 24 => Diving, facing south, diving west, frame 1
	 * 25 => Diving, facing south, diving west, frame 2
	 * 26 => Diving, facing south, diving east, frame 1
	 * 27 => Diving, facing south, diving east, frame 2
	 * The textures are mirrored on the y-axis.
	 * The second diving frames are not used (yet).
	 */
	int index = 0;
	Vector3 vec = p->getVelocity();
	if(p->isGoalkeeper() && p->getPosition().z > 0.1f &&
			(fabs(vec.x) > 0.1f || fabs(vec.y) > 0.1f)) {
		// diving
		auto vecToBall = MatchEntity::vectorFromTo(*p, *mMatch->getBall());
		if(vecToBall.y > 0.0f) {
			// facing north
			if(vec.x > 0.0f) {
				// diving east
				index = 20;
			} else {
				index = 22;
			}
		} else {
			if(vec.x > 0.0f) {
				index = 24;
			} else {
				index = 26;
			}
		}
	} else {
		if(vec.null()) {
			vec = MatchEntity::vectorFromTo(*p, *mMatch->getBall());
		}
		if(vec.x > fabs(vec.y)) {
			index = 1; // west
		}
		else if(vec.x < -fabs(vec.y)) {
			index = 3; // east
		}
		else if(vec.y < 0) {
			index = 2; // south
		}
		if(p->tackling()) {
			index += 8;
		}
		else if(!p->standing()) {
			// Just pick one of the images. We don't want the fallen player
			// to turn to look at the ball.
			index = 5;
		}

		if(index < 4 && p->getVelocity().length() > 0.2f) {
			index = 12 + index * 2;
			auto it = mAnimationStep.find(p);
			if(it == mAnimationStep.end()) {
				mAnimationStep[p] = 0;
			} else {
				unsigned int& s = it->second;
				if(!mPaused)
					s++;
				if(s > 24) {
					s = 0;
				}
				if(s >= 12) {
					index++;
				}
			}
		}
	}

	if(p->getTeam()->isFirst()) {
		return mPlayerTextureHome[index];
	}
	else {
		return mPlayerTextureAway[index];
	}
}

void MatchSDLGUI::drawPlayers()
{
	for(int i = 0; i < 2; i++) {
		const Player* pl;
		int j = 0;
		while(1) {
			pl = mMatch->getPlayer(i, j);
			if(!pl)
				break;
			j++;
			const Vector3& v(pl->getPosition());

			drawSprite(*mPlayerShadowTexture,
					Rectangle((-mCamera.x + v.x - 0.8f + v.z * 0.3f) * mScaleLevel + screenWidth * 0.5f,
						(-mCamera.y + v.y - 0.8f - v.z * 0.4f) * mScaleLevel + screenHeight * 0.5f,
						mScaleLevel * 2.0f, mScaleLevel * 2.0f),
					Rectangle(1, 1, -1, -1), playerShadowHeight);

			drawSprite(*playerTexture(pl),
					Rectangle((-mCamera.x + v.x - 0.8f) * mScaleLevel + screenWidth * 0.5f,
						(-mCamera.y + v.y + v.z * 0.6f) * mScaleLevel + screenHeight * 0.5f,
						mScaleLevel * 2.0f, mScaleLevel * 2.0f),
					Rectangle(1, 1, -1, -1), playerHeight);

			if(mDebugDisplay > 0) {
				drawText(v.x, v.y,
						FontConfig(pl->getAIController()->getDescription().c_str(),
							Color(0, 0, 0), 0.05f), false, true);
			}

			{
				char buf[128];
				sprintf(buf, "%d", pl->getShirtNumber());
				drawText(v.x, v.y + 2.0f,
						FontConfig(buf, !mObserver && pl == mPlayer ?
							Color(255, 255, 255) : Color(30, 30, 30), 0.05f),
						false, true);
			}
		}
	}
}

void MatchSDLGUI::drawBall()
{
	Vector3 v(mMatch->getBall()->getPosition());
	if(mMatch->getBall()->grabbed())
		v.z += 1.0f;

	drawSprite(*mBallShadowTexture, Rectangle((-mCamera.x + v.x - 0.2f + v.z * 0.3f) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y + v.y - 0.2f - v.z * 0.4f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * 0.6f, mScaleLevel * 0.6f),
			Rectangle(1, 1, -1, -1), ballShadowHeight);
	drawSprite(*mBallTexture, Rectangle((-mCamera.x + v.x - 0.2f) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y + v.y - 0.1f + v.z * 0.6f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * 0.6f, mScaleLevel * 0.6f),
			Rectangle(1, 1, -1, -1), ballHeight);
}

void MatchSDLGUI::startFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!mFreeCamera) {
		if(mControlledPlayerIndex != -1 && mCamFollowsPlayer) {
			mCamera.x = mPlayer->getPosition().x;
			mCamera.y = mPlayer->getPosition().y;
		}
		else if((mMatch->getPlayState() == PlayState::InPlay ||
					(MatchHelpers::distanceToPitch(*mMatch, mMatch->getBall()->getPosition()) < MAX_KICK_DISTANCE &&
					 mMatch->getPlayState() != PlayState::OutDirectFreekick)) ||
				(mControlledPlayerIndex != -1 && !mCamFollowsPlayer)) {
			mCamera.x = mMatch->getBall()->getPosition().x;
			mCamera.y = mMatch->getBall()->getPosition().y;
		}
	}

	{
		const float minXPitch = mMatch->getPitchWidth() * -0.5f - 5.0f;
		const float maxXPitch = mMatch->getPitchWidth() * 0.5f + 5.0f;
		const float minXCamPix = minXPitch * mScaleLevel + screenWidth * 0.5f;
		const float maxXCamPix = maxXPitch * mScaleLevel - screenWidth * 0.5f;
		const float minXCam = minXCamPix / mScaleLevel;
		const float maxXCam = maxXCamPix / mScaleLevel;
		if(minXCam > maxXCam)
			mCamera.x = 0.0f;
		else
			mCamera.x = clamp(minXCam, mCamera.x, maxXCam);
	}

	{
		const float minYPitch = mMatch->getPitchHeight() * -0.5f - 5.0f;
		const float maxYPitch = mMatch->getPitchHeight() * 0.5f + 5.0f;
		const float minYCamPix = minYPitch * mScaleLevel + screenHeight * 0.5f;
		const float maxYCamPix = maxYPitch * mScaleLevel - screenHeight * 0.5f;
		const float minYCam = minYCamPix / mScaleLevel;
		const float maxYCam = maxYCamPix / mScaleLevel;
		if(minYCam > maxYCam)
			mCamera.y = 0.0f;
		else
			mCamera.y = clamp(minYCam, mCamera.y, maxYCam);
	}
}

void MatchSDLGUI::finishFrame()
{
	SDL_GL_SwapBuffers();
}

bool MatchSDLGUI::colorConflict(const Common::Color& c1, const Common::Color& c2)
{
	// luminance difference is in range [0, 255].
	// hue difference is in range [0, pi].
	static const float lumCoeff = 1.0 / 80.0;
	static const float hueCoeff = 1.0 / PI;

	int lum1 = c1.luminance();
	int lum2 = c2.luminance();

	float lumDiff = abs(lum1 - lum2) * lumCoeff;

	float hue1 = c1.hue();
	float hue2 = c2.hue();

	if(hue2 < hue1)
		std::swap(hue1, hue2);

	if(hue2 > 0.5 * PI && hue1 <= -0.5 * PI)
		hue1 += 2.0 * PI;

	float hueDiff = fabs(hue1 - hue2) * hueCoeff;

	return lumDiff + hueDiff < 1.0f;
}

bool MatchSDLGUI::kitConflict(const Soccer::Kit& kit0, const Soccer::Kit& kit1) const
{
	if(MatchSDLGUI::colorConflict(kit0.getPrimaryShirtColor(), kit1.getPrimaryShirtColor()))
		return true;

	if(kit0.getKitType() != Soccer::Kit::KitType::Plain) {
		if(MatchSDLGUI::colorConflict(kit0.getSecondaryShirtColor(), kit1.getPrimaryShirtColor()))
			return true;

		if(kit1.getKitType() != Soccer::Kit::KitType::Plain) {
			if(MatchSDLGUI::colorConflict(kit0.getSecondaryShirtColor(), kit1.getSecondaryShirtColor()))
				return true;
		}
	}

	if(kit1.getKitType() != Soccer::Kit::KitType::Plain) {
		if(MatchSDLGUI::colorConflict(kit0.getPrimaryShirtColor(), kit1.getSecondaryShirtColor()))
			return true;
	}

	return false;
}

std::pair<const Soccer::Kit, const Soccer::Kit> MatchSDLGUI::getKits() const
{
	if(!kitConflict(mMatch->getTeam(0)->getHomeKit(),
			mMatch->getTeam(1)->getHomeKit()))
		return std::make_pair(mMatch->getTeam(0)->getHomeKit(),
				mMatch->getTeam(1)->getHomeKit());

	if(!kitConflict(mMatch->getTeam(0)->getHomeKit(),
			mMatch->getTeam(1)->getAwayKit()))
		return std::make_pair(mMatch->getTeam(0)->getHomeKit(),
				mMatch->getTeam(1)->getAwayKit());

	if(!kitConflict(mMatch->getTeam(0)->getAwayKit(),
			mMatch->getTeam(1)->getHomeKit()))
		return std::make_pair(mMatch->getTeam(0)->getAwayKit(),
				mMatch->getTeam(1)->getHomeKit());

	if(!kitConflict(mMatch->getTeam(0)->getAwayKit(),
			mMatch->getTeam(1)->getAwayKit()))
		return std::make_pair(mMatch->getTeam(0)->getAwayKit(),
				mMatch->getTeam(1)->getAwayKit());

	return std::make_pair(Soccer::Kit(Soccer::Kit::KitType::Plain,
				Common::Color::Blue, Common::Color::Blue, Common::Color::Blue, Common::Color::Blue),
			Soccer::Kit(Soccer::Kit::KitType::Plain,
				Common::Color::Green, Common::Color::Green, Common::Color::Green, Common::Color::Green));
}

Common::Color MatchSDLGUI::mapPitchColor(const Common::Color& c1, const Common::Color& c2,
		const Common::Color& c)
{
	unsigned int r = c1.r + (float)(c.r / 255.0f) * (c2.r - c1.r);
	unsigned int g = c1.g + (float)(c.r / 255.0f) * (c2.g - c1.g);
	unsigned int b = c1.b + (float)(c.r / 255.0f) * (c2.b - c1.b);

	return Common::Color(r, g, b);
}

Common::Color MatchSDLGUI::mapKitColor(const Soccer::Kit& kit, const Common::Color& c)
{
	if(c.r == 255) {
		switch(kit.getKitType()) {
			case Soccer::Kit::KitType::Plain:
				return kit.getPrimaryShirtColor();

			case Soccer::Kit::KitType::Striped:
				if(c.g & 0x80)
					return kit.getSecondaryShirtColor();
				else
					return kit.getPrimaryShirtColor();

			case Soccer::Kit::KitType::HorizontalStriped:
				if(c.b & 0x80)
					return kit.getSecondaryShirtColor();
				else
					return kit.getPrimaryShirtColor();

			case Soccer::Kit::KitType::ColoredSleeves:
				if(c.g & 0x40)
					return kit.getSecondaryShirtColor();
				else
					return kit.getPrimaryShirtColor();
		}
	}
	else if(c.r == 240 && c.g == 240 && c.b == 0) {
		return kit.getShortsColor();
	}
	else if(c.r == 0 && c.g == 0 && c.b == 255) {
		return kit.getSocksColor();
	}

	return c;
}

void MatchSDLGUI::loadTextures()
{
	mBallTexture = boost::shared_ptr<Texture>(new Texture("share/ball1.png", 0, 8));
	mBallShadowTexture = boost::shared_ptr<Texture>(new Texture("share/ball1shadow.png", 0, 8));
	SDLSurface surfs[16] = { SDLSurface("share/player1-n.png"),
		SDLSurface("share/player1-w.png"),
		SDLSurface("share/player1-s.png"),
		SDLSurface("share/player1-e.png"),
		SDLSurface("share/player1-fallen-n.png"),
		SDLSurface("share/player1-fallen-w.png"),
		SDLSurface("share/player1-fallen-s.png"),
		SDLSurface("share/player1-fallen-e.png"),
		SDLSurface("share/player1-tackle-n.png"),
		SDLSurface("share/player1-tackle-w.png"),
		SDLSurface("share/player1-tackle-s.png"),
		SDLSurface("share/player1-tackle-e.png"),
		SDLSurface("share/player1-dive-n-w.png"),
		SDLSurface("share/player1-dive-n-e.png"),
		SDLSurface("share/player1-dive-s-w.png"),
		SDLSurface("share/player1-dive-s-e.png"),
       	};

	std::vector<SDLSurface> homes;
	std::vector<SDLSurface> aways;

	std::pair<const Soccer::Kit, const Soccer::Kit> kits = getKits();
	for(auto& s : surfs) {
		SDLSurface hs(s);
		SDLSurface as(s);
		hs.mapPixelColor( [&] (const Color& c) { return mapKitColor(kits.first, c); } );
		as.mapPixelColor( [&] (const Color& c) { return mapKitColor(kits.second, c); } );
		homes.push_back(hs);
		aways.push_back(as);
	}

	for(unsigned int i = 0; i < 12; i++) {
		mPlayerTextureHome[i] = boost::shared_ptr<Texture>(new Texture(homes[i], 0, 32));
		mPlayerTextureAway[i] = boost::shared_ptr<Texture>(new Texture(aways[i], 0, 32));
	}

	// n, w, s, e
	for(unsigned int j = 0; j < 4; j++) {
		// frame 1
		mPlayerTextureHome[12 + j * 2] = boost::shared_ptr<Texture>(new Texture(homes[j], 32, 32));
		mPlayerTextureAway[12 + j * 2] = boost::shared_ptr<Texture>(new Texture(aways[j], 32, 32));
		// frame 2
		mPlayerTextureHome[12 + j * 2 + 1] = boost::shared_ptr<Texture>(new Texture(homes[j], 64, 32));
		mPlayerTextureAway[12 + j * 2 + 1] = boost::shared_ptr<Texture>(new Texture(aways[j], 64, 32));
	}

	// diving
	for(unsigned int j = 0; j < 2; j++) {
		for(unsigned int k = 0; k < 2; k++) {
			// frame 1
			mPlayerTextureHome[20 + j * 4 + k * 2] = boost::shared_ptr<Texture>(new Texture(homes[12 + j * 2 + k], 32, 32));
			mPlayerTextureAway[20 + j * 4 + k * 2] = boost::shared_ptr<Texture>(new Texture(aways[12 + j * 2 + k], 32, 32));
			// frame 2
			mPlayerTextureHome[20 + j * 4 + k * 2 + 1] = boost::shared_ptr<Texture>(new Texture(homes[12 + j * 2 + k], 64, 32));
			mPlayerTextureAway[20 + j * 4 + k * 2 + 1] = boost::shared_ptr<Texture>(new Texture(aways[12 + j * 2 + k], 64, 32));
		}
	}

	int grassValue = rand() % 2 + 1;
	std::stringstream ss;
	ss << "share/grass" << grassValue << ".png";
	Common::Color color1(6, 100, 9);
	Common::Color color2(25, 162, 20);

	color1.r += rand() % 10 - 5;
	color1.g += rand() % 10 - 5;
	color1.b += rand() % 10 - 5;
	color2.r += rand() % 10 - 5;
	color2.g += rand() % 10 - 5;
	color2.b += rand() % 10 - 5;

	SDLSurface pitchSurface = SDLSurface(ss.str().c_str());
	pitchSurface.mapPixelColor( [&] (const Color& c) { return mapPitchColor(color1, color2, c); } );
	mPitchTexture = boost::shared_ptr<Texture>(new Texture(pitchSurface, 0, 0));
	mPlayerShadowTexture = boost::shared_ptr<Texture>(new Texture("share/player1shadow.png", 0, 32));

	mGoal1Texture = boost::shared_ptr<Texture>(new Texture("share/goal1.png", 0, 0));
}

bool MatchSDLGUI::handleInput(float frameTime)
{
	bool quitting = false;
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE:
						quitting = true;
						break;
					case SDLK_MINUS:
					case SDLK_KP_MINUS:
					case SDLK_PAGEDOWN:
						mScaleLevelVelocity = -1.0f; break;
					case SDLK_PLUS:
					case SDLK_KP_PLUS:
					case SDLK_PAGEUP:
						mScaleLevelVelocity = 1.0f; break;

					case SDLK_c:
						if(mControlledPlayerIndex != -1) {
							mCamFollowsPlayer = !mCamFollowsPlayer;
						}
						break;

					case SDLK_f:
						if(SDL_GetModState() & KMOD_CTRL) {
							if(mPlayerControlVelocity.null())
								mFreeCamera = !mFreeCamera;
						}
						break;

					case SDLK_w:
					case SDLK_UP:
						if(mFreeCamera)
							mCameraVelocity.y = -1.0f;
						else
							mPlayerControlVelocity.y = 1.0f;
						break;

					case SDLK_s:
					case SDLK_DOWN:
						if(mFreeCamera)
							mCameraVelocity.y = 1.0f;
						else
							mPlayerControlVelocity.y = -1.0f;
						break;

					case SDLK_d:
					case SDLK_RIGHT:
						if(mFreeCamera)
							mCameraVelocity.x = -1.0f;
						else
							mPlayerControlVelocity.x = 1.0f;
						break;

					case SDLK_a:
					case SDLK_LEFT:
						if(mFreeCamera)
							mCameraVelocity.x = 1.0f;
						else
							mPlayerControlVelocity.x = -1.0f;
						break;

					case SDLK_RCTRL:
						mPlayerKickPowerVelocity = 1.0f; break;

					case SDLK_p:
					case SDLK_PAUSE:
						mPaused = !mPaused;
						break;

					case SDLK_v:
						if(SDL_GetModState() & KMOD_CTRL) {
							mDebugDisplay++;
							if(mDebugDisplay > 3)
								mDebugDisplay = 0;
						}
						break;

					case SDLK_SPACE:
						if(mMatch->getBall()->getPosition().z > 1.0f) {
							mHeading = true;
						}
						else {
							mTackling = true;
						}
						break;

					default:
						break;
				}
				break;

			case SDL_KEYUP:
				switch(event.key.keysym.sym) {
					case SDLK_MINUS:
					case SDLK_KP_MINUS:
					case SDLK_PLUS:
					case SDLK_KP_PLUS:
					case SDLK_PAGEUP:
					case SDLK_PAGEDOWN:
						mScaleLevelVelocity = 0.0f; break;

					case SDLK_w:
					case SDLK_s:
					case SDLK_UP:
					case SDLK_DOWN:
						if(mFreeCamera)
							mCameraVelocity.y = 0.0f;
						else
							mPlayerControlVelocity.y = 0.0f;
						break;

					case SDLK_a:
					case SDLK_d:
					case SDLK_RIGHT:
					case SDLK_LEFT:
						if(mFreeCamera)
							mCameraVelocity.x = 0.0f;
						else
							mPlayerControlVelocity.x = 0.0f;
						break;

					case SDLK_RCTRL:
						mPlayerKickPowerVelocity = 0.0f; break;

					default:
						break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				switch(event.button.button) {
					case SDL_BUTTON_RIGHT:
					case SDL_BUTTON_LEFT:
						if(event.button.button == SDL_BUTTON_RIGHT)
							mPlayerKickPowerVelocity = 1.0f;
						else
							mPlayerKickPowerVelocity = 0.5f;
						mMouseAim = true;
						break;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				switch(event.button.button) {
					case SDL_BUTTON_RIGHT:
					case SDL_BUTTON_LEFT:
						mPlayerKickPowerVelocity = 0.0f;
						break;

					case SDL_BUTTON_WHEELUP:
						mScaleLevel += 4.0f; break;
					case SDL_BUTTON_WHEELDOWN:
						mScaleLevel -= 4.0f; break;
					default:
						break;
				}
				break;

			case SDL_QUIT:
				quitting = true;
				break;
			default:
				break;
		}
	}
	handleInputState(frameTime);
	return quitting;
}

void MatchSDLGUI::handleInputState(float frameTime)
{
	if(mFreeCamera) {
		mCamera -= mCameraVelocity * frameTime * 10.0f;
	}
	mScaleLevel += mScaleLevelVelocity * frameTime * 10.0f;
	mScaleLevel = clamp(10.0f, mScaleLevel, 20.0f);
	if(!mPlayerKickPower && mPlayerKickPowerVelocity) {
		mPlayerKickPower = 0.3f;
	}
	mPlayerKickPower += mPlayerKickPowerVelocity * frameTime;
}

void MatchSDLGUI::drawSprite(const Texture& t,
		const Rectangle& vertcoords,
		const Rectangle& texcoords, float depth)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t.getTexture());
	glBegin(GL_QUADS);
	glTexCoord2f(texcoords.x, texcoords.y);
	glVertex3f(vertcoords.x, vertcoords.y, depth);
	glTexCoord2f(texcoords.x + texcoords.w, texcoords.y);
	glVertex3f(vertcoords.x + vertcoords.w, vertcoords.y, depth);
	glTexCoord2f(texcoords.x + texcoords.w, texcoords.y + texcoords.h);
	glVertex3f(vertcoords.x + vertcoords.w, vertcoords.y + vertcoords.h, depth);
	glTexCoord2f(texcoords.x, texcoords.y + texcoords.h);
	glVertex3f(vertcoords.x, vertcoords.y + vertcoords.h, depth);
	glEnd();
}

boost::shared_ptr<PlayerAction> MatchSDLGUI::act(double time)
{
	float kickpower = 0.0f;
	bool mouseaim = false;
	bool tackling = false;
	bool heading = false;

	if(mTackling) {
		mTackling = false;
		tackling = true;
	}
	if(mHeading) {
		mHeading = false;
		heading = true;
	}

	Vector3 toBall = Vector3(mMatch->getBall()->getPosition() - mPlayer->getPosition());
	if((mMouseAim && MatchHelpers::canKickBall(*mPlayer))|| (mPlayerKickPower && !mPlayerKickPowerVelocity)) {
		// about to kick
		kickpower = mPlayerKickPower;
		mPlayerKickPower = 0.0f;
		mouseaim = mMouseAim;
		if(mPlayerKickPower && !mPlayerKickPowerVelocity) {
			mMouseAim = false;
		}
	}
	if(mMatch->getPlayState() == PlayState::OutKickoff && !MatchHelpers::myTeamInControl(*mPlayer)) {
		// opponent kickoff
		return AIHelpers::createMoveActionTo(*mPlayer,
				mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getHomePosition()));
	}
	if(!playing(mMatch->getPlayState())) {
		// restart
		// if blocking restart, turn over to AI
		if(MatchHelpers::playerBlockingRestart(*mPlayer)) {
			mPlayer->setAIControlled();
			return boost::shared_ptr<PlayerAction>(new IdlePA());
		}
		if(MatchHelpers::myTeamInControl(*mPlayer)) {
			bool nearest = MatchHelpers::nearestOwnPlayerTo(*mPlayer,
					mPlayer->getMatch()->getBall()->getPosition());
			if(nearest && toBall.length() > MAX_KICK_DISTANCE) {
				return AIHelpers::createMoveActionToBall(*mPlayer);
			}
		}
	}

	if(heading) {
		Vector3 jump = mPlayerControlVelocity;
		jump.z = 1.0f;
		return boost::shared_ptr<PlayerAction>(new JumpToPA(jump));
	}

	if(kickpower) {
		// kicking
		if(!mouseaim) {
			Vector3 kicktgt = mPlayerControlVelocity * kickpower;
			if(mPlayerKickPowerVelocity > 0.5f)
				kicktgt.z += kicktgt.length() * 0.3f;
			return boost::shared_ptr<PlayerAction>(new KickBallPA(Vector3(kicktgt)));
		}
		else {
			// pass, shot, dribble?
			Vector3 tgt(getMousePositionOnPitch());
			Vector3 goalpos = MatchHelpers::oppositeGoalPosition(*mPlayer);
			float goaldiff = (tgt - goalpos).length();
			Player* passtgt = MatchHelpers::nearestOwnPlayerTo(*mPlayer->getTeam(), tgt);
			float passdiff = (tgt - passtgt->getPosition()).length();
			if(goaldiff < 10.0f && goaldiff < passdiff) {
				// full power
				Vector3 kicktgt = getMousePositionOnPitch();
				if(mPlayerKickPowerVelocity > 0.5f)
					kicktgt.z = kicktgt.length() * 0.1f;
				return boost::shared_ptr<PlayerAction>(new KickBallPA(kicktgt, nullptr, true));
			}
			else if(passdiff < 5.0f) {
				// pass
				Vector3 kicktgt = AIHelpers::getPassKickVector(*mPlayer, *passtgt);
				if(mPlayerKickPowerVelocity > 0.5f)
					kicktgt.z += kicktgt.length() * 0.3f;
				return boost::shared_ptr<PlayerAction>(new KickBallPA(kicktgt, passtgt));
			}
			else {
				// dribble
				Vector3 kicktgt = AIHelpers::getPassKickVector(*mPlayer, tgt);
				if(mPlayerKickPowerVelocity > 0.5f)
					kicktgt.z += kicktgt.length() * 0.3f;
				return boost::shared_ptr<PlayerAction>(new KickBallPA(kicktgt));
			}
		}
	}
	else {
		if(mPlayerKickPowerVelocity) {
			// powering kick up => run to/stay on ball
			return AIHelpers::createMoveActionTo(*mPlayer, mMatch->getBall()->getPosition());
		}
		if(!mPlayerControlVelocity.null()) {
			if(tackling) {
				return boost::shared_ptr<PlayerAction>(new TacklePA(Vector3(mPlayerControlVelocity)));
			}
			else {
				// not about to kick or tackle => run around
				return boost::shared_ptr<PlayerAction>(new RunToPA(Vector3(mPlayerControlVelocity)));
			}
		}
		return boost::shared_ptr<PlayerAction>(new IdlePA());
	}
}

void MatchSDLGUI::setPlayerController(double frameTime)
{
	mPlayerSwitchTimer.doCountdown(frameTime);
	mPlayerSwitchTimer.check();
	if(playing(mMatch->getMatchHalf())) {
		if(mPlayer->isAIControlled() &&
				(playing(mMatch->getPlayState()) ||
				!MatchHelpers::playerBlockingRestart(*mPlayer))) {
			mPlayer->setController(this);
			mPlayerKickPower = 0.0f;
		}
		if(mControlledPlayerIndex == -1) {
			Player* pl = MatchHelpers::nearestOwnPlayerToBall(*mMatch->getTeam(mControlledTeamIndex));
			if(pl != mPlayer && !mPlayerSwitchTimer.running() &&
					(!pl->isGoalkeeper() || !playing(mMatch->getPlayState()))) {
				mPlayer->setAIControlled();
				setPlayer(pl);
				pl->setController(this);
				mPlayerSwitchTimer.rewind();
			}
		}
	}
	else if(!mPlayer->isAIControlled()) {
		mPlayer->setAIControlled();
	}
}

void MatchSDLGUI::drawText(float x, float y,
		const FontConfig& f,
		bool screencoordinates, bool centered)
{
	if(f.mText.size() == 0)
		return;
	auto it = mTextMap.find(f);
	if(it == mTextMap.end()) {
		SDL_Surface* text;
		SDL_Color color = {f.mColor.r, f.mColor.g, f.mColor.b};

		text = TTF_RenderUTF8_Blended(mFont, f.mText.c_str(), color);
		if(!text) {
			fprintf(stderr, "Could not render text: %s\n",
					TTF_GetError());
			return;
		}
		else {
			boost::shared_ptr<Texture> texture(new Texture(text));
			boost::shared_ptr<TextTexture> ttexture(new TextTexture(texture, text->w, text->h));
			auto it2 = mTextMap.insert(std::make_pair(f, ttexture));
			it = it2.first;
			SDL_FreeSurface(text);
		}

	}

	assert(it != mTextMap.end());
	float spritex, spritey;
	float spritewidth, spriteheight;
	if(screencoordinates) {
		spritex = x;
		spritey = y;
		spritewidth  = it->first.mScale * it->second->mWidth;
		spriteheight = it->first.mScale * it->second->mHeight;
	}
	else {
		spritex = (-mCamera.x + x) * mScaleLevel + screenWidth * 0.5f;
		spritey = (-mCamera.y + y) * mScaleLevel + screenHeight * 0.5f;
		spritewidth  = mScaleLevel * it->first.mScale * it->second->mWidth;
		spriteheight = mScaleLevel * it->first.mScale * it->second->mHeight;
	}
	if(centered) {
		spritex -= spritewidth * 0.5f;
	}

	drawSprite(*it->second->mTexture, Rectangle(spritex, spritey,
				spritewidth, spriteheight),
			Rectangle(0, 1, 1, -1), textHeight);
}

Vector3 MatchSDLGUI::getMousePositionOnPitch() const
{
	int xp, yp;
	float x, y;
	SDL_GetMouseState(&xp, &yp);
	yp = screenHeight - yp;

	x = float(xp) / mScaleLevel + mCamera.x - (screenWidth / (2.0f * mScaleLevel));
	y = float(yp) / mScaleLevel + mCamera.y - (screenHeight / (2.0f * mScaleLevel));

	// printf("Position at (%3.3f, %3.3f)\n", x, y);
	return Vector3(x, y, 0);
}

void MatchSDLGUI::drawPitchLines()
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	float addx = screenWidth * 0.5f;
	float addy = screenHeight * 0.5f;

	for(auto& r : mPitchLines) {
		glBegin(GL_LINE_STRIP);
		for(auto& l : r) {
			glVertex3f((l.x - mCamera.x) * mScaleLevel + addx,
					(l.y - mCamera.y) * mScaleLevel + addy,
					pitchLineHeight);
		}
		glEnd();
	}

	// penalty spots
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	glVertex3f(-mCamera.x * mScaleLevel + addx,
			(11.0f - mMatch->getPitchHeight() * 0.5f - mCamera.y) * mScaleLevel + addy,
			pitchLineHeight);
	glVertex3f(-mCamera.x * mScaleLevel + addx,
			(-11.0f + mMatch->getPitchHeight() * 0.5f - mCamera.y) * mScaleLevel + addy,
			pitchLineHeight);
	glEnd();

	// centre circle
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < 32; i++) {
		float v = PI * i / 16.0f;
		glVertex3f((9.15f * sin(v) - mCamera.x) * mScaleLevel + addx,
				(9.15f * cos(v) - mCamera.y) * mScaleLevel + addy,
				pitchLineHeight);
	}
	glVertex3f(-mCamera.x * mScaleLevel + addx,
			(9.15f - mCamera.y) * mScaleLevel + addy,
			pitchLineHeight);
	glEnd();
}

void MatchSDLGUI::setupPitchLines()
{
	float pwidth = mMatch->getPitchWidth();
	float pheight = mMatch->getPitchHeight();

	std::vector<LineCoord> borders;
	borders.push_back(LineCoord(-pwidth * 0.5f, -pheight * 0.5f));
	borders.push_back(LineCoord(pwidth * 0.5f, -pheight * 0.5f));
	borders.push_back(LineCoord(pwidth * 0.5f, pheight * 0.5f));
	borders.push_back(LineCoord(-pwidth * 0.5f, pheight * 0.5f));
	borders.push_back(LineCoord(-pwidth * 0.5f, -pheight * 0.5f));
	mPitchLines.push_back(borders);

	std::vector<LineCoord> midline;
	midline.push_back(LineCoord(-pwidth * 0.5f, 0));
	midline.push_back(LineCoord(pwidth * 0.5f, 0));
	mPitchLines.push_back(midline);

	std::vector<LineCoord> penaltybox;
	penaltybox.push_back(LineCoord(-20.15f, -pheight * 0.5f));
	penaltybox.push_back(LineCoord(-20.15f, 16.5f - pheight * 0.5f));
	penaltybox.push_back(LineCoord(20.15f, 16.5f - pheight * 0.5f));
	penaltybox.push_back(LineCoord(20.15f, -pheight * 0.5f));
	mPitchLines.push_back(penaltybox);

	for(auto& l : penaltybox)
		l.y = -l.y;
	mPitchLines.push_back(penaltybox);

	std::vector<LineCoord> gkbox;
	gkbox.push_back(LineCoord(-9.16f, -pheight * 0.5f));
	gkbox.push_back(LineCoord(-9.16f, 5.5f - pheight * 0.5f));
	gkbox.push_back(LineCoord(9.16f, 5.5f - pheight * 0.5f));
	gkbox.push_back(LineCoord(9.16f, -pheight * 0.5f));
	mPitchLines.push_back(gkbox);

	for(auto& l : gkbox)
		l.y = -l.y;
	mPitchLines.push_back(gkbox);
}

void MatchSDLGUI::drawGoals()
{
	float pheight = mMatch->getPitchHeight();
	static const float goalWidth2 = 7.32f * 0.5f;
	drawSprite(*mGoal1Texture, Rectangle((-mCamera.x - goalWidth2) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y + pheight * 0.5f - 0.1f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * goalWidth2 * 2.0f,
				mScaleLevel * GOAL_WIDTH_2),
			Rectangle(0, 1, 1, -1), goalHeight);
	drawSprite(*mGoal1Texture, Rectangle((-mCamera.x - goalWidth2) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y - pheight * 0.5f + 0.1f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * goalWidth2 * 2.0f,
				mScaleLevel * -GOAL_WIDTH_2),
			Rectangle(0, 1, 1, -1), goalHeight);
}


