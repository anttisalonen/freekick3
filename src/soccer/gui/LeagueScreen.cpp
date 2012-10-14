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

void LeagueScreen::addTableText(const char* text, float x, float y,
		TextAlignment align, Common::Color col)
{
	mTableLabels.push_back(addLabel(text, x, y, align, 0.6f, col));
}

void LeagueScreen::drawTable()
{
	float y = 0.09f;

	for(auto l : mTableLabels) {
		removeButton(l);
	}
	mTableLabels.clear();

	addTableText("Team",    0.05f, y);
	addTableText("M",       0.25f, y);
	addTableText("W",       0.29f, y);
	addTableText("D",       0.33f, y);
	addTableText("L",       0.37f, y);
	addTableText("GF",      0.41f, y);
	addTableText("GA",      0.45f, y);
	addTableText("GD",      0.49f, y);
	addTableText("P",       0.53f, y);
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
		const Common::Color textColor = e.first->getController().HumanControlled ?
			Common::Color(128, 128, 255) : Common::Color::White;
		addTableText(e.first->getName().c_str(),                    0.05f, y,
				TextAlignment::MiddleLeft, textColor);
		addTableText(std::to_string(e.second.Matches).c_str(),      0.25f, y);
		addTableText(std::to_string(e.second.Wins).c_str(),         0.29f, y);
		addTableText(std::to_string(e.second.Draws).c_str(),        0.33f, y);
		addTableText(std::to_string(e.second.Losses).c_str(),       0.37f, y);
		addTableText(std::to_string(e.second.GoalsFor).c_str(),     0.41f, y);
		addTableText(std::to_string(e.second.GoalsAgainst).c_str(), 0.45f, y);
		addTableText(std::to_string(e.second.GoalsFor - e.second.GoalsAgainst).c_str(),
											 0.49f, y);
		addTableText(std::to_string(e.second.Points).c_str(),       0.53f, y);
		y += 0.03f;
	}
}

}


