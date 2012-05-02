#include <assert.h>
#include <string.h>

#include <stdexcept>
#include <sstream>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "MatchSDLGUI.h"
#include "MatchHelpers.h"
#include "PlayerAIController.h"

static const int screenWidth = 800;
static const int screenHeight = 600;

MatchSDLGUI::MatchSDLGUI(std::shared_ptr<Match> match, int argc, char** argv)
	: MatchGUI(match),
	PlayerController(mMatch->getPlayer(0, 9)),
	mScaleLevel(15.0f),
	mScaleLevelVelocity(0.0f),
	mFreeCamera(false),
	mPlayerKickPower(0.0f),
	mPlayerKickPowerVelocity(0.0f),
	mFont(nullptr),
	mObserver(false)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		throw std::runtime_error("SDL init");
	}
	if(SDL_EnableUNICODE(1) == -1) {
		fprintf(stderr, "Unable to enable Unicode: %s\n", SDL_GetError());
		throw std::runtime_error("Enable Unicode");
	}
	mScreen = SDL_SetVideoMode(screenWidth, screenHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL);
	if(!mScreen) {
		fprintf(stderr, "Unable to set video mode\n");
		throw std::runtime_error("Set video mode");
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef __WIN32__
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
#endif
	if(IMG_Init(IMG_INIT_PNG) == -1) {
		fprintf(stderr, "Unable to init SDL_image: %s\n", IMG_GetError());
		throw std::runtime_error("SDL_image");
	}
	if(TTF_Init() == -1) {
		fprintf(stderr, "Unable to init SDL_ttf: %s\n", TTF_GetError());
		throw std::runtime_error("SDL_ttf");
	}
	SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);

	loadTextures();
	loadFont();

	if(!setupScreen()) {
		fprintf(stderr, "Unable to setup screen\n");
		throw std::runtime_error("Setup screen");
	}

	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-o")) {
			mObserver = true;
		}
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

void MatchSDLGUI::play()
{
	double prevTime = Clock::getTime();
	if(mObserver)
		mFreeCamera = true;
	while(!mMatch->matchOver()) {
		double newTime = Clock::getTime();
		double frameTime = newTime - prevTime;
		prevTime = newTime;
		mMatch->update(frameTime);
		if(!mObserver)
			setPlayerController();
		if(handleInput(frameTime))
			break;
		startFrame();
		drawEnvironment();
		drawBall();
		drawPlayers();
		finishFrame();
	}
}

void MatchSDLGUI::drawEnvironment()
{
	drawSprite(*mPitchTexture, Rectangle((-mCamera.x - mMatch->getPitchWidth() * 0.5f) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y - mMatch->getPitchHeight() * 0.5f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * mMatch->getPitchWidth(),
				mScaleLevel * mMatch->getPitchHeight()),
			Rectangle(0, 0, 20, 20), 0);
	std::stringstream result;
	result << mMatch->getScore(false) << " - " << mMatch->getScore(true);
	drawText(10, 10, FontConfig(result.str().c_str(), Color(0, 0, 0), 3.0f), true, false);

	const Team* t = mMatch->getTeam(1);
	for(int j = -mMatch->getPitchHeight() * 0.5f + 8; j < mMatch->getPitchHeight() * 0.5 - 8; j += 8) {
		for(int i = -mMatch->getPitchWidth() * 0.5f + 8; i < mMatch->getPitchWidth() * 0.5 - 8; i += 8) {
			float score = t->getSupportingPositionScoreAt(AbsVector3(i, j, 0));
			int iscore(score * 255.0f);
			char buf[128];
			sprintf(buf, "%d", iscore);
			if(iscore < 0)
				iscore = 0;
			if(iscore > 255)
				iscore = 255;
			drawText(i, j, FontConfig(buf, Color(iscore, iscore, iscore), 0.1f), false, false);
		}
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
			drawSprite(pl->getTeam()->isFirst() ? *mPlayerTextureHome : *mPlayerTextureAway,
					Rectangle((-mCamera.x + v.v.x) * mScaleLevel + screenWidth * 0.5f,
						(-mCamera.y + v.v.y) * mScaleLevel + screenHeight * 0.5f,
						mScaleLevel * 2.0f, mScaleLevel * 2.0f),
					Rectangle(1, 1, -1, -1), 0.1f);
			drawText(v.v.x, v.v.y,
					FontConfig(pl->getAIController()->getDescription().c_str(),
						Color(0, 0, 0), 0.05f), false, false);
		}
	}
}

void MatchSDLGUI::drawBall()
{
	const AbsVector3& v(mMatch->getBall()->getPosition());
	drawSprite(*mBallTexture, Rectangle((-mCamera.x + v.v.x) * mScaleLevel + screenWidth * 0.5f,
				(-mCamera.y + v.v.y) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * 0.6f, mScaleLevel * 0.6f),
			Rectangle(1, 1, -1, -1), 0.1f);
}

void MatchSDLGUI::startFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!mFreeCamera) {
		if(mMatch->getPlayState() == PlayState::InPlay ||
				MatchHelpers::distanceToPitch(*mMatch, mMatch->getBall()->getPosition()) < MAX_KICK_DISTANCE) {
			mCamera.x = mMatch->getBall()->getPosition().v.x;
			mCamera.y = mMatch->getBall()->getPosition().v.y;
		}
	}
}

void MatchSDLGUI::finishFrame()
{
	SDL_GL_SwapBuffers();
}

bool MatchSDLGUI::setupScreen()
{
	GLenum err;
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1, 1, 1, 1);
	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screenWidth, 0, screenHeight, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);

	err = glGetError();
	if(err != GL_NO_ERROR) {
		fprintf(stderr, "GL error: %s (%d)\n",
				GLErrorToString(err),
				err);
		return false;
	}
	return true;
}

void MatchSDLGUI::loadTextures()
{
	mBallTexture = std::shared_ptr<Texture>(new Texture("share/ball1.png", 0, 8));
	SDLSurface surf("share/player1-n.png");
	surf.changePixelColor(Color(255, 0, 0), Color(255, 255, 255));
	mPlayerTextureHome = std::shared_ptr<Texture>(new Texture(surf, 0, 32));
	surf.changePixelColor(Color(255, 255, 255), Color(0, 0, 0));
	mPlayerTextureAway = std::shared_ptr<Texture>(new Texture(surf, 0, 32));
	mPitchTexture = std::shared_ptr<Texture>(new Texture("share/grass1.png", 0, 0));
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
					case SDLK_w:
						if(mFreeCamera)
							mCameraVelocity.y = -1.0f;
						break;
					case SDLK_s:
						if(mFreeCamera)
							mCameraVelocity.y = 1.0f;
						break;
					case SDLK_a:
						if(mFreeCamera)
							mCameraVelocity.x = 1.0f;
						break;
					case SDLK_d:
						if(mFreeCamera)
							mCameraVelocity.x = -1.0f;
						break;
					case SDLK_MINUS:
					case SDLK_KP_MINUS:
					case SDLK_PAGEDOWN:
						mScaleLevelVelocity = -1.0f; break;
					case SDLK_PLUS:
					case SDLK_KP_PLUS:
					case SDLK_PAGEUP:
						mScaleLevelVelocity = 1.0f; break;
					case SDLK_UP:
						mPlayerControlVelocity.y = 1.0f; break;
					case SDLK_DOWN:
						mPlayerControlVelocity.y = -1.0f; break;
					case SDLK_RIGHT:
						mPlayerControlVelocity.x = 1.0f; break;
					case SDLK_LEFT:
						mPlayerControlVelocity.x = -1.0f; break;
					case SDLK_RCTRL:
						mPlayerKickPowerVelocity = 1.0f; break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch(event.key.keysym.sym) {
					case SDLK_w:
					case SDLK_s:
						if(mFreeCamera)
							mCameraVelocity.y = 0.0f;
						break;
					case SDLK_a:
					case SDLK_d:
						if(mFreeCamera)
							mCameraVelocity.x = 0.0f;
						break;
					case SDLK_MINUS:
					case SDLK_KP_MINUS:
					case SDLK_PLUS:
					case SDLK_KP_PLUS:
					case SDLK_PAGEUP:
					case SDLK_PAGEDOWN:
						mScaleLevelVelocity = 0.0f; break;
					case SDLK_UP:
					case SDLK_DOWN:
						mPlayerControlVelocity.y = 0.0f; break;
					case SDLK_RIGHT:
					case SDLK_LEFT:
						mPlayerControlVelocity.x = 0.0f; break;
					case SDLK_RCTRL:
						mPlayerKickPowerVelocity = 0.0f; break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch(event.button.button) {
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
	if(!mPlayerKickPower && mPlayerKickPowerVelocity) {
		mPlayerKickPower = 0.3f;
	}
	mPlayerKickPower += mPlayerKickPowerVelocity * frameTime;
}

const char* MatchSDLGUI::GLErrorToString(GLenum err)
{
	switch(err) {
		case GL_NO_ERROR: return "GL_NO_ERROR";
		case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
		case GL_TABLE_TOO_LARGE: return "GL_TABLE_TOO_LARGE";
	}
	return "unknown error";
}

void MatchSDLGUI::drawSprite(const Texture& t,
		const Rectangle& vertcoords,
		const Rectangle& texcoords, float depth)
{
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

std::shared_ptr<PlayerAction> MatchSDLGUI::act(double time)
{
	float kickpower = 0.0f;
	AbsVector3 toBall = AbsVector3(mMatch->getBall()->getPosition().v - mPlayer->getPosition().v);
	if(mPlayerKickPower && !mPlayerKickPowerVelocity) {
		kickpower = mPlayerKickPower;
		mPlayerKickPower = 0.0f;
	}
	if(!playing(mMatch->getPlayState()) && MatchHelpers::allowedToKick(*mPlayer)) {
		if(toBall.v.length() > MAX_KICK_DISTANCE) {
			return std::shared_ptr<PlayerAction>(new
					RunToPA(AbsVector3(toBall.v.normalized())));
		}
	}
	if(mPlayerControlVelocity.null()) {
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
	if(kickpower) {
		return std::shared_ptr<PlayerAction>(new KickBallPA(AbsVector3(mPlayerControlVelocity * kickpower)));
	}
	else if(playing(mMatch->getPlayState())) {
		if(!(mPlayerKickPower && toBall.v.length() < MAX_KICK_DISTANCE * 0.7f)) {
			return std::shared_ptr<PlayerAction>(new RunToPA(AbsVector3(mPlayerControlVelocity)));
		}
	}
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

void MatchSDLGUI::setPlayerController()
{
	if(playing(mMatch->getMatchHalf())) {
		if(mPlayer->isAIControlled()) {
			mPlayer->setController(this);
			mPlayerKickPower = 0.0f;
			printf("Now controlling\n");
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
			std::shared_ptr<Texture> texture(new Texture(text));
			std::shared_ptr<TextTexture> ttexture(new TextTexture(texture, text->w, text->h));
			auto it2 = mTextMap.insert(std::make_pair(f, ttexture));
			it = it2.first;
			SDL_FreeSurface(text);
		}

	}

	assert(it != mTextMap.end());
	if(screencoordinates) {
		drawSprite(*it->second->mTexture, Rectangle(x, y,
					it->first.mScale * it->second->mWidth,
					it->first.mScale * it->second->mHeight),
				Rectangle(0, 1, 1, -1), 5.0f);
	}
	else {
		drawSprite(*it->second->mTexture, Rectangle((-mCamera.x + x) * mScaleLevel + screenWidth * 0.5f,
					(-mCamera.y + y) * mScaleLevel + screenHeight * 0.5f,
					mScaleLevel * it->first.mScale * it->second->mWidth,
					mScaleLevel * it->first.mScale * it->second->mHeight),
				Rectangle(0, 1, 1, -1), 5.0f);
	}
}

