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
	mScaleLevel(11.0f),
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

	mScreen = SDL_utils::initSDL(screenWidth, screenHeight);

	loadTextures();
	loadFont();

	SDL_utils::setupOrthoScreen(screenWidth, screenHeight);
	setupPitchLines();

	setPlayer(mMatch->getPlayer(mControlledTeamIndex, mControlledPlayerIndex == -1 ?
				9 : mControlledPlayerIndex));
	setPlayerController(0.0f);

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
			finishFrame();
		}

		if(!mPaused) {
			if(!progressMatch(frameTime))
				break;
		}
	}
	if(mMatch->matchOver()) {
		Soccer::MatchResult mres(mMatch->getScore(1), mMatch->getScore(0));
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
				mScaleLevel * pwidth * 2.0f,
				mScaleLevel * pheight * 2.0f),
			Rectangle(0, 0, 20, 20), pitchHeight);
	drawPitchLines();
	drawGoals();
	char resultbuf[256];
	snprintf(resultbuf, 255, "%s %d - %d %s", mMatch->getTeam(0)->getName().c_str(),
			mMatch->getScore(true), mMatch->getScore(false),
			mMatch->getTeam(1)->getName().c_str());
	resultbuf[255] = 0;
	drawText(90, screenHeight - 30, FontConfig(resultbuf, Color(255, 255, 255), 1.5f), true, false);

	char timebuf[128];
	int min = int(mMatch->getTime());
	if(mMatch->getMatchHalf() >= MatchHalf::HalfTimePauseBegin)
		min += 45;
	if(mMatch->getMatchHalf() >= MatchHalf::Finished)
		min += 45;
	sprintf(timebuf, "%d min.", min);
	drawText(10, screenHeight - 30, FontConfig(timebuf, Color(255, 255, 255), 1.5f), true, false);

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

	if(mDebugDisplay > 1) {
		const Team* t = mMatch->getTeam(mDebugDisplay == 2 ? 0 : 1);
		for(int j = -pheight * 0.5f + 8; j < pheight * 0.5 - 8; j += 8) {
			for(int i = -pwidth * 0.5f + 8; i < pwidth * 0.5 - 8; i += 8) {
				float score1 = t->getShotScoreAt(AbsVector3(i, j, 0));
				float score2 = t->getPassScoreAt(AbsVector3(i, j, 0));
				glDisable(GL_TEXTURE_2D);
				glPointSize(5.0f);
				glBegin(GL_POINTS);
				glColor3f(score1, 0.0f, 0.0f);
				glVertex3f((-mCamera.x + i) * mScaleLevel + screenWidth * 0.5f - 3.0f,
						(-mCamera.y + j) * mScaleLevel + screenHeight * 0.5f,
						textHeight);
				glColor3f(0.0f, 0.0f, score2);
				glVertex3f((-mCamera.x + i) * mScaleLevel + screenWidth * 0.5f + 3.0f,
						(-mCamera.y + j) * mScaleLevel + screenHeight * 0.5f,
						textHeight);
				glEnd();
				glColor3f(1.0f, 1.0f, 1.0f);
				glEnable(GL_TEXTURE_2D);
			}
		}
	}

	if(mPaused) {
		drawText(screenWidth / 2, screenHeight / 2, FontConfig("Paused", Color(255, 255, 255), 2.0f),
				true, true);
	}
}

const boost::shared_ptr<Texture> MatchSDLGUI::playerTexture(const Player* p)
{
	AbsVector3 vec = p->getVelocity();
	if(vec.v.null()) {
		vec = MatchEntity::vectorFromTo(*p, *mMatch->getBall());
	}
	int dir = 0;
	if(vec.v.x > fabs(vec.v.y)) {
		dir = 1; // west
	}
	else if(vec.v.x < -fabs(vec.v.y)) {
		dir = 3; // east
	}
	else if(vec.v.y < 0) {
		dir = 2; // south
	}
	if(p->tackling()) {
		dir += 8;
	}
	else if(!p->standing()) {
		// Just pick one of the images. We don't want the fallen player
		// to turn to look at the ball.
		dir = 5;
	}

	if(p->getTeam()->isFirst()) {
		return mPlayerTextureHome[dir];
	}
	else {
		return mPlayerTextureAway[dir];
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
			const AbsVector3& v(pl->getPosition());

			drawSprite(*mPlayerShadowTexture,
					Rectangle((-mCamera.x + v.v.x - 0.8f + v.v.z * 0.3f) * mScaleLevel + screenWidth * 0.5f,
						(-mCamera.y + v.v.y - 0.8f - v.v.z * 0.4f) * mScaleLevel + screenHeight * 0.5f,
						mScaleLevel * 2.0f, mScaleLevel * 2.0f),
					Rectangle(1, 1, -1, -1), playerShadowHeight);

			drawSprite(*playerTexture(pl),
					Rectangle((-mCamera.x + v.v.x - 0.8f) * mScaleLevel + screenWidth * 0.5f,
						(-mCamera.y + v.v.y + v.v.z * 0.6f) * mScaleLevel + screenHeight * 0.5f,
						mScaleLevel * 2.0f, mScaleLevel * 2.0f),
					Rectangle(1, 1, -1, -1), playerHeight);

			if(mDebugDisplay > 0) {
				drawText(v.v.x, v.v.y,
						FontConfig(pl->getAIController()->getDescription().c_str(),
							Color(0, 0, 0), 0.05f), false, true);
			}

			{
				char buf[128];
				sprintf(buf, "%d", pl->getShirtNumber());
				drawText(v.v.x, v.v.y + 2.0f,
						FontConfig(buf, !mObserver && pl == mPlayer ?
							Color(255, 255, 255) : Color(30, 30, 30), 0.05f),
						false, true);
			}
		}
	}
}

void MatchSDLGUI::drawBall()
{
	const AbsVector3& v(mMatch->getBall()->getPosition());

	drawSprite(*mBallShadowTexture, Rectangle((-mCamera.x + v.v.x - 0.2f + v.v.z * 0.3f) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y + v.v.y - 0.2f - v.v.z * 0.4f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * 0.6f, mScaleLevel * 0.6f),
			Rectangle(1, 1, -1, -1), ballShadowHeight);
	drawSprite(*mBallTexture, Rectangle((-mCamera.x + v.v.x - 0.2f) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y + v.v.y - 0.1f + v.v.z * 0.6f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * 0.6f, mScaleLevel * 0.6f),
			Rectangle(1, 1, -1, -1), ballHeight);
}

void MatchSDLGUI::startFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!mFreeCamera) {
		if(mControlledPlayerIndex != -1 && mCamFollowsPlayer) {
			mCamera.x = mPlayer->getPosition().v.x;
			mCamera.y = mPlayer->getPosition().v.y;
		}
		else if(mMatch->getPlayState() == PlayState::InPlay ||
				(MatchHelpers::distanceToPitch(*mMatch, mMatch->getBall()->getPosition()) < MAX_KICK_DISTANCE &&
				 mMatch->getPlayState() != PlayState::OutDirectFreekick)) {
			mCamera.x = mMatch->getBall()->getPosition().v.x;
			mCamera.y = mMatch->getBall()->getPosition().v.y;
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

bool MatchSDLGUI::kitConflict(const Soccer::Kit& kit0, const Soccer::Kit& kit1) const
{
	static const int colorThreshold = 160;

	if(kit0.getPrimaryShirtColor() - kit1.getPrimaryShirtColor() < colorThreshold)
		return true;

	if(kit0.getPrimaryShirtColor() - kit1.getSecondaryShirtColor() < colorThreshold)
		return true;

	if(kit0.getSecondaryShirtColor() - kit1.getPrimaryShirtColor() < colorThreshold)
		return true;

	if(kit0.getSecondaryShirtColor() - kit1.getSecondaryShirtColor() < colorThreshold)
		return true;

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
				Common::Color::Red, Common::Color::Red, Common::Color::Red, Common::Color::Red),
			Soccer::Kit(Soccer::Kit::KitType::Plain,
				Common::Color::Blue, Common::Color::Blue, Common::Color::Blue, Common::Color::Blue));
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
	SDLSurface surfs[12] = { SDLSurface("share/player1-n.png"),
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
		SDLSurface("share/player1-tackle-e.png") };

	std::pair<const Soccer::Kit, const Soccer::Kit> kits = getKits();
	int i = 0;
	for(auto& s : surfs) {
		SDLSurface homes(s);
		SDLSurface aways(s);
		homes.mapPixelColor( [&] (const Color& c) { return mapKitColor(kits.first, c); } );
		aways.mapPixelColor( [&] (const Color& c) { return mapKitColor(kits.second, c); } );
		mPlayerTextureHome[i] = boost::shared_ptr<Texture>(new Texture(homes, 0, 32));
		mPlayerTextureAway[i] = boost::shared_ptr<Texture>(new Texture(aways, 0, 32));

		i++;
	}
	mPitchTexture = boost::shared_ptr<Texture>(new Texture("share/grass1.png", 0, 0));
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
						if(mPlayerControlVelocity.null())
							mFreeCamera = !mFreeCamera;
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
						mDebugDisplay++;
						if(mDebugDisplay > 3)
							mDebugDisplay = 0;
						break;

					case SDLK_SPACE:
						if(mMatch->getBall()->getPosition().v.z > 1.0f) {
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

	AbsVector3 toBall = AbsVector3(mMatch->getBall()->getPosition().v - mPlayer->getPosition().v);
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
		if(MatchHelpers::myTeamInControl(*mPlayer)) {
			bool nearest = MatchHelpers::nearestOwnPlayerTo(*mPlayer,
					mPlayer->getMatch()->getBall()->getPosition());
			if(nearest && toBall.v.length() > MAX_KICK_DISTANCE) {
				return AIHelpers::createMoveActionToBall(*mPlayer);
			}
		}
		else {
			// if blocking restart, turn over to AI
			mPlayer->setAIControlled();
			return boost::shared_ptr<PlayerAction>(new IdlePA());
		}
	}

	if(heading) {
		AbsVector3 jump = mPlayerControlVelocity;
		jump.v.z = 1.0f;
		std::cout << "Heading: " << jump.v << "\n";
		return boost::shared_ptr<PlayerAction>(new JumpToPA(jump));
	}

	if(kickpower) {
		// kicking
		if(!mouseaim) {
			Vector3 kicktgt = mPlayerControlVelocity * kickpower;
			if(mPlayerKickPowerVelocity > 0.5f)
				kicktgt.z += kicktgt.length() * 0.3f;
			return boost::shared_ptr<PlayerAction>(new KickBallPA(AbsVector3(kicktgt)));
		}
		else {
			// pass, shot, dribble?
			AbsVector3 tgt(getMousePositionOnPitch());
			AbsVector3 goalpos = MatchHelpers::oppositeGoalPosition(*mPlayer);
			float goaldiff = (tgt.v - goalpos.v).length();
			Player* passtgt = MatchHelpers::nearestOwnPlayerTo(*mPlayer->getTeam(), tgt);
			float passdiff = (tgt.v - passtgt->getPosition().v).length();
			if(goaldiff < 10.0f && goaldiff < passdiff) {
				// full power
				AbsVector3 kicktgt = getMousePositionOnPitch();
				if(mPlayerKickPowerVelocity > 0.5f)
					kicktgt.v.z = kicktgt.v.length() * 0.1f;
				return boost::shared_ptr<PlayerAction>(new KickBallPA(kicktgt, nullptr, true));
			}
			else if(passdiff < 5.0f) {
				// pass
				AbsVector3 kicktgt = AIHelpers::getPassKickVector(*mPlayer, *passtgt);
				if(mPlayerKickPowerVelocity > 0.5f)
					kicktgt.v.z += kicktgt.v.length() * 0.3f;
				return boost::shared_ptr<PlayerAction>(new KickBallPA(kicktgt, passtgt));
			}
			else {
				// dribble
				AbsVector3 kicktgt = AIHelpers::getPassKickVector(*mPlayer, tgt);
				if(mPlayerKickPowerVelocity > 0.5f)
					kicktgt.v.z += kicktgt.v.length() * 0.3f;
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
				return boost::shared_ptr<PlayerAction>(new TacklePA(AbsVector3(mPlayerControlVelocity)));
			}
			else {
				// not about to kick or tackle => run around
				return boost::shared_ptr<PlayerAction>(new RunToPA(AbsVector3(mPlayerControlVelocity)));
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
		if(mPlayer->isAIControlled() && playing(mMatch->getPlayState())) {
			mPlayer->setController(this);
			mPlayerKickPower = 0.0f;
			printf("Now controlling\n");
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

AbsVector3 MatchSDLGUI::getMousePositionOnPitch() const
{
	int xp, yp;
	float x, y;
	SDL_GetMouseState(&xp, &yp);
	yp = screenHeight - yp;

	x = float(xp) / mScaleLevel + mCamera.x - (screenWidth / (2.0f * mScaleLevel));
	y = float(yp) / mScaleLevel + mCamera.y - (screenHeight / (2.0f * mScaleLevel));

	// printf("Position at (%3.3f, %3.3f)\n", x, y);
	return AbsVector3(x, y, 0);
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


