#include <iostream>
#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/SDL_utils.h"

#include "soccer/gui/ScreenManager.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/Menu.h"
#include "soccer/gui/Image.h"

namespace Soccer {

using namespace Common;

static const int screenWidth = 800;
static const int screenHeight = 600;

static const int fontHeight = 24;

ScreenManager::ScreenManager(const Menu& m)
	: mMenu(m),
	mPressedButton(std::string(""))
{
	mScreen = SDL_utils::initSDL(screenWidth, screenHeight);
	SDL_utils::setupOrthoScreen(screenWidth, screenHeight);

	mFont = TTF_OpenFont("share/DejaVuSans.ttf", fontHeight);
	if(!mFont) {
		fprintf(stderr, "Could not open font: %s\n", TTF_GetError());
		throw std::runtime_error("Loading font");
	}

	mBackground = boost::shared_ptr<Texture>(new Texture("share/bg.png", 0, 0));
}

ScreenManager::~ScreenManager()
{
	if(mFont)
		TTF_CloseFont(mFont);
	TTF_Quit();
	SDL_Quit();
}

void ScreenManager::addScreen(boost::shared_ptr<Screen> s)
{
	mScreens.push_back(s);
}

void ScreenManager::dropScreen()
{
	if(mScreens.size())
		mScreens.pop_back();
}

void ScreenManager::dropScreensUntil(const std::string& screenname)
{
	while(mScreens.size()) {
		if(getCurrentScreen()->getName() == screenname)
			break;
		dropScreen();
	}
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

	// draw widgets
	boost::shared_ptr<Screen> currentScreen = getCurrentScreen();
	if(currentScreen) {
		// maybe it would be better to move all this drawing stuff to
		// an abstract Widget::draw, but I want to keep all the gl
		// calls in one place.
		for(auto i : currentScreen->getImages()) {
			if(i->hidden())
				continue;

			const Rectangle& r = i->getRectangle();
			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, i->getTexture()->getTexture());
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(r.x, screenHeight - r.y, 0.5f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(r.x + r.w, screenHeight - r.y, 0.5f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(r.x + r.w, screenHeight - r.y - r.h, 0.5f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(r.x, screenHeight - r.y - r.h, 0.5f);
			glEnd();
		}

		for(auto b : currentScreen->getButtons()) {
			if(b->hidden())
				continue;

			const Rectangle& r = b->getRectangle();
			if(!b->isTransparent()) {
				glDisable(GL_TEXTURE_2D);
				glBegin(GL_QUADS);
				const Color& c1 = b->getColor1();
				const Color& c2 = b->getColor2();
				glColor3ub(c1.r, c1.g, c1.b);
				glVertex3f(r.x, screenHeight - r.y, 1.0f);
				glVertex3f(r.x + r.w, screenHeight - r.y, 1.0f);
				glColor3ub(c2.r, c2.g, c2.b);
				glVertex3f(r.x + r.w, screenHeight - r.y - r.h, 1.0f);
				glVertex3f(r.x, screenHeight - r.y - r.h, 1.0f);
				glEnd();
			}

			float tw2 = 0.5f * b->getTextTexture()->getWidth() * b->getTextWidth();
			float th2 = 0.5f * b->getTextTexture()->getHeight() * b->getTextHeight();

			const Common::Color& textcolor = b->getTextColor();
			glColor3ub(textcolor.r, textcolor.g, textcolor.b);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, b->getTextTexture()->getTexture());
			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f);

			Rectangle textbox(r.x, r.y,
					2.0f * tw2, 2.0f * th2);

			switch(b->centeredText()) {
				case TextAlignment::TopLeft:
					break;

				case TextAlignment::TopMiddle:
					textbox.x += r.w * 0.5f - tw2;
					break;

				case TextAlignment::TopRight:
					textbox.x += r.w - 2.0f * tw2;
					break;

				case TextAlignment::MiddleLeft:
					textbox.y += r.h * 0.5f - th2;
					break;

				case TextAlignment::Centered:
					textbox.x += r.w * 0.5f - tw2;
					textbox.y += r.h * 0.5f - th2;
					break;

				case TextAlignment::MiddleRight:
					textbox.x += r.w - 2.0f * tw2;
					textbox.y += r.h * 0.5f - th2;
					break;

				case TextAlignment::BottomLeft:
					textbox.y += r.h - 2.0f * th2;
					break;

				case TextAlignment::BottomMiddle:
					textbox.x += r.w * 0.5f - tw2;
					textbox.y += r.h - 2.0f * th2;
					break;

				case TextAlignment::BottomRight:
					textbox.x += r.w - 2.0f * tw2;
					textbox.y += r.h - 2.0f * th2;
					break;
			}

			textbox.y = screenHeight - textbox.y;
			glVertex3f(textbox.x,             textbox.y, 1.1f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(textbox.x + textbox.w, textbox.y, 1.1f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(textbox.x + textbox.w, textbox.y - textbox.h, 1.1f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(textbox.x,             textbox.y - textbox.h, 1.1f);

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
				dropScreen();
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
	boost::shared_ptr<Screen> currentScreen = getCurrentScreen();
	if(!currentScreen) {
		return true;
	}

	for(auto b : currentScreen->getButtons()) {
		if(!b->hidden() && b->clicked(x, y)) {
			if(!up) {
				mPressedButton = std::string(b->getText());
			} else {
				if(mPressedButton == b->getText()) {
					currentScreen->buttonPressed(b);
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

boost::shared_ptr<Screen> ScreenManager::getCurrentScreen() const
{
	if(mScreens.empty())
		return boost::shared_ptr<Screen>();
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
