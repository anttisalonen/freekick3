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
	int sn;
	int gk;
	PlayerSkills sk;

	if(pelem->QueryIntAttribute("shirtnumber", &sn) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing player shirtnumber");
	if(pelem->QueryIntAttribute("goalkeeper", &gk) != TIXML_SUCCESS)
		throw std::runtime_error("Error parsing player goalkeeper flag");

	const TiXmlElement* skillselem = pelem->FirstChildElement("Skills");
	if(!skillselem)
		throw std::runtime_error("Error parsing player skills");
	sk.KickPower = getPlayerSkill(skillselem, "KickPower");
	sk.RunSpeed = getPlayerSkill(skillselem, "RunSpeed");
	sk.BallControl = getPlayerSkill(skillselem, "BallControl");
	return std::shared_ptr<Player>(new Player(sn, gk != 0, sk));
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



}
