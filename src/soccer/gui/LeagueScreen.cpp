#include <algorithm>

#include "soccer/Match.h"
#include "soccer/DataExchange.h"
#include "soccer/gui/LeagueScreen.h"

namespace Soccer {

LeagueScreen::LeagueScreen(std::shared_ptr<ScreenManager> sm, std::shared_ptr<StatefulLeague> l)
	: Screen(sm),
	mLeague(l)
{
	addButton("Back",  Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	mNextButton = addButton("Round", Common::Rectangle(0.40f, 0.90f, 0.25f, 0.06f));

	drawTable();

}

void LeagueScreen::drawTable()
{
	float y = 0.09f;
	const float textSize = 0.056f;

	for(auto l : mTableLabels) {
		removeButton(l);
	}
	mTableLabels.clear();

	mTableLabels.push_back(addLabel("Team",    0.05f, y, false, textSize));
	mTableLabels.push_back(addLabel("M",       0.40f, y, false, textSize));
	mTableLabels.push_back(addLabel("W",       0.45f, y, false, textSize));
	mTableLabels.push_back(addLabel("D",       0.50f, y, false, textSize));
	mTableLabels.push_back(addLabel("L",       0.55f, y, false, textSize));
	mTableLabels.push_back(addLabel("F",       0.60f, y, false, textSize));
	mTableLabels.push_back(addLabel("A",       0.65f, y, false, textSize));
	mTableLabels.push_back(addLabel("P",       0.70f, y, false, textSize));
	y += 0.03f;

	const std::map<std::shared_ptr<StatefulTeam>, LeagueEntry>& es = mLeague->getEntries();
	std::vector<std::pair<std::shared_ptr<StatefulTeam>, LeagueEntry>> ves(es.begin(), es.end());
	std::sort(ves.begin(), ves.end(), [](const std::pair<std::shared_ptr<StatefulTeam>, LeagueEntry>& p1,
				const std::pair<std::shared_ptr<StatefulTeam>, LeagueEntry>& p2) {
			if(p1.second.Points != p2.second.Points)
				return p1.second.Points > p2.second.Points;
			int gd1 = p1.second.GoalsFor - p1.second.GoalsAgainst;
			int gd2 = p2.second.GoalsFor - p2.second.GoalsAgainst;
			if(gd1 != gd2)
				return gd1 > gd2;
			if(p1.second.GoalsFor != p2.second.GoalsFor)
				return p1.second.GoalsFor > p2.second.GoalsFor;
			return strcmp(p1.first->getName().c_str(),
				p2.first->getName().c_str()) < 0;
			});

	for(auto e : ves) {
		mTableLabels.push_back(addLabel(e.first->getName().c_str(),                    0.05f, y, false, textSize));
		mTableLabels.push_back(addLabel(std::to_string(e.second.Matches).c_str(),      0.40f, y, false, textSize));
		mTableLabels.push_back(addLabel(std::to_string(e.second.Wins).c_str(),         0.45f, y, false, textSize));
		mTableLabels.push_back(addLabel(std::to_string(e.second.Draws).c_str(),        0.50f, y, false, textSize));
		mTableLabels.push_back(addLabel(std::to_string(e.second.Losses).c_str(),       0.55f, y, false, textSize));
		mTableLabels.push_back(addLabel(std::to_string(e.second.GoalsFor).c_str(),     0.60f, y, false, textSize));
		mTableLabels.push_back(addLabel(std::to_string(e.second.GoalsAgainst).c_str(), 0.65f, y, false, textSize));
		mTableLabels.push_back(addLabel(std::to_string(e.second.Points).c_str(),       0.70f, y, false, textSize));
		y += 0.03f;
	}
}

void LeagueScreen::buttonPressed(std::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreensUntil("Main Menu");
	}
	else if(buttonText == "Round") {
		bool done = mLeague->nextMatch([&](const Match& m) { return playMatch(m); });
		if(done) {
			mNextButton->hide();
		}
		drawTable();
	}
}

MatchResult LeagueScreen::playMatch(const Match& m)
{
	int h = rand() % 4;
	int a = rand() % 4;
	return MatchResult(h, a);
}

const std::string LeagueScreen::ScreenName = std::string("League Screen");

const std::string& LeagueScreen::getName() const
{
	return ScreenName;
}


}


