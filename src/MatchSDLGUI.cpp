#include "MatchSDLGUI.h"

#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

static const int screenWidth = 800;
static const int screenHeight = 600;

MatchSDLGUI::MatchSDLGUI(std::shared_ptr<Match> match)
	: MatchGUI(match),
	mScaleLevel(10.0f),
	mScaleLevelVelocity(0.0f)
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

	if(!setupScreen()) {
		fprintf(stderr, "Unable to setup screen\n");
		throw std::runtime_error("Setup screen");
	}
}

MatchSDLGUI::~MatchSDLGUI()
{
	TTF_Quit();
	SDL_Quit();
}

void MatchSDLGUI::play()
{
	double prevTime = Clock::getTime();
	while(!mMatch->matchOver()) {
		double newTime = Clock::getTime();
		double frameTime = newTime - prevTime;
		prevTime = newTime;
		mMatch->update(frameTime);
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
	drawSprite(*mPitchTexture, Rectangle((mCamera.x - mMatch->getPitchWidth() * 0.5f) * mScaleLevel + screenWidth * 0.5f,
				(mCamera.y - mMatch->getPitchHeight() * 0.5f) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * mMatch->getPitchWidth(),
				mScaleLevel * mMatch->getPitchHeight()),
			Rectangle(0, 0, 20, 20), 0);
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
			drawSprite(*mPlayerTexture, Rectangle((mCamera.x + v.v.x) * mScaleLevel + screenWidth * 0.5f,
						(mCamera.y + v.v.y) * mScaleLevel + screenHeight * 0.5f,
						mScaleLevel, mScaleLevel),
					Rectangle(1, 1, -1, -1), 0.1f);
		}
	}
}

void MatchSDLGUI::drawBall()
{
	const AbsVector3& v(mMatch->getBall()->getPosition());
	drawSprite(*mBallTexture, Rectangle((mCamera.x + v.v.x) * mScaleLevel + screenWidth * 0.5f,
				(mCamera.y + v.v.y) * mScaleLevel + screenHeight * 0.5f,
				mScaleLevel * 0.3f, mScaleLevel * 0.3f),
			Rectangle(1, 1, -1, -1), 0.1f);
}

void MatchSDLGUI::startFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	mPlayerTexture = std::shared_ptr<Texture>(new Texture("share/player1-n.png", 0, 32));
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
						mCameraVelocity.y -= 1.0f; break;
					case SDLK_s:
						mCameraVelocity.y += 1.0f; break;
					case SDLK_a:
						mCameraVelocity.x += 1.0f; break;
					case SDLK_d:
						mCameraVelocity.x -= 1.0f; break;
					case SDLK_MINUS:
					case SDLK_KP_MINUS:
						mScaleLevelVelocity -= 1.0f; break;
					case SDLK_PLUS:
					case SDLK_KP_PLUS:
						mScaleLevelVelocity += 1.0f; break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch(event.key.keysym.sym) {
					case SDLK_w:
					case SDLK_s:
						mCameraVelocity.y = 0.0f; break;
					case SDLK_a:
					case SDLK_d:
						mCameraVelocity.x = 0.0f; break;
					case SDLK_MINUS:
					case SDLK_KP_MINUS:
					case SDLK_PLUS:
					case SDLK_KP_PLUS:
						mScaleLevelVelocity = 0.0f; break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
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
	mCamera += mCameraVelocity * frameTime * 10.0f;
	mScaleLevel += mScaleLevelVelocity * frameTime * 10.0f;
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

