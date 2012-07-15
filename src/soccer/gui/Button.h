#ifndef SOCCER_BUTTON_H
#define SOCCER_BUTTON_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "soccer/gui/Widget.h"

#include "common/Color.h"
#include "common/Texture.h"
#include "common/Rectangle.h"

namespace Soccer {

enum class TextAlignment {
	TopLeft,
	TopMiddle,
	TopRight,
	MiddleLeft,
	Centered,
	MiddleRight,
	BottomLeft,
	BottomMiddle,
	BottomRight
};

class Button : public Widget {
	public:
		Button(const char* text, TTF_Font* font, const Common::Rectangle& dim);
		const std::string& getText() const;
		void setText(const std::string& t);
		const Common::Texture* getTextTexture() const;
		bool isTransparent() const;
		void setTransparent(bool t);
		TextAlignment centeredText() const;
		void setCenteredText(TextAlignment c);
		const Common::Color& getColor1() const;
		const Common::Color& getColor2() const;
		void setColor1(const Common::Color& c);
		void setColor2(const Common::Color& c);
		void setTextWidth(float f);
		void setTextHeight(float f);
		float getTextWidth() const;
		float getTextHeight() const;
		void setTextColor(const Common::Color& c);
		const Common::Color& getTextColor() const;

		static Common::Color DefaultColor1;
		static Common::Color DefaultColor2;

	private:
		void setTexture();
		std::string mText;
		TTF_Font* mFont;
		boost::shared_ptr<Common::Texture> mTextTexture;
		bool mTransparent;
		TextAlignment mCenteredText;
		Common::Color mColor1;
		Common::Color mColor2;
		Common::Color mTextColor;
		float mTextWidth;
		float mTextHeight;
};

}

#endif

