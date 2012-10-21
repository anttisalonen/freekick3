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

LeagueScreen::LeagueScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulLeague> l,
		bool onlyOneRound)
	: CompetitionScreen(sm, "League", l, onlyOneRound),
	mLeague(l)
{
	updateScreenElements();
}

void LeagueScreen::saveCompetition(boost::archive::binary_oarchive& oa) const
{
	oa << mLeague;
}

void LeagueScreen::addTableText(Screen& scr, const char* text, float x, float y,
		TextAlignment align, Common::Color col, std::vector<boost::shared_ptr<Button>>& labels)
{
	labels.push_back(scr.addLabel(text, x, y, align, 0.6f, col));
}

void LeagueScreen::drawTable(Screen& scr, std::vector<boost::shared_ptr<Button>>& labels, const StatefulLeague& l, float x, float y)
{
	x -= 0.05f;

	addTableText(scr, "Team",    x + 0.05f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "M",       x + 0.25f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "W",       x + 0.29f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "D",       x + 0.33f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "L",       x + 0.37f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "GF",      x + 0.41f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "GA",      x + 0.45f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "GD",      x + 0.49f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	addTableText(scr, "P",       x + 0.53f, y, TextAlignment::MiddleLeft, Common::Color::White, labels);
	y += 0.03f;

	std::vector<boost::shared_ptr<StatefulTeam>> es = l.getTeamsByPosition();

	for(auto e : es) {
		const auto& le = l.getEntries().find(e);
		assert(le != l.getEntries().end());

		const Common::Color textColor = e->getController().HumanControlled ?
			Common::Color(128, 128, 255) : Common::Color::White;
		addTableText(scr, e->getName().c_str(),                    x + 0.05f, y,
				TextAlignment::MiddleLeft, textColor, labels);
		addTableText(scr, std::to_string(le->second.Matches).c_str(),      x + 0.25f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		addTableText(scr, std::to_string(le->second.Wins).c_str(),         x + 0.29f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		addTableText(scr, std::to_string(le->second.Draws).c_str(),        x + 0.33f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		addTableText(scr, std::to_string(le->second.Losses).c_str(),       x + 0.37f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		addTableText(scr, std::to_string(le->second.GoalsFor).c_str(),     x + 0.41f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		addTableText(scr, std::to_string(le->second.GoalsAgainst).c_str(), x + 0.45f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		addTableText(scr, std::to_string(le->second.GoalsFor - le->second.GoalsAgainst).c_str(), x + 0.49f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		addTableText(scr, std::to_string(le->second.Points).c_str(),       x + 0.53f, y,
				TextAlignment::MiddleLeft, Common::Color::White, labels);
		y += 0.03f;
	}
}

bool LeagueScreen::drawTable()
{
	for(auto lbl : mTableLabels) {
		removeButton(lbl);
	}
	mTableLabels.clear();

	drawTable(*this, mTableLabels, *mLeague, 0.05f, 0.09f);
	return true;
}

}


