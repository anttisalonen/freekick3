#ifndef SOCCER_WIDGET_H
#define SOCCER_WIDGET_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/Color.h"
#include "common/Texture.h"
#include "common/Rectangle.h"

namespace Soccer {

class Widget {
	public:
		Widget(const Common::Rectangle& dim);
		virtual bool clicked(int x, int y);
		const Common::Rectangle& getRectangle() const;
		void setRectangle(const Common::Rectangle& r);
		bool hidden() const;
		bool visible() const;
		void hide();
		void show();
		bool active() const;
		void activate();
		void deactivate();
		virtual void draw(int screenWidth, int screenHeight) = 0;

	protected:
		Common::Rectangle mRectangle;
		bool mHidden;
		bool mActive;
};

}

#endif


