#include "common/Rectangle.h"

#include "soccer/Tournament.h"

#include "soccer/gui/PresetTournamentScreen.h"
#include "soccer/gui/TournamentScreen.h"
#include "soccer/gui/TeamSelectionScreen.h"

using namespace Common;

namespace Soccer {

PresetTournamentScreen::PresetTournamentScreen(boost::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	addButton("UEFA Champions League", Rectangle(0.35f, 0.45f, 0.30f, 0.07f));
	addButton("Back",                  Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
}

void PresetTournamentScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreen();
	}
	else if(buttonText == "UEFA Champions League") {
		std::vector<boost::shared_ptr<Team>> teams = getUEFAChampionsLeagueTeams();
		std::unique_ptr<TournamentConfig> tournamentConfig(new TournamentConfig());;
		auto k1 = boost::shared_ptr<TournamentStage>(new KnockoutStage(1, false, 1));
		auto k2 = boost::shared_ptr<TournamentStage>(new KnockoutStage(2, true, 2));
		auto k3 = boost::shared_ptr<TournamentStage>(new KnockoutStage(2, true, 4));
		auto k4 = boost::shared_ptr<TournamentStage>(new KnockoutStage(2, true, 8));
		tournamentConfig->pushStage(k1);
		tournamentConfig->pushStage(k2);
		tournamentConfig->pushStage(k3);
		tournamentConfig->pushStage(k4);
		tournamentConfig->pushStage(boost::shared_ptr<TournamentStage>(new GroupStage(8, 32, 16, 2)));
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new TeamSelectionScreen(mScreenManager, teams, std::move(tournamentConfig))));
	}
}

const std::string PresetTournamentScreen::ScreenName = "Preset Tournament Screen";

const std::string& PresetTournamentScreen::getName() const
{
	return ScreenName;
}

std::vector<boost::shared_ptr<Team>> PresetTournamentScreen::getUEFAChampionsLeagueTeams()
{
	typedef std::tuple<std::string, std::string> TeamKey;

	std::vector<boost::shared_ptr<Team>> results;

	static const TeamKey teams[] = {
		TeamKey("England", "Chelsea"),
		TeamKey("England", "Manchester City"),
		TeamKey("England", "Manchester Utd"),
		TeamKey("England", "Arsenal"),
		TeamKey("Spain", "Real Madrid"),
		TeamKey("Spain", "Barcelona"),
		TeamKey("Spain", "Valencia"),
		TeamKey("Germany", "Bor. Dortmund"),
		TeamKey("Germany", "Bayern Munchen"),
		TeamKey("Germany", "Schalke"),
		TeamKey("Italy", "Juventus"),
		TeamKey("Italy", "Milan"),
		TeamKey("France", "Montpellier"),
		TeamKey("France", "Paris St-Germain"),
		TeamKey("Portugal", "Fc Porto"),
		TeamKey("Portugal", "Benfica"),
		TeamKey("Russia", "Zenit St. P'Burg"),
		TeamKey("Ukraine", "Shakhtar Donetsk"),
		TeamKey("Netherlands", "Ajax"),
		TeamKey("Turkey", "Galatasaray"),
		TeamKey("Greece", "Olympiakos"),
		TeamKey("Denmark", "Nordsjaelland"),
		TeamKey("Spain", "Malaga"),
		TeamKey("Romania", "Cfr Cluj"),
		TeamKey("Scotland", "Celtic"),
		TeamKey("France", "Lille"),
		TeamKey("Portugal", "Braga"),
		TeamKey("Russia", "Spartak Moskva"),
		TeamKey("Ukraine", "Dynamo Kyiv"),
		TeamKey("Croatia", "Dinamo Zagreb"),
		TeamKey("Belgium", "Anderlecht"),
		TeamKey("Belarus", "Bate Borisov"),
       	};

	auto tdb = mScreenManager->getTeamDatabase();
	auto eur = tdb.getOrCreateContinent("Europe");

	for(auto& tk : teams) {
		bool found = false;
		auto ls = eur->getT(std::get<0>(tk));
		if(ls) {
			auto leagues = ls->getContainer();
			for(auto& l : leagues) {
				if(l.second->getLevel() == 0) {
					auto team = l.second->getT(std::get<1>(tk));
					if(team) {
						results.push_back(team);
						found = true;
						break;
					}
				}
			}
		}
		if(!found) {
			std::cerr << "Warning: couldn't find team " << std::get<1>(tk) << " in " << std::get<0>(tk) << "\n";
		}
	}
	return results;
}

}

