#ifndef SOCCER_SCREEN_H
#define SOCCER_SCREEN_H

#include <string>
#include <memory>
#include <vector>

#include "common/Rectangle.h"

#include "soccer/gui/Button.h"
#include "soccer/gui/ScreenManager.h"

namespace Soccer {

class Screen {
	public:
		Screen(std::shared_ptr<ScreenManager> sm);
		virtual ~Screen() { }
		const std::vector<std::shared_ptr<Button>>& getButtons() const;
		virtual void buttonPressed(const std::string& buttonText) = 0;
		
	protected:
		void addButton(const char* text, const Common::Rectangle& dim);
		std::shared_ptr<ScreenManager> mScreenManager;

	private:
		std::vector<std::shared_ptr<Button>> mButtons;

};


}

#endif

