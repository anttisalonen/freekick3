#include "MatchSDLGUI.h"

#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

static const int screenWidth = 1024;
static const int screenHeight = 768;

MatchSDLGUI::MatchSDLGUI(std::shared_ptr<Match> match)
	: MatchGUI(match)
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
	mClock = Clock();
	while(!mMatch->matchOver()) {
		double frameTime = mClock.limitFPS(60);
		mMatch->update(frameTime);
		if(handleInput())
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
	/* TODO */
}

void MatchSDLGUI::drawPlayers()
{
	glBindTexture(GL_TEXTURE_2D, mPlayerTexture->getTexture());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(100, 100, 0);
	glTexCoord2f(1, 0);
	glVertex3f(300, 100, 0);
	glTexCoord2f(1, 1);
	glVertex3f(300, 300, 0);
	glTexCoord2f(0, 1);
	glVertex3f(100, 300, 0);
	glEnd();
}

void MatchSDLGUI::drawBall()
{
	/* TODO */
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
	glClearColor(0, 0, 0, 1);
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
	mPlayerTexture = std::shared_ptr<Texture>(new Texture("share/player1-n.png"));
}

bool MatchSDLGUI::handleInput()
{
	bool quitting = false;
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					quitting = true;
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
	return quitting;
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

