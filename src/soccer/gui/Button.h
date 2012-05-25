#ifndef SOCCER_BUTTON_H
#define SOCCER_BUTTON_H

#include <string>
#include <memory>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

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

class Button {
	public:
		Button(const char* text, TTF_Font* font, const Common::Rectangle& dim);
		bool clicked(int x, int y) const;
		const std::string& getText() const;
		const Common::Rectangle& getRectangle() const;
		const Common::Texture* getTextTexture() const;
		bool hidden() const;
		void hide();
		void show();
		bool active() const;
		void activate();
		void deactivate();
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

		static Common::Color DefaultColor1;
		static Common::Color DefaultColor2;

	private:
		std::string mText;
		Common::Rectangle mRectangle;
		std::shared_ptr<Common::Texture> mTextTexture;
		bool mHidden;
		bool mActive;
		bool mTransparent;
		TextAlignment mCenteredText;
		Common::Color mColor1;
		Common::Color mColor2;
		float mTextWidth;
		float mTextHeight;
};

}

#endif

