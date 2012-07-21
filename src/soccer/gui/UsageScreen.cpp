#include "soccer/gui/UsageScreen.h"

namespace Soccer {

using namespace Common;

UsageScreen::UsageScreen(boost::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	addLabel("Blue",                  0.20f, 0.15f, TextAlignment::MiddleLeft, 1.0f, Color(128, 128, 255));
	addLabel("button: Human",         0.30f, 0.15f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Red",                   0.20f, 0.20f, TextAlignment::MiddleLeft, 1.0f, Color(255, 128, 128));
	addLabel("button: Computer",      0.30f, 0.20f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));

	addLabel("W, A, S, D",            0.15f, 0.35f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Mouse",                 0.15f, 0.40f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Left mouse button",     0.15f, 0.45f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Right mouse button",    0.15f, 0.50f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Space",                 0.15f, 0.55f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("C",                     0.15f, 0.60f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));

	addLabel("Run",                   0.60f, 0.35f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Aim kick",              0.60f, 0.40f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Pass/dribble/low shot", 0.60f, 0.45f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Long ball/high shot",   0.60f, 0.50f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Tackle/jump",           0.60f, 0.55f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));
	addLabel("Switch camera",         0.60f, 0.60f, TextAlignment::MiddleLeft, 1.0f, Color(255, 255, 255));

	addButton("Back",          Rectangle(0.35f, 0.90f, 0.30f, 0.07f));
}

void UsageScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreen();
	}
}

const std::string UsageScreen::ScreenName = std::string("Usage");

const std::string& UsageScreen::getName() const
{
	return ScreenName;
}

}
