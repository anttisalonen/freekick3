#include <algorithm>
#include <fstream>

#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "soccer/Match.h"
#include "soccer/DataExchange.h"
#include "soccer/gui/Menu.h"
#include "soccer/gui/LeagueScreen.h"

namespace Soccer {

LeagueScreen::LeagueScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulLeague> l)
	: Screen(sm),
	mLeague(l),
	mTextSize(0.048f),
	mMyTeamColor(128, 128, 255)
{
	addButton("Back",  Common::Rectangle(0.01f, 0.90f, 0.23f, 0.06f));
	addButton("Save",   Common::Rectangle(0.01f, 0.83f, 0.23f, 0.06f));
	mSkipButton   = addButton("Skip",   Common::Rectangle(0.26f, 0.90f, 0.23f, 0.06f));
	mResultButton = addButton("Result", Common::Rectangle(0.51f, 0.90f, 0.23f, 0.06f));
	mMatchButton  = addButton("Match",  Common::Rectangle(0.76f, 0.90f, 0.23f, 0.06f));

	updateRoundMatches();
	updateScreenElements();
}

void LeagueScreen::addText(LabelType t, const char* text, float x, float y,
		TextAlignment align, Common::Color col)
{
	std::vector<boost::shared_ptr<Button>>* bts = nullptr;
	switch(t) {
		case LabelType::Table:
			bts = &mTableLabels;
			break;

		case LabelType::Result:
			bts = &mResultLabels;
			break;
	}

	bts->push_back(addLabel(text, x, y, align, 0.6f, col));
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
	addText(LabelType::Table, "W",       0.29f, y);
	addText(LabelType::Table, "D",       0.33f, y);
	addText(LabelType::Table, "L",       0.37f, y);
	addText(LabelType::Table, "GF",      0.41f, y);
	addText(LabelType::Table, "GA",      0.45f, y);
	addText(LabelType::Table, "GD",      0.49f, y);
	addText(LabelType::Table, "P",       0.53f, y);
	y += 0.03f;

	const std::map<boost::shared_ptr<StatefulTeam>, LeagueEntry>& es = mLeague->getEntries();
	std::vector<std::pair<boost::shared_ptr<StatefulTeam>, LeagueEntry>> ves(es.begin(), es.end());
	std::sort(ves.begin(), ves.end(), [](const std::pair<boost::shared_ptr<StatefulTeam>, LeagueEntry>& p1,
				const std::pair<boost::shared_ptr<StatefulTeam>, LeagueEntry>& p2) -> bool {
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
		const Common::Color& textColor = e.first->getController().HumanControlled ?
			mMyTeamColor : Common::Color::White;
		addText(LabelType::Table, e.first->getName().c_str(),                    0.05f, y,
				TextAlignment::MiddleLeft, textColor);
		addText(LabelType::Table, std::to_string(e.second.Matches).c_str(),      0.25f, y);
		addText(LabelType::Table, std::to_string(e.second.Wins).c_str(),         0.29f, y);
		addText(LabelType::Table, std::to_string(e.second.Draws).c_str(),        0.33f, y);
		addText(LabelType::Table, std::to_string(e.second.Losses).c_str(),       0.37f, y);
		addText(LabelType::Table, std::to_string(e.second.GoalsFor).c_str(),     0.41f, y);
		addText(LabelType::Table, std::to_string(e.second.GoalsAgainst).c_str(), 0.45f, y);
		addText(LabelType::Table, std::to_string(e.second.GoalsFor - e.second.GoalsAgainst).c_str(),
											 0.49f, y);
		addText(LabelType::Table, std::to_string(e.second.Points).c_str(),       0.53f, y);
		y += 0.03f;
	}
}

void LeagueScreen::addMatchLabels(const Match& m, float xp, float yp)
{
	const Common::Color& textColor1 = m.getTeam(0)->getController().HumanControlled ?
		mMyTeamColor : Common::Color::White;
	const Common::Color& textColor2 = m.getTeam(1)->getController().HumanControlled ?
		mMyTeamColor : Common::Color::White;
	addText(LabelType::Result, m.getTeam(0)->getName().c_str(), xp - 0.02f, yp,
			TextAlignment::MiddleRight, textColor1);
	addText(LabelType::Result, " - ", xp, yp, TextAlignment::Centered);
	addText(LabelType::Result, m.getTeam(1)->getName().c_str(), xp + 0.02f, yp,
			TextAlignment::MiddleLeft, textColor2);
	if(m.getResult().Played) {
		addText(LabelType::Result, std::to_string(m.getResult().HomeGoals).c_str(), 0.74f, yp,
				TextAlignment::Centered);
		addText(LabelType::Result, std::to_string(m.getResult().AwayGoals).c_str(), 0.76f, yp,
				TextAlignment::Centered);
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
		// next round
		addMatchLabels(*m, 0.75f, y);
		y += 0.03f;
	}

	{
		// next match
		boost::shared_ptr<Match> m = mLeague->getNextMatch();
		if(m) {
			addMatchLabels(*m, 0.50f, 0.88f);
		}
	}
}

void LeagueScreen::updateRoundMatches()
{
	mRoundMatches.clear();
	const Round* r = mLeague->getCurrentRound();
	if(r) {
		mRoundMatches = r->getMatches();
	}
}

bool LeagueScreen::playNextMatch(bool display)
{
	if(allRoundMatchesPlayed()) {
		updateRoundMatches();
	}
	bool done = mLeague->nextMatch([&](const Match& m) { return playMatch(display, m); });
	return done;
}

void LeagueScreen::updateScreenElements()
{
	const boost::shared_ptr<Match> m = mLeague->getNextMatch();
	if(!m) {
		mSkipButton->hide();
		mResultButton->hide();
		mMatchButton->hide();
	}
	else {
		mSkipButton->hide();
		if(shouldShowSkipButton())
			mSkipButton->show();
	}
	drawTable();
	drawInfo();
}

bool LeagueScreen::shouldShowSkipButton() const
{
	const boost::shared_ptr<Match> m = mLeague->getNextMatch();
	return m && !(m->getTeam(0)->getController().HumanControlled ||
				m->getTeam(1)->getController().HumanControlled);
}

void LeagueScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreensUntil("Main Menu");
	}
	else if(buttonText == "Save") {
		saveLeague();
	}
	else if(buttonText == "Skip") {
		while(shouldShowSkipButton()) {
			bool done = playNextMatch(false);
			if(done)
				break;
		}
		updateScreenElements();
	}
	else if(buttonText == "Result") {
		playNextMatch(false);
		updateScreenElements();
	}
	else if(buttonText == "Match") {
		playNextMatch(true);
		updateScreenElements();
	}
}

MatchResult LeagueScreen::playMatch(bool display, const Match& m)
{
	const MatchResult& r = m.play(display);
	return r;
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

void LeagueScreen::saveLeague() const
{
	std::string filename(Menu::getSaveDir());
	filename += "/League.sav";
	std::ofstream ofs(filename, std::ios::out | std::ios::binary | std::ios::trunc);
	boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
	out.push(boost::iostreams::bzip2_compressor());
	out.push(ofs);
	boost::archive::binary_oarchive oa(out);
	oa << mLeague;
}

}


