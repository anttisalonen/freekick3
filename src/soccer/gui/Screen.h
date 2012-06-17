#ifndef SOCCER_SCREEN_H
#define SOCCER_SCREEN_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Rectangle.h"

#include "soccer/gui/Button.h"
#include "soccer/gui/ScreenManager.h"

namespace Soccer {

class Screen {
	public:
		Screen(boost::shared_ptr<ScreenManager> sm);
		virtual ~Screen() { }
		const std::vector<boost::shared_ptr<Button>>& getButtons() const;
		virtual void buttonPressed(boost::shared_ptr<Button> button) = 0;
		virtual const std::string& getName() const = 0;

	protected:
		boost::shared_ptr<Button> addButton(const char* text, const Common::Rectangle& dim);
		boost::shared_ptr<Button> addLabel(const char* text, float x, float y,
				TextAlignment centered = TextAlignment::Centered,
				float fsize = 1.0f, Common::Color col = Common::Color::White);
		bool removeButton(boost::shared_ptr<Button> b);
		boost::shared_ptr<ScreenManager> mScreenManager;

	private:
		void setButtonTextSize(boost::shared_ptr<Button> b);
		std::vector<boost::shared_ptr<Button>> mButtons;

};


}

#endif

