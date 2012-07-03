#include <algorithm>

#include "soccer/gui/TeamTacticsScreen.h"
#include "soccer/gui/Menu.h"

namespace Soccer {

TeamTacticsScreen::TeamTacticsScreen(boost::shared_ptr<ScreenManager> sm, Match& m,
			TeamTacticsScreenOwner& ttso)
	: Screen(sm),
	mMatch(m),
	mTtso(ttso),
	mChosenplnum(0),
	mShowHome(true),
	mHumanTeam(-1)
{
	addButton("Back",   Common::Rectangle(0.01f, 0.90f, 0.23f, 0.06f));
	mHomeButton = addButton("Home",  Common::Rectangle(0.51f, 0.90f, 0.23f, 0.06f));
	mAwayButton = addButton("Away",  Common::Rectangle(0.51f, 0.90f, 0.23f, 0.06f));
	mHomeButton->hide();
	mHomeLabel = addButton(m.getTeam(0)->getName().c_str(), Common::Rectangle(0.40f, 0.05f, 0.23f, 0.06f));
	mAwayLabel = addButton(m.getTeam(1)->getName().c_str(), Common::Rectangle(0.40f, 0.05f, 0.23f, 0.06f));
	mAwayLabel->hide();
	addButton("Match",  Common::Rectangle(0.76f, 0.90f, 0.23f, 0.06f));

	for(int i = 0; i < 2; i++) {
		float x = 0.05f;
		float y = 0.15f;
		if(m.getTeam(i)->getController().HumanControlled) {
			if(mHumanTeam != -1) {
				// both teams are human-controlled - don't allow choosing anything.
				mHumanTeam = -1;
				mChosenplnum = 0;
			}
			else {
				mHumanTeam = i;
				mChosenplnum = m.getTeam(i)->getController().PlayerShirtNumber;
			}
		}
		int plnum = 0;
		for(auto p : m.getTeam(i)->getTactics().mTactics) {
			plnum++;
			auto pl = m.getTeam(i)->getPlayerById(p.first);
			assert(pl);
			boost::shared_ptr<Button> b = addButton(pl->getName().c_str(), Common::Rectangle(x, y, 0.20f, 0.03f));
			b->setCenteredText(TextAlignment::MiddleLeft);
			mPlrlabels[i].insert(std::make_pair(b, plnum));
			y += 0.04f;
		}
	}

	setupPlrLabels();
	if(mHumanTeam == 1) {
		buttonPressed(mAwayButton);
	}
}

void TeamTacticsScreen::setupPlrLabels()
{
	for(int i = 0; i < 2; i++) {
		for(auto l : mPlrlabels[i]) {
			if((mHomeButton->hidden()) == (i == 0))
				l.first->show();
			else
				l.first->hide();

			if(mHumanTeam == i) {
				if(l.second == mChosenplnum || mChosenplnum == 0)
					Menu::setButtonHumanColor(l.first);
				else
					Menu::setButtonComputerColor(l.first);
			}
			else {
				Menu::setButtonComputerColor(l.first);
			}
		}
	}
}

void TeamTacticsScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreen();
	}
	else if(buttonText == "Home") {
		mHomeButton->hide();
		mAwayButton->show();
		mHomeLabel->show();
		mAwayLabel->hide();
		setupPlrLabels();
	}
	else if(buttonText == "Away") {
		mHomeButton->show();
		mAwayButton->hide();
		mHomeLabel->hide();
		mAwayLabel->show();
		setupPlrLabels();
	}
	else if(buttonText == "Match") {
		mTtso.TeamTacticsScreenFinished(mChosenplnum);
	}
	else {
		if(mHumanTeam != -1) {
			auto it = mPlrlabels[mHumanTeam].find(button);
			if(it != mPlrlabels[mHumanTeam].end()) {
				if(mChosenplnum != 0 && mChosenplnum == it->second)
					mChosenplnum = 0;
				else
					mChosenplnum = it->second;
				setupPlrLabels();
			}
		}
	}
}

const std::string TeamTacticsScreen::ScreenName = std::string("Team Tactics Screen");

const std::string& TeamTacticsScreen::getName() const
{
	return ScreenName;
}

}


