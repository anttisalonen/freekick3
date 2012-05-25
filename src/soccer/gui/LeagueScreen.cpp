#include <algorithm>

#include "soccer/Match.h"
#include "soccer/DataExchange.h"
#include "soccer/gui/LeagueScreen.h"

namespace Soccer {

LeagueScreen::LeagueScreen(std::shared_ptr<ScreenManager> sm, std::shared_ptr<StatefulLeague> l)
	: Screen(sm),
	mLeague(l),
	mTextSize(0.048f)
{
	addButton("Back",  Common::Rectangle(0.02f, 0.90f, 0.20f, 0.06f));
	mResultButton = addButton("Result", Common::Rectangle(0.24f, 0.90f, 0.20f, 0.06f));

	drawTable();
	drawInfo();
}

void LeagueScreen::addText(LabelType t, const char* text, float x, float y,
		TextAlignment align)
{
	std::vector<std::shared_ptr<Button>>* bts = nullptr;
	switch(t) {
		case LabelType::Table:
			bts = &mTableLabels;
			break;

		case LabelType::Result:
			bts = &mResultLabels;
			break;
	}

	bts->push_back(addLabel(text, x, y, align, 0.6f));
}

void LeagueScreen::drawTable()
{
	float y = 0.09f;

	for(auto l : mTableLabels) {
		removeButton(l);
	}
	mTableLabels.clear();

	addText(LabelType::Table, "Team",    0.05f, y);
	addText(LabelType::Table, "M",       0.25f, y);
	addText(LabelType::Table, "W",       0.30f, y);
	addText(LabelType::Table, "D",       0.35f, y);
	addText(LabelType::Table, "L",       0.40f, y);
	addText(LabelType::Table, "F",       0.45f, y);
	addText(LabelType::Table, "A",       0.50f, y);
	addText(LabelType::Table, "P",       0.55f, y);
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
		addText(LabelType::Table, e.first->getName().c_str(),                    0.05f, y);
		addText(LabelType::Table, std::to_string(e.second.Matches).c_str(),      0.25f, y);
		addText(LabelType::Table, std::to_string(e.second.Wins).c_str(),         0.30f, y);
		addText(LabelType::Table, std::to_string(e.second.Draws).c_str(),        0.35f, y);
		addText(LabelType::Table, std::to_string(e.second.Losses).c_str(),       0.40f, y);
		addText(LabelType::Table, std::to_string(e.second.GoalsFor).c_str(),     0.45f, y);
		addText(LabelType::Table, std::to_string(e.second.GoalsAgainst).c_str(), 0.50f, y);
		addText(LabelType::Table, std::to_string(e.second.Points).c_str(),       0.55f, y);
		y += 0.03f;
	}
}

void LeagueScreen::drawInfo()
{
	for(auto l : mResultLabels) {
		removeButton(l);
	}
	mResultLabels.clear();

	float y = 0.1f;
	for(auto m : mRoundMatches) {
		addText(LabelType::Result, m->getTeam(0)->getName().c_str(), 0.73f, y, TextAlignment::MiddleRight);
		addText(LabelType::Result, " - ", 0.75f, y, TextAlignment::Centered);
		addText(LabelType::Result, m->getTeam(1)->getName().c_str(), 0.77f, y);
		if(m->getResult().Played) {
			addText(LabelType::Result, std::to_string(m->getResult().HomeGoals).c_str(), 0.74f, y,
					TextAlignment::Centered);
			addText(LabelType::Result, std::to_string(m->getResult().AwayGoals).c_str(), 0.76f, y,
					TextAlignment::Centered);
		}
		y += 0.03f;
	}
}

void LeagueScreen::buttonPressed(std::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreensUntil("Main Menu");
	}
	else if(buttonText == "Result") {
		if(allRoundMatchesPlayed()) {
			mRoundMatches.clear();
			const Round* r = mLeague->getCurrentRound();
			if(r) {
				mRoundMatches = r->getMatches();
			}
		}
		bool done = mLeague->nextMatch([&](const Match& m) { return playMatch(m); });
		if(done) {
			mResultButton->hide();
		}
		drawTable();
		drawInfo();
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

bool LeagueScreen::allRoundMatchesPlayed() const
{
	for(auto m : mRoundMatches) {
		if(!m->getResult().Played) {
			return false;
		}
	}
	return true;
}

}


