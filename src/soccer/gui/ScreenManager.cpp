#include <iostream>
#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/SDL_utils.h"

#include "soccer/gui/ScreenManager.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/Menu.h"

namespace Soccer {

using namespace Common;

static const int screenWidth = 800;
static const int screenHeight = 600;

ScreenManager::ScreenManager(const Menu& m)
	: mMenu(m),
	mPressedButton(std::string(""))
{
	mScreen = SDL_utils::initSDL(screenWidth, screenHeight);
	SDL_utils::setupOrthoScreen(screenWidth, screenHeight);

	mFont = TTF_OpenFont("share/DejaVuSans.ttf", 12);
	if(!mFont) {
		fprintf(stderr, "Could not open font: %s\n", TTF_GetError());
		throw std::runtime_error("Loading font");
	}

	mBackground = std::shared_ptr<Texture>(new Texture("share/bg.png", 0, 0));
}

ScreenManager::~ScreenManager()
{
	if(mFont)
		TTF_CloseFont(mFont);
	TTF_Quit();
	SDL_Quit();
}

void ScreenManager::addScreen(std::shared_ptr<Screen> s)
{
	mScreens.push_back(s);
}

void ScreenManager::dropScreen()
{
	if(mScreens.size())
		mScreens.pop_back();
}

void ScreenManager::drawScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw background
	glColor3f(0.5f, 0.5f, 0.5f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mBackground->getTexture());
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, screenHeight, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(screenWidth, screenHeight, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(screenWidth, 0.0f, 0.0f);
	glEnd();

	// draw buttons
	std::shared_ptr<Screen> currentScreen = getCurrentScreen();
	if(currentScreen) {
		for(auto b : currentScreen->getButtons()) {
			const Rectangle& r = b->getRectangle();
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glColor3f(0.85f, 0.75f, 0.50f);
			glVertex3f(r.x, screenHeight - r.y, 1.0f);
			glVertex3f(r.x + r.w, screenHeight - r.y, 0.0f);
			glColor3f(0.75, 0.50f, 0.45f);
			glVertex3f(r.x + r.w, screenHeight - r.y - r.h, 0.0f);
			glVertex3f(r.x, screenHeight - r.y - r.h, 0.0f);
			glEnd();

			float tw2 = b->getTexture()->getWidth() * 0.8f;
			float th2 = b->getTexture()->getHeight() * 0.8f;

			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, b->getTexture()->getTexture());
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(r.x + r.w * 0.5f - tw2, screenHeight - r.y - r.h * 0.5f + th2, 1.1f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(r.x + r.w * 0.5f + tw2, screenHeight - r.y - r.h * 0.5f + th2, 1.1f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(r.x + r.w * 0.5f + tw2, screenHeight - r.y - r.h * 0.5f - th2, 1.1f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(r.x + r.w * 0.5f - tw2, screenHeight - r.y - r.h * 0.5f - th2, 1.1f);
			glEnd();

		}
	}

	SDL_GL_SwapBuffers();
}

bool ScreenManager::handleEvents()
{
	SDL_Event ev;
	int ret = SDL_WaitEvent(&ev);
	if(!ret) {
		fprintf(stderr, "Error in SDL_WaitEvent: %s\n", SDL_GetError());
		clearScreens();
		return true;
	}
	switch(ev.type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if(ev.button.button == SDL_BUTTON_LEFT)
				return recordMouseButton(ev.type == SDL_MOUSEBUTTONUP, ev.button.x, ev.button.y);

		case SDL_KEYDOWN:
			if(ev.key.keysym.sym == SDLK_ESCAPE)
			{
				std::shared_ptr<Screen> currentScreen = getCurrentScreen();
				if(currentScreen)
					currentScreen->buttonPressed(std::string("Back"));
			}
			return true;

		case SDL_QUIT:
			clearScreens();
			return true;

		case SDL_VIDEORESIZE:
		case SDL_VIDEOEXPOSE:
			return true;

		default:
			return false;
	}

	return false;
}

bool ScreenManager::recordMouseButton(bool up, int x, int y)
{
	std::shared_ptr<Screen> currentScreen = getCurrentScreen();
	if(!currentScreen) {
		return true;
	}

	for(auto b : currentScreen->getButtons()) {
		if(b->clicked(x, y)) {
			if(!up) {
				mPressedButton = std::string(b->getText());
			} else {
				if(mPressedButton == b->getText()) {
					currentScreen->buttonPressed(mPressedButton);
					mPressedButton = std::string("");
					return true;
				}
				mPressedButton = std::string("");
			}
			break;
		}
	}
	return false;
}

bool ScreenManager::isRunning() const
{
	return !mScreens.empty();
}

std::shared_ptr<Screen> ScreenManager::getCurrentScreen() const
{
	if(mScreens.empty())
		return nullptr;
	else
		return mScreens[mScreens.size() - 1];
}

void ScreenManager::clearScreens()
{
	mScreens.clear();
}

TTF_Font* ScreenManager::getFont()
{
	return mFont;
}

int ScreenManager::getScreenWidth() const
{
	return screenWidth;
}

int ScreenManager::getScreenHeight() const
{
	return screenHeight;
}

const TeamDatabase& ScreenManager::getTeamDatabase() const
{
	return mMenu.getTeamDatabase();
}

const PlayerDatabase& ScreenManager::getPlayerDatabase() const
{
	return mMenu.getPlayerDatabase();
}


}
