#include <string>
#include <stdexcept>

#include "soccer/DataExchange.h"

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

std::shared_ptr<Player> DataExchange::parsePlayer(const TiXmlElement* pelem)
{
	PlayerSkills sk;
	PlayerPosition position;
	int id;

	if(pelem->QueryIntAttribute("id", &id) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing player");

	const TiXmlElement* skillselem = pelem->FirstChildElement("Skills");
	const TiXmlElement* nameelem = pelem->FirstChildElement("Name");
	const TiXmlElement* poselem = pelem->FirstChildElement("Position");
	if(!skillselem || !nameelem || !poselem)
		throw std::runtime_error("Error parsing player");

	const char* pos = poselem->GetText();
	const char* name = nameelem->GetText();
	if(!pos || !name)
		throw std::runtime_error("Error parsing player");

	if(!strcmp(pos, "goalkeeper"))
		position = PlayerPosition::Goalkeeper;
	else if(!strcmp(pos, "defender"))
		position = PlayerPosition::Defender;
	else if(!strcmp(pos, "midfielder"))
		position = PlayerPosition::Midfielder;
	else if(!strcmp(pos, "forward"))
		position = PlayerPosition::Forward;
	else
		throw std::runtime_error("Error parsing player");

	sk.KickPower = getPlayerSkill(skillselem, "KickPower");
	sk.RunSpeed = getPlayerSkill(skillselem, "RunSpeed");
	sk.BallControl = getPlayerSkill(skillselem, "BallControl");
	return std::shared_ptr<Player>(new Player(id, name, position, sk));
}

std::shared_ptr<Match> DataExchange::parseMatchDataFile(const char* fn)
{
	TiXmlDocument doc(fn);
	std::stringstream ss;
	ss << "Error parsing match file " << fn;

	if(!doc.LoadFile())
		throw std::runtime_error(ss.str());

	TiXmlHandle handle(&doc);

	TiXmlElement* teamelem = handle.FirstChild("Match").FirstChild("Teams").FirstChild("Team").ToElement();
	if(!teamelem)
		throw std::runtime_error(ss.str());

	std::shared_ptr<Team> t1(new Team());
	std::shared_ptr<Team> t2(new Team());
	std::shared_ptr<Team> thisteam;

	for(; teamelem; teamelem = teamelem->NextSiblingElement()) {
		int home;
		if(teamelem->QueryIntAttribute("home", &home) != TIXML_SUCCESS)
			throw std::runtime_error(ss.str());
		thisteam = home ? t1 : t2;
		TiXmlElement* playerselem = teamelem->FirstChildElement("Players");
		if(!playerselem)
			throw std::runtime_error(ss.str());
		for(TiXmlElement* pelem = playerselem->FirstChildElement(); pelem; pelem = pelem->NextSiblingElement()) {
			thisteam->addPlayer(parsePlayer(pelem));
		}
	}

	/* TODO: parse match result */

	std::shared_ptr<Match> m(new Match(t1, t2, TeamTactics(), TeamTactics()));
	return m;
}

void DataExchange::createMatchDataFile(const Match& m, const char* fn)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* matchelem = new TiXmlElement("Match");
	TiXmlElement* teamselem = new TiXmlElement("Teams");
	for(int i = 0; i < 2; i++) {
		TiXmlElement* teamelem = new TiXmlElement("Team");
		teamelem->SetAttribute("home", i == 0);
		std::shared_ptr<Team> t = m.getTeam(i);
		int j = 0;
		TiXmlElement* playerselem = new TiXmlElement("Players");
		while(1) {
			std::shared_ptr<Player> p = t->getPlayer(j);
			if(!p)
				break;
			j++;
			TiXmlElement* playerelem = new TiXmlElement("Player");
			playerelem->SetAttribute("id", p->getId());

			TiXmlElement* nameelem = new TiXmlElement("Name");
			nameelem->LinkEndChild(new TiXmlText(p->getName()));
			playerelem->LinkEndChild(nameelem);

			TiXmlElement* poselem = new TiXmlElement("Position");
			switch(p->getPlayerPosition()) {
				case PlayerPosition::Goalkeeper:
					poselem->LinkEndChild(new TiXmlText("goalkeeper"));
					break;

				case PlayerPosition::Defender:
					poselem->LinkEndChild(new TiXmlText("defender"));
					break;

				case PlayerPosition::Midfielder:
					poselem->LinkEndChild(new TiXmlText("midfielder"));
					break;

				case PlayerPosition::Forward:
					poselem->LinkEndChild(new TiXmlText("forward"));
					break;
			}
			playerelem->LinkEndChild(poselem);

			TiXmlElement* skillselem = new TiXmlElement("Skills");
			TiXmlElement* skill1elem = new TiXmlElement("KickPower");
			TiXmlElement* skill2elem = new TiXmlElement("RunSpeed");
			TiXmlElement* skill3elem = new TiXmlElement("BallControl");
			skill1elem->LinkEndChild(new TiXmlText(std::to_string(p->getSkills().KickPower)));
			skill2elem->LinkEndChild(new TiXmlText(std::to_string(p->getSkills().RunSpeed)));
			skill3elem->LinkEndChild(new TiXmlText(std::to_string(p->getSkills().BallControl)));
			skillselem->LinkEndChild(skill1elem);
			skillselem->LinkEndChild(skill2elem);
			skillselem->LinkEndChild(skill3elem);
			playerelem->LinkEndChild(skillselem);
			playerselem->LinkEndChild(playerelem);
		}
		teamelem->LinkEndChild(playerselem);
		teamselem->LinkEndChild(teamelem);
	}
	matchelem->LinkEndChild(teamselem);

	TiXmlElement* matchresultelem = new TiXmlElement("MatchResult");
	matchresultelem->SetAttribute("played", 0);
	TiXmlElement* homereselem = new TiXmlElement("Home");
	TiXmlElement* awayreselem = new TiXmlElement("Away");
	homereselem->LinkEndChild(new TiXmlText("0"));
	awayreselem->LinkEndChild(new TiXmlText("0"));
	matchresultelem->LinkEndChild(homereselem);
	matchresultelem->LinkEndChild(awayreselem);
	matchelem->LinkEndChild(matchresultelem);

	doc.LinkEndChild(decl);
	doc.LinkEndChild(matchelem);
	doc.SaveFile(fn);

}


void DataExchange::updateTeamDatabase(const char* fn, TeamDatabase& db)
{
	/* TODO */
}

void DataExchange::updatePlayerDatabase(const char* fn, PlayerDatabase& db)
{
	/* TODO */
}


}
