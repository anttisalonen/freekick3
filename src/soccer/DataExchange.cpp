#include <string>
#include <stdexcept>

#include "common/Color.h"

#include "soccer/DataExchange.h"
#include "soccer/Match.h"
#include "soccer/Player.h"

namespace Soccer {

static float getPlayerSkill(const TiXmlElement* skillselem, const char* skillname)
{
	const TiXmlElement* skillelem = skillselem->FirstChildElement(skillname);
	if(!skillelem)
		throw std::runtime_error("Error parsing player skills");
	const char* t = skillelem->GetText();
	if(!t)
		throw std::runtime_error("Error parsing player skills");
	return atof(t);
}

boost::shared_ptr<Player> DataExchange::parsePlayer(const TiXmlElement* pelem)
{
	PlayerSkills sk;
	int id;

	if(pelem->QueryIntAttribute("id", &id) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing player ID");

	const TiXmlElement* skillselem = pelem->FirstChildElement("Skills");
	const TiXmlElement* nameelem = pelem->FirstChildElement("Name");
	if(!skillselem || !nameelem)
		throw std::runtime_error("Error parsing player skill/name");

	const char* name = nameelem->GetText();
	if(!name)
		throw std::runtime_error("Error parsing player name");

	sk.ShotPower = getPlayerSkill(skillselem, "ShotPower");
	sk.RunSpeed = getPlayerSkill(skillselem, "RunSpeed");
	sk.BallControl = getPlayerSkill(skillselem, "BallControl");
	sk.Passing = getPlayerSkill(skillselem, "Passing");
	sk.Tackling = getPlayerSkill(skillselem, "Tackling");
	sk.Heading = getPlayerSkill(skillselem, "Heading");
	sk.GoalKeeping = getPlayerSkill(skillselem, "GoalKeeping");
	return boost::shared_ptr<Player>(new Player(id, name, sk));
}

boost::shared_ptr<Team> DataExchange::parseTeam(const TiXmlElement* teamelem)
{
	int id;

	if(teamelem->QueryIntAttribute("id", &id) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing team ID");

	const TiXmlElement* nameelem = teamelem->FirstChildElement("Name");
	const TiXmlElement* playerselem = teamelem->FirstChildElement("Players");
	if(!playerselem || !nameelem)
		throw std::runtime_error("Error parsing team Name/Players");

	const char* name = nameelem->GetText();
	if(!name)
		throw std::runtime_error("Error parsing team name");

	std::vector<int> playerids;
	std::vector<boost::shared_ptr<Player>> players;
	for(const TiXmlElement* pelem = playerselem->FirstChildElement(); pelem; pelem = pelem->NextSiblingElement()) {
		int playerid;
		if(pelem->QueryIntAttribute("id", &playerid) != TIXML_SUCCESS)
			throw std::runtime_error("Error parsing player in team");
		nameelem = pelem->FirstChildElement("Name");
		if(nameelem) {
			players.push_back(parsePlayer(pelem));
		}
		else {
			playerids.push_back(playerid);
		}
	}

	std::vector<Kit> kits;
	const TiXmlElement* kitselem = teamelem->FirstChildElement("Kits");
	if(kitselem) {
		for(const TiXmlElement* kitelem = kitselem->FirstChildElement(); kitelem; kitelem = kitelem->NextSiblingElement()) {
			int kittype;
			if(kitelem->QueryIntAttribute("type", &kittype) != TIXML_SUCCESS)
				throw std::runtime_error("Error parsing kit in team");
			if(kittype < 0 || kittype > 3)
				throw std::runtime_error("Error parsing kit type in team");

			Common::Color colors[4];
			int j = 0;
			for(auto cs : { "ShirtColor1", "ShirtColor2", "ShortsColor", "SocksColor" }) {
				const TiXmlElement* color1elem = kitelem->FirstChildElement(cs);
				if(!color1elem)
					throw std::runtime_error("Error parsing kit in team");
				int components[3];
				int i = 0;
				for(auto s : { "r", "g", "b" }) {
					if(color1elem->QueryIntAttribute(s, &components[i]) != TIXML_SUCCESS)
						throw std::runtime_error("Error parsing kit in team");
					if(components[i] < 0 || components[i] > 255)
						throw std::runtime_error("Error parsing kit in team");
					i++;
				}
				colors[j++] = Common::Color(components[0], components[1], components[2]);
			}
			kits.push_back(Kit(Kit::KitType(kittype), colors[0], colors[1], colors[2], colors[3]));
			if(kits.size() == 2)
				break;
		}
	}

	while(kits.size() < 2) {
		auto color = kits.size() == 0 ? Common::Color::Black : Common::Color::White;
		kits.push_back(Kit(Kit::KitType::Plain, color, color, color, color));
	}

	boost::shared_ptr<Team> team;
	if(playerids.empty()) {
		team.reset(new Team(id, name, kits[0], kits[1], players));
	}
	else {
		team.reset(new Team(id, name, kits[0], kits[1], playerids));
		for(auto p : players)
			team->addPlayer(p);
	}
	return team;
}

TeamTactics DataExchange::parseTactics(const TiXmlElement* elem)
{
	float pressure, longballs, fastpassing, shootclose;

	if(elem->QueryFloatAttribute("pressure", &pressure) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing Tactics pressure");
	if(elem->QueryFloatAttribute("longballs", &longballs) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing Tactics long balls");
	if(elem->QueryFloatAttribute("fastpassing", &fastpassing) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing Tactics fast passing");
	if(elem->QueryFloatAttribute("shootclose", &shootclose) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing Tactics shoot close");

	const TiXmlElement* playerselem = elem->FirstChildElement("Players");

	std::map<int, PlayerTactics> pt;
	for(const TiXmlElement* pelem = playerselem->FirstChildElement(); pelem; pelem = pelem->NextSiblingElement()) {
		float widthposition;
		float radius;
		int playerid;
		int ppos;
		int offensive;

		if(pelem->QueryIntAttribute("id", &playerid) != TIXML_SUCCESS)
			throw std::runtime_error("Error parsing Tactics player ID");
		if(pelem->QueryFloatAttribute("widthposition", &widthposition) != TIXML_SUCCESS)
			throw std::runtime_error("Error parsing Tactics width position");
		if(pelem->QueryFloatAttribute("radius", &radius) != TIXML_SUCCESS)
			throw std::runtime_error("Error parsing Tactics radius");
		if(pelem->QueryIntAttribute("position", &ppos) != TIXML_SUCCESS)
			throw std::runtime_error("Error parsing Tactics position");
		if(pelem->QueryIntAttribute("offensive", &offensive) != TIXML_SUCCESS)
			throw std::runtime_error("Error parsing Tactics offensive");
		pt.insert(std::make_pair(playerid, PlayerTactics(widthposition, radius, PlayerPosition(ppos), offensive != 0)));
	}
	if(pt.size() != 11) {
		throw std::runtime_error("Error parsing Tactics (expecting 11 player tactics)");
	}

	return TeamTactics(pt, pressure, longballs, fastpassing, shootclose);
}

boost::shared_ptr<Match> DataExchange::parseMatchDataFile(const char* fn)
{
	TiXmlDocument doc(fn);
	std::stringstream ss;
	ss << "Error parsing match file " << fn;

	if(!doc.LoadFile(TIXML_ENCODING_UTF8))
		throw std::runtime_error(ss.str());

	TiXmlHandle handle(&doc);

	TiXmlElement* teamelem = handle.FirstChild("Match").FirstChild("Teams").FirstChild("Team").ToElement();
	if(!teamelem)
		throw std::runtime_error(ss.str());

	std::vector<boost::shared_ptr<Team>> teams;

	for(; teamelem; teamelem = teamelem->NextSiblingElement()) {
		if(teams.size() > 2) {
			throw std::runtime_error(ss.str());
		}
		teams.push_back(parseTeam(teamelem));
	}
	if(teams.size() != 2) {
		throw std::runtime_error(ss.str());
	}

	const TiXmlElement* matchreselem = handle.FirstChild("Match").FirstChild("MatchResult").ToElement();
	if(!matchreselem)
		throw std::runtime_error(ss.str());

	MatchResult mres;
	int played;

	if(matchreselem->QueryIntAttribute("played", &played) != TIXML_SUCCESS)
		throw std::runtime_error(ss.str());
	mres.Played = played;

	if(mres.Played) {
		if(matchreselem->QueryUnsignedAttribute("home", &mres.HomeGoals) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
		if(matchreselem->QueryUnsignedAttribute("away", &mres.AwayGoals) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
		if(matchreselem->QueryUnsignedAttribute("homePenalties", &mres.HomePenalties) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
		if(matchreselem->QueryUnsignedAttribute("awayPenalties", &mres.AwayPenalties) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
	}

	TiXmlElement* controllerelem = handle.FirstChild("Match").FirstChild("Controllers").FirstChild("Controller").ToElement();
	if(!controllerelem)
		throw std::runtime_error(ss.str());

	std::vector<TeamController> tcs;

	for(; controllerelem; controllerelem = controllerelem->NextSiblingElement()) {
		if(tcs.size() > 2) {
			throw std::runtime_error(ss.str());
		}
		int plnum;
		std::string controller;
		if(controllerelem->QueryIntAttribute("number", &plnum) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
		if(controllerelem->QueryStringAttribute("controller", &controller) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
		if(controller == "human")
			tcs.push_back(TeamController(true, plnum));
		else
			tcs.push_back(TeamController(false, 0));
	}
	if(tcs.size() != 2) {
		throw std::runtime_error(ss.str());
	}

	std::vector<TeamTactics> tt;

	TiXmlElement* tacticelem = handle.FirstChild("Match").FirstChild("TeamTactics").FirstChild("Team").ToElement();
	if(!tacticelem)
		throw std::runtime_error(ss.str());

	for(; tacticelem; tacticelem = tacticelem->NextSiblingElement()) {
		if(tt.size() > 2) {
			throw std::runtime_error(ss.str());
		}
		tt.push_back(parseTactics(tacticelem));
	}
	if(tt.size() != 2) {
		throw std::runtime_error(ss.str());
	}

	const TiXmlElement* matchruleselem = handle.FirstChild("Match").FirstChild("MatchRules").ToElement();
	if(!matchruleselem)
		throw std::runtime_error(ss.str());

	int et, pen, awaygoals;
	int homeagg = 0, awayagg = 0;

	if(matchruleselem->QueryIntAttribute("et", &et) != TIXML_SUCCESS)
		throw std::runtime_error(ss.str());

	if(matchruleselem->QueryIntAttribute("pen", &pen) != TIXML_SUCCESS)
		throw std::runtime_error(ss.str());

	if(matchruleselem->QueryIntAttribute("awaygoals", &awaygoals) != TIXML_SUCCESS)
		throw std::runtime_error(ss.str());

	if(awaygoals) {
		if(matchruleselem->QueryIntAttribute("homeaggregate", &homeagg) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());

		if(matchruleselem->QueryIntAttribute("awayaggregate", &awayagg) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
	}

	boost::shared_ptr<Match> m(new Match(boost::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[0], tcs[0], tt[0])),
				boost::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[1], tcs[1], tt[1])),
				MatchRules(et, pen, awaygoals, homeagg, awayagg)));
	m->setResult(mres);
	return m;
}

TiXmlElement* DataExchange::createTeamElement(const Team& t, bool reference_players)
{
	TiXmlElement* teamelem = new TiXmlElement("Team");

	teamelem->SetAttribute("id", t.getId());

	{
		TiXmlElement* nameelem = new TiXmlElement("Name");
		nameelem->LinkEndChild(new TiXmlText(t.getName()));
		teamelem->LinkEndChild(nameelem);
	}

	TiXmlElement* playerselem = new TiXmlElement("Players");

	int j = 0;
	while(1) {
		boost::shared_ptr<Player> p = t.getPlayer(j);
		if(!p)
			break;
		j++;

		{
			TiXmlElement* playerelem;
			if(reference_players) {
				playerelem = new TiXmlElement("Player");
				playerelem->SetAttribute("id", p->getId());
			}
			else {
				playerelem = createPlayerElement(*p);
			}
			playerselem->LinkEndChild(playerelem);
		}
	}
	teamelem->LinkEndChild(playerselem);

	TiXmlElement* kitselem = new TiXmlElement("Kits");
	for(auto k : { t.getHomeKit(), t.getAwayKit() }) {
		TiXmlElement* kitelem = new TiXmlElement("Kit");
		kitelem->SetAttribute("type", int(k.getKitType()));

		TiXmlElement* col1elem = new TiXmlElement("ShirtColor1");
		col1elem->SetAttribute("r", int(k.getPrimaryShirtColor().r));
		col1elem->SetAttribute("g", int(k.getPrimaryShirtColor().g));
		col1elem->SetAttribute("b", int(k.getPrimaryShirtColor().b));

		TiXmlElement* col2elem = new TiXmlElement("ShirtColor2");
		col2elem->SetAttribute("r", int(k.getSecondaryShirtColor().r));
		col2elem->SetAttribute("g", int(k.getSecondaryShirtColor().g));
		col2elem->SetAttribute("b", int(k.getSecondaryShirtColor().b));

		TiXmlElement* col3elem = new TiXmlElement("ShortsColor");
		col3elem->SetAttribute("r", int(k.getShortsColor().r));
		col3elem->SetAttribute("g", int(k.getShortsColor().g));
		col3elem->SetAttribute("b", int(k.getShortsColor().b));

		TiXmlElement* col4elem = new TiXmlElement("SocksColor");
		col4elem->SetAttribute("r", int(k.getSocksColor().r));
		col4elem->SetAttribute("g", int(k.getSocksColor().g));
		col4elem->SetAttribute("b", int(k.getSocksColor().b));

		kitelem->LinkEndChild(col1elem);
		kitelem->LinkEndChild(col2elem);
		kitelem->LinkEndChild(col3elem);
		kitelem->LinkEndChild(col4elem);
		kitselem->LinkEndChild(kitelem);
	}
	teamelem->LinkEndChild(kitselem);

	return teamelem;
}

TiXmlElement* DataExchange::createTeamTacticsElement(const TeamTactics& t)
{
	TiXmlElement* teamelem = new TiXmlElement("Team");

	teamelem->SetDoubleAttribute("pressure", t.Pressure);
	teamelem->SetDoubleAttribute("longballs", t.LongBalls);
	teamelem->SetDoubleAttribute("fastpassing", t.FastPassing);
	teamelem->SetDoubleAttribute("shootclose", t.ShootClose);

	TiXmlElement* playerselem = new TiXmlElement("Players");

	assert(t.mTactics.size() == 11);
	for(auto p : t.mTactics) {
		TiXmlElement* playerelem = new TiXmlElement("Player");
		playerelem->SetAttribute("id", p.first);
		playerelem->SetDoubleAttribute("widthposition", p.second.WidthPosition);
		playerelem->SetDoubleAttribute("radius", p.second.Radius);
		playerelem->SetAttribute("position", int(p.second.Position));
		playerelem->SetAttribute("offensive", p.second.Offensive);
		playerselem->LinkEndChild(playerelem);
	}
	teamelem->LinkEndChild(playerselem);

	return teamelem;
}

TiXmlElement* DataExchange::createPlayerElement(const Player& p)
{
	TiXmlElement* playerelem = new TiXmlElement("Player");

	playerelem->SetAttribute("id", p.getId());

	TiXmlElement* nameelem = new TiXmlElement("Name");
	nameelem->LinkEndChild(new TiXmlText(p.getName()));
	playerelem->LinkEndChild(nameelem);

	TiXmlElement* skillselem = new TiXmlElement("Skills");
	TiXmlElement* skill1elem = new TiXmlElement("ShotPower");
	TiXmlElement* skill2elem = new TiXmlElement("Passing");
	TiXmlElement* skill3elem = new TiXmlElement("RunSpeed");
	TiXmlElement* skill4elem = new TiXmlElement("BallControl");
	TiXmlElement* skill5elem = new TiXmlElement("Tackling");
	TiXmlElement* skill6elem = new TiXmlElement("Heading");
	TiXmlElement* skill7elem = new TiXmlElement("GoalKeeping");
	skill1elem->LinkEndChild(new TiXmlText(std::to_string(p.getSkills().ShotPower)));
	skill2elem->LinkEndChild(new TiXmlText(std::to_string(p.getSkills().Passing)));
	skill3elem->LinkEndChild(new TiXmlText(std::to_string(p.getSkills().RunSpeed)));
	skill4elem->LinkEndChild(new TiXmlText(std::to_string(p.getSkills().BallControl)));
	skill5elem->LinkEndChild(new TiXmlText(std::to_string(p.getSkills().Tackling)));
	skill6elem->LinkEndChild(new TiXmlText(std::to_string(p.getSkills().Heading)));
	skill7elem->LinkEndChild(new TiXmlText(std::to_string(p.getSkills().GoalKeeping)));
	skillselem->LinkEndChild(skill1elem);
	skillselem->LinkEndChild(skill2elem);
	skillselem->LinkEndChild(skill3elem);
	skillselem->LinkEndChild(skill4elem);
	skillselem->LinkEndChild(skill5elem);
	skillselem->LinkEndChild(skill6elem);
	skillselem->LinkEndChild(skill7elem);
	playerelem->LinkEndChild(skillselem);

	return playerelem;
}

void DataExchange::createMatchDataFile(const Match& m, const char* fn)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* matchelem = new TiXmlElement("Match");
	TiXmlElement* teamselem = new TiXmlElement("Teams");
	for(int i = 0; i < 2; i++) {
		boost::shared_ptr<Team> t = m.getTeam(i);
		TiXmlElement* teamelem = createTeamElement(*t, false);
		teamselem->LinkEndChild(teamelem);
	}
	matchelem->LinkEndChild(teamselem);

	TiXmlElement* teamtacticselem = new TiXmlElement("TeamTactics");
	for(int i = 0; i < 2; i++) {
		TiXmlElement* tacticelem = createTeamTacticsElement(m.getTeam(i)->getTactics());
		teamtacticselem->LinkEndChild(tacticelem);
	}
	matchelem->LinkEndChild(teamtacticselem);

	TiXmlElement* matchresultelem = new TiXmlElement("MatchResult");
	matchresultelem->SetAttribute("played", m.getResult().Played ? "1" : "0");
	matchresultelem->SetAttribute("home", m.getResult().HomeGoals);
	matchresultelem->SetAttribute("away", m.getResult().AwayGoals);
	matchresultelem->SetAttribute("homePenalties", m.getResult().HomePenalties);
	matchresultelem->SetAttribute("awayPenalties", m.getResult().AwayPenalties);
	matchelem->LinkEndChild(matchresultelem);

	{
		TiXmlElement* controllerselem = new TiXmlElement("Controllers");
		TiXmlElement* cont1elem = new TiXmlElement("Controller");
		TiXmlElement* cont2elem = new TiXmlElement("Controller");
		cont1elem->SetAttribute("controller", m.getTeam(0)->getController().HumanControlled ?
				"human" : "computer");
		cont1elem->SetAttribute("number", m.getTeam(0)->getController().PlayerShirtNumber);
		cont2elem->SetAttribute("controller", m.getTeam(1)->getController().HumanControlled ?
				"human" : "computer");
		cont2elem->SetAttribute("number", m.getTeam(1)->getController().PlayerShirtNumber);
		controllerselem->LinkEndChild(cont1elem);
		controllerselem->LinkEndChild(cont2elem);
		matchelem->LinkEndChild(controllerselem);
	}

	{
		TiXmlElement* ruleselem = new TiXmlElement("MatchRules");
		ruleselem->SetAttribute("et", m.getRules().ExtraTimeOnTie ? "1" : "0");
		ruleselem->SetAttribute("pen", m.getRules().PenaltiesOnTie ? "1" : "0");
		ruleselem->SetAttribute("awaygoals", m.getRules().AwayGoals ? "1" : "0");
		if(m.getRules().AwayGoals) {
			ruleselem->SetAttribute("homeaggregate", m.getRules().HomeAggregate);
			ruleselem->SetAttribute("awayaggregate", m.getRules().AwayAggregate);
		}
		matchelem->LinkEndChild(ruleselem);
	}

	doc.LinkEndChild(decl);
	doc.LinkEndChild(matchelem);
	if(!doc.SaveFile(fn)) {
		throw std::runtime_error(std::string("Unable to save XML file ") + fn);
	}
}


void DataExchange::updateTeamDatabase(const char* fn, TeamDatabase& db)
{
	TiXmlDocument doc(fn);
	std::stringstream ss;
	ss << "Error parsing team database file " << fn << ": ";

	if(!doc.LoadFile(TIXML_ENCODING_UTF8))
		throw std::runtime_error(ss.str());

	TiXmlHandle handle(&doc);

	TiXmlElement* continentelem = handle.FirstChild("Teams").FirstChild("Continent").ToElement();
	if(!continentelem) {
		ss << "no Teams/Continent";
		throw std::runtime_error(ss.str());
	}

	for(; continentelem; continentelem = continentelem->NextSiblingElement()) {
		std::string continentname;
		if(continentelem->QueryStringAttribute("name", &continentname) != TIXML_SUCCESS)
			throw std::runtime_error("Error parsing continent name");

		for(const TiXmlElement* countryelem = continentelem->FirstChildElement(); countryelem;
				countryelem = countryelem->NextSiblingElement()) {
			std::string countryname;
			unsigned int level = 0;

			if(countryelem->QueryStringAttribute("name", &countryname) != TIXML_SUCCESS)
				throw std::runtime_error("Error parsing country name");

			for(const TiXmlElement* leagueelem = countryelem->FirstChildElement(); leagueelem;
					leagueelem = leagueelem->NextSiblingElement()) {
				std::string leaguename;
				if(leagueelem->QueryStringAttribute("name", &leaguename) != TIXML_SUCCESS)
					throw std::runtime_error("Error parsing league name");

				boost::shared_ptr<Soccer::League> league = db.getOrCreateLeague(continentname.c_str(),
						countryname.c_str(), leaguename.c_str(), level);

				for(const TiXmlElement* teamelem = leagueelem->FirstChildElement(); teamelem;
						teamelem = teamelem->NextSiblingElement()) {
					boost::shared_ptr<Team> t = parseTeam(teamelem);
					league->addT(t);
				}

				level++;
			}
		}
	}
}

void DataExchange::updatePlayerDatabase(const char* fn, PlayerDatabase& db)
{
	TiXmlDocument doc(fn);
	std::stringstream ss;
	ss << "Error parsing team database file " << fn << ": ";

	if(!doc.LoadFile(TIXML_ENCODING_UTF8)) {
		ss << "could not open file";
		throw std::runtime_error(ss.str());
	}

	TiXmlHandle handle(&doc);

	TiXmlElement* playerselem = handle.FirstChild("Players").ToElement();
	if(!playerselem) {
		ss << "no Players element";
		throw std::runtime_error(ss.str());
	}

	for(TiXmlElement* pelem = playerselem->FirstChildElement(); pelem; pelem = pelem->NextSiblingElement()) {
		boost::shared_ptr<Player> p = parsePlayer(pelem);
		db.insert(std::make_pair(p->getId(), p));
	}
}

void DataExchange::createTeamDatabase(const char* fn, const TeamDatabase& db)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* teamselem = new TiXmlElement("Teams");
	for(auto& cont : db.getContainer()) {
		TiXmlElement* contelem = new TiXmlElement("Continent");
		contelem->SetAttribute("name", cont.second->getName());

		for(auto& lsys : cont.second->getContainer()) {
			TiXmlElement* lsyselem = new TiXmlElement("LeagueSystem");
			lsyselem->SetAttribute("name", lsys.second->getName());

			for(auto& leag : lsys.second->getContainer()) {
				TiXmlElement* leagelem = new TiXmlElement("League");
				leagelem->SetAttribute("name", leag.second->getName());

				for(auto& tm : leag.second->getContainer()) {
					TiXmlElement* teamelem = createTeamElement(*tm.second, true);
					leagelem->LinkEndChild(teamelem);
				}
				lsyselem->LinkEndChild(leagelem);
			}
			contelem->LinkEndChild(lsyselem);
		}
		teamselem->LinkEndChild(contelem);
	}
	doc.LinkEndChild(decl);
	doc.LinkEndChild(teamselem);
	if(!doc.SaveFile(fn)) {
		throw std::runtime_error(std::string("Unable to save XML file ") + fn);
	}
}

void DataExchange::createPlayerDatabase(const char* fn, const PlayerDatabase& db)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* players = new TiXmlElement("Players");
	for(auto& pl : db) {
		TiXmlElement* playerelem = createPlayerElement(*pl.second);
		players->LinkEndChild(playerelem);
	}
	doc.LinkEndChild(decl);
	doc.LinkEndChild(players);
	if(!doc.SaveFile(fn)) {
		throw std::runtime_error(std::string("Unable to save XML file ") + fn);
	}
}


}
