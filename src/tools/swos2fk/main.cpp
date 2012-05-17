#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>

#include "soccer/DataExchange.h"
#include "soccer/Continent.h"
#include "soccer/Player.h"
#include "soccer/Team.h"

static const int team_size = 684;
static const int player_size = 38;
static const int player_name_len = 23;

typedef struct
{
	int id;
	int nationality;
	int number;
	char player_name[player_name_len];
	/* unsigned char multi_purpose; - represented by field_position and head_type */
	int field_position;
	int head_type;
	int passing;
	int shooting;
	int heading;
	int tackling;
	int ball_control;
	int speed;
	int finishing;
	int value;
} s_player;

typedef struct
{
	int type;
	int first_color;
	int second_color;
	int short_color;
	int socks_color;
} s_kit;

static const int team_name_len = 19;
static const int coach_name_len = 25;
static const int player_position_in_file_len = 15;
static const int number_of_players = 16;

typedef struct
{
	int nation;
	int team_number;
	int swos_team_number;
	char team_name[team_name_len];
	int tactics;
	int division;
	s_kit primary_kit;
	s_kit secondary_kit;
	char coach_name[coach_name_len];
	char player_position_in_file[player_position_in_file_len];
	s_player players[number_of_players];
	int value;
} s_team;

class SwosParser {
	public:
		static int parse_team(const unsigned char* team_block, s_team* team, int firstPlayerId);
		static int parse_player(const unsigned char* player_block, s_player* player, int playerId);
		static int parse_kit(const unsigned char* kit_block, s_kit* kit);
		static void correct_name(char* n);
};

int SwosParser::parse_team(const unsigned char* team_block, s_team* team, int firstPlayerId)
{
	int retval = 0;
	const unsigned char* iter = team_block;
	team->nation = *iter++;
	team->team_number = *iter++;
	memcpy(&team->swos_team_number, (const void*)iter, 2); iter += 2;
	int pos_4 = *iter++;
	if(pos_4 != 0)
	{
		std::cerr << "parse_team: failed.\n";
		return 1;
	}
	memcpy(&team->team_name, (const void*)iter, team_name_len); iter += team_name_len;
	correct_name(team->team_name);
	// std::cerr << team->team_name << endl;
	team->tactics = *iter++;
	team->division = *iter++;
	for(int i = 0; i < 2; i++)
	{
		if(!i)
			retval = parse_kit(iter, &team->primary_kit);
		else
			retval = parse_kit(iter, &team->secondary_kit);
		if(retval != 0)
		{
			std::cerr << "parse_team: failed kit parse.\n";
			return 1;
		}
		iter += 5;
	}
	memcpy(&team->coach_name, (const void*)iter, coach_name_len); iter += coach_name_len;
	correct_name(team->coach_name);
	// std::cerr << team->coach_name << endl;
	memcpy(&team->player_position_in_file, (const void*)iter, player_position_in_file_len); iter += player_position_in_file_len;
	team->value = 0;
	for(int pnum = 0; pnum < number_of_players; pnum++)
	{
		retval = parse_player(iter, &team->players[pnum], firstPlayerId);
		if(retval != 0)
		{
			std::cerr << "parse_team: failed player parse.\n";
			return 1;
		}
		iter += player_size;
		firstPlayerId++;
		team->value += team->players[pnum].value;
	}
	// std::cerr << team->value << endl;
	return 0;
}

int SwosParser::parse_player(const unsigned char* player_block, s_player* player, int playerId)
{
	const unsigned char* iter = player_block;
	player->id = playerId;
	player->nationality = *iter++;
	int pos_1 = *iter++;
	if(pos_1 != 0)
	{
		std::cerr << "parse_player: pos_1 failed: " << pos_1 << ".\n";
		return 1;
	}
	player->number = *iter++;
	memcpy(&player->player_name, (const void*)iter, player_name_len); iter += player_name_len;
	correct_name(player->player_name);
	// std::cerr << player->player_name << endl;
	unsigned char multi_purpose = *iter++;
	player->field_position = multi_purpose >> 5;
	player->head_type      = (multi_purpose >> 3) & 0x03;

	unsigned char skill_byte = *iter++;
	player->passing      = skill_byte & 0x07;
	skill_byte = *iter++;
	player->shooting     = skill_byte >> 4;
	player->heading      = skill_byte & 0x07;
	skill_byte = *iter++;
	player->tackling     = skill_byte >> 4;
	player->ball_control = skill_byte & 0x07;
	skill_byte = *iter++;
	player->speed        = skill_byte >> 4;
	player->finishing    = skill_byte & 0x07;
	iter++;
	player->value = *iter++;
	// cout << player->value << endl;
	// cout << player->tackling << endl;
	// cout << player->finishing << endl;
	return 0;
}

int SwosParser::parse_kit(const unsigned char* kit_block, s_kit* kit)
{
    const unsigned char* iter = kit_block;
    kit->type = *iter++;
    kit->first_color = *iter++;
    kit->second_color = *iter++;
    kit->short_color = *iter++;
    kit->socks_color = *iter++;
    return 0;
}

void SwosParser::correct_name(char* n)
{
    char* iter = n;
    while(*iter != 0)
    {
        if(iter != n)
        {
            if (*(iter - 1) != ' ' && *(iter - 1) != '\'' && *(iter - 1) != '-')
            {
                *iter = tolower(*iter);
            }
        }
        iter++;
    }
}

class FreekickWriter {
	public:
		FreekickWriter(const std::string& outputDir, const std::string& teamFile,
				bool scramble, const std::vector<s_team>& teams);
		int write();

	private:
		void setupDefaultNationalities();
		const char* teamNationalityToString(int i);
		std::string scramble(const char* n);
		std::string mOutputDir;
		std::string mTeamFile;
		const std::vector<s_team>& mTeams;
		int mCurrentTeamId;
		std::map<std::pair<int, int>, int> mLeagues;
		int mCurrentLeagueId;
		std::map<int, std::string> mNationalities;
		bool mScramble;
};

FreekickWriter::FreekickWriter(const std::string& outputDir, const std::string& teamFile,
		bool scramble, const std::vector<s_team>& teams)
	 : mOutputDir(outputDir),
	 mTeamFile(teamFile),
	 mTeams(teams),
	 mCurrentTeamId(1),
	 mCurrentLeagueId(1),
	 mScramble(scramble)
{
	if(mTeamFile.empty()) {
		setupDefaultNationalities();
	}
	else {
		std::ifstream teamstream(mTeamFile);
		if(!teamstream.is_open())
		{
			throw std::runtime_error(std::string("Could not open file ") + mTeamFile);
		}
		while(1) {
			std::string str;
			std::getline(teamstream, str);
			if(teamstream.fail())
				break;
			std::string fname, country;
			std::stringstream strstr(str);
			std::getline(strstr, fname, ':');
			std::getline(strstr, country, ':');
			mNationalities[atoi(fname.c_str())] = country;
		}
	}
}

const char* nationToContinent(int i)
{
	if(i <= 41)
		return "Europe";

	switch(i) {
		case 42: return "Africa";
		case 43: return "South America";
		case 44: return "Oceania";
		case 45: return "South America";
		case 46: return "South America";
		case 48: return "South America";
		case 49: return "South America";
		case 50: return "South America";
		case 51: return "North & Central America";
		case 55: return "Asia";
		case 60: return "North & Central America";
		case 62: return "Oceania";
		case 64: return "South America";
		case 65: return "South America";
		case 66: return "South America";
		case 67: return "Asia";
		case 69: return "Africa";
		case 71: return "South America";
		case 73: return "North & Central America";
		case 75: return "Asia";
		case 76: return "Europe";
		case 77: return "South America";
		case 78: return "Europe";
		case 79: return "Africa";
	}
	return "Unknown";
}

void FreekickWriter::setupDefaultNationalities()
{
	mNationalities.clear();
	for(int i = 0; i < 80; i++) {
		const char* value = nullptr;
		switch(i) {
			case 0: value = "Albania"; break;
			case 1: value = "Austria"; break;
			case 2: value = "Belgium"; break;
			case 3: value = "Bulgaria"; break;
			case 4: value = "Croatia"; break;
			case 5: value = "Cyprus"; break;
			case 6: value = "Czech Republic"; break;
			case 7: value = "Denmark"; break;
			case 8: value = "England"; break;
			case 9: break;
			case 10: value = "Estonia"; break;
			case 11: value = "Faroe Islands"; break;
			case 12: value = "Finland"; break;
			case 13: value = "France"; break;
			case 14: value = "Germany"; break;
			case 15: value = "Greece"; break;
			case 16: value = "Hungary"; break;
			case 17: value = "Iceland"; break;
			case 18: value = "Ireland"; break;
			case 19: value = "Israel"; break;
			case 20: value = "Italy"; break;
			case 21: value = "Latvia"; break;
			case 22: value = "Lithuania"; break;
			case 23: value = "Luxembourg"; break;
			case 24: value = "Malta"; break;
			case 25: value = "Netherlands"; break;
			case 26: value = "Northern Ireland"; break;
			case 27: value = "Norway"; break;
			case 28: value = "Poland"; break;
			case 29: value = "Portugal"; break;
			case 30: value = "Romania"; break;
			case 31: value = "Russia"; break;
			case 32: value = "San Marino"; break;
			case 33: value = "Scotland"; break;
			case 34: value = "Slovenia"; break;
			case 35: value = "Spain"; break;
			case 36: value = "Sweden"; break;
			case 37: value = "Switzerland"; break;
			case 38: value = "Turkey"; break;
			case 39: value = "Ukraine"; break;
			case 40: value = "Wales"; break;
			case 41: value = "Serbia"; break;
			case 42: value = "Algeria"; break;
			case 43: value = "Argentina"; break;
			case 44: value = "Australia"; break;
			case 45: value = "Bolivia"; break;
			case 46: value = "Brazil"; break;
			case 47: break;
			case 48: value = "Chile"; break;
			case 49: value = "Colombia"; break;
			case 50: value = "Ecuador"; break;
			case 51: value = "El Salvador"; break;
			case 52: break;
			case 53: break;
			case 54: break;
			case 55: value = "Japan"; break;
			case 56: break;
			case 57: break;
			case 58: break;
			case 59: break;
			case 60: value = "Mexico"; break;
			case 61: break;
			case 62: value = "New Zealand"; break;
			case 63: break;
			case 64: value = "Paraguay"; break;
			case 65: value = "Peru"; break;
			case 66: value = "Surinam"; break;
			case 67: value = "China"; break;
			case 68: break;
			case 69: value = "South Africa"; break;
			case 70: break;
			case 71: value = "Uruguay"; break;
			case 72: break;
			case 73: value = "U.S.A."; break;
			case 74: break;
			case 75: value = "India"; break;
			case 76: value = "Belarus"; break;
			case 77: value = "Venezuela"; break;
			case 78: value = "Slovenia"; break;
			case 79: value = "Ghana"; break;
		}
		if(!value)
			continue;

		mNationalities[i] = std::string(value);
	}
}

const char* FreekickWriter::teamNationalityToString(int i)
{
	const auto& it = mNationalities.find(i);
	if(it == mNationalities.end())
		return "Unknown";
	else
		return it->second.c_str();
}

std::string FreekickWriter::scramble(const char* n)
{
	std::string ret(n);
	for(unsigned int i = 0; i < ret.size(); i++) {
		if(isdigit(ret[i])) {
			if(ret[i] == '9')
				ret[i] = '0';
			else
				ret[i]++;
		}
		if(isalpha(ret[i])) {
			switch(ret[i]) {
				case 'a': ret[i] = 'e'; break;
				case 'A': ret[i] = 'E'; break;
				case 'e': ret[i] = 'i'; break;
				case 'E': ret[i] = 'I'; break;
				case 'i': ret[i] = 'o'; break;
				case 'I': ret[i] = 'O'; break;
				case 'o': ret[i] = 'u'; break;
				case 'O': ret[i] = 'U'; break;
				case 'u': ret[i] = 'y'; break;
				case 'U': ret[i] = 'Y'; break;
				case 'y': ret[i] = 'a'; break;
				case 'Y': ret[i] = 'A'; break;
				case 'z': case 'Z': case ' ': break;
				case 'p': case 'P': case 'w': case 'W': break;
				default: ret[i]++; break;
			}
		}
	}
	return ret;
}

const char* playerNationalityToString(int n)
{
	switch(n)
	{
		case 0: return "Albania";
		case 1: return "Austria";
		case 2: return "Belgium";
		case 3: return "Bulgaria";
		case 4: return "Croatia";
		case 5: return "Cyprus";
		case 6: return "Czech Republic";
		case 7: return "Denmark";
		case 8: return "England";
		case 9: return "Estonia";
		case 10: return "Faroe Islands";
		case 11: return "Finland";
		case 12: return "France";
		case 13: return "Germany";
		case 14: return "Greece";
		case 15: return "Hungary";
		case 16: return "Iceland";
		case 17: return "Israel";
		case 18: return "Italy";
		case 19: return "Latvia";
		case 20: return "Lithuania";
		case 21: return "Luxembourg";
		case 22: return "Malta";
		case 23: return "The Netherlands";
		case 24: return "Northern Ireland";
		case 25: return "Norway";
		case 26: return "Poland";
		case 27: return "Portugal";
		case 28: return "Romania";
		case 29: return "Russia";
		case 30: return "San Marino";
		case 31: return "Scotland";
		case 32: return "Slovenia";
		case 33: return "Sweden";
		case 34: return "Turkey";
		case 35: return "Ukraine";
		case 36: return "Wales";
		case 37: return "Serbia";  // check for montenegrin players
		case 38: return "Belarus";
		case 39: return "Slovakia";
		case 40: return "Spain";
		case 41: return "Armenia";
		case 42: return "Bosnia-Herzegovina";
		case 43: return "Azerbaijan";
		case 44: return "Georgia";
		case 45: return "Switzerland";
		case 46: return "Ireland";
		case 47: return "FYR Macedonia";
		case 48: return "Turkmenistan";
		case 49: return "Liechtenstein";
		case 50: return "Moldova";
		case 51: return "Costa Rica";
		case 52: return "El Salvador";
		case 53: return "Guatemala";
		case 54: return "Honduras";
		case 55: return "Bahamas";
		case 56: return "Mexico";
		case 57: return "Panama";
		case 58: return "U.S.A.";
		case 59: return "Bahrain";
		case 60: return "Nicaragua";
		case 61: return "Bermuda";
		case 62: return "Jamaica";
		case 63: return "Trinidad and Tobago";
		case 64: return "Canada";
		case 65: return "Barbados";
		case 66: return "El Salvador";
		case 67: return "Saint Vincent and the Grenadines";
		case 68: return "Argentina";
		case 69: return "Bolivia";
		case 70: return "Brazil";
		case 71: return "Chile";
		case 72: return "Colombia";
		case 73: return "Ecuador";
		case 74: return "Paraguay";
		case 75: return "Surinam";
		case 76: return "Uruguay";
		case 77: return "Venezuela";
		case 78: return "Guyana";
		case 79: return "Peru";
		case 80: return "Algeria";
		case 81: return "South Africa";
		case 82: return "Botswana";
		case 83: return "Burkina Faso";
		case 84: return "Burundi";
		case 85: return "Lesotho";
		case 86: return "Congo";
		case 87: return "Zambia";
		case 88: return "Ghana";
		case 89: return "Senegal";
		case 90: return "Ivory Coast";
		case 91: return "Tunisia";
		case 92: return "Mali";
		case 93: return "Madagascar";
		case 94: return "Cameroon";
		case 95: return "Chad";
		case 96: return "Uganda";
		case 97: return "Liberia";
		case 98: return "Mozambique";
		case 99: return "Kenia";
		case 100: return "Sudan";
		case 101: return "Swaziland";
		case 102: return "Angola";
		case 103: return "Togo";
		case 104: return "Zimbabwe";
		case 105: return "Egypt";
		case 106: return "Tanzania";
		case 107: return "Nigeria";
		case 108: return "Ethiopia";
		case 109: return "Gabon";
		case 110: return "Sierra Leone";
		case 111: return "Benin";
		case 112: return "Congo";
		case 113: return "Guinea";
		case 114: return "Sri Lanka";
		case 115: return "Morocco";
		case 116: return "Gambia";
		case 117: return "Malawi";
		case 118: return "Japan";
		case 119: return "Taiwan";
		case 120: return "India";
		case 121: return "Bangladesh";
		case 122: return "Brunei";
		case 123: return "Iraq";
		case 124: return "Jordan";
		case 125: return "Sri Lanka";
		case 126: return "Syria";
		case 127: return "South Korea";
		case 128: return "Iran";
		case 129: return "Vietnam";
		case 130: return "Malaysia";
		case 131: return "Saudi Arabia";
		case 132: return "Yemen";
		case 133: return "Kuwait";
		case 134: return "Laos";
		case 135: return "North Korea";
		case 136: return "Oman";
		case 137: return "Pakistan";
		case 138: return "Philippines";
		case 139: return "China";
		case 140: return "Singapore";
		case 141: return "Mauritius";
		case 142: return "Burma";
		case 143: return "Papua New Guinea";
		case 144: return "Thailand";
		case 145: return "Uzbekistan";
		case 146: return "Qatar";
		case 147: return "United Arab Emirates";
		case 148: return "Australia";
		case 149: return "New Zealand";
		case 150: return "Fiji";
		case 151: return "Solomon Islands";
		case 152: default: return "Unknown";
	}
}

const char* leagueLevelToString(int i)
{
	switch(i)
	{
		case 0: return "1. League"; break;
		case 1: return "2. League"; break;
		case 2: return "3. League"; break;
		case 3: return "4. League"; break;
	}
	return "Non-League";
}

int FreekickWriter::write()
{
	Soccer::TeamDatabase teamdb;
	Soccer::PlayerDatabase playerdb;

	for(auto& st : mTeams) {
		std::shared_ptr<Soccer::League> league = teamdb.getOrCreateLeague(nationToContinent(st.nation),
				teamNationalityToString(st.nation), leagueLevelToString(st.division));

		printf("%s in %s - %s - %s\n", st.team_name, leagueLevelToString(st.division),
				teamNationalityToString(st.nation), nationToContinent(st.nation));

		std::vector<std::shared_ptr<Soccer::Player>> players;

		for(auto& sp : st.players) {
			Soccer::PlayerPosition plpos;
			switch(sp.field_position) {
				case 0:
					plpos = Soccer::PlayerPosition::Goalkeeper; break;

				case 1: // right back
				case 2: // left back
				case 3: // defender
					plpos = Soccer::PlayerPosition::Defender; break;

				case 4: // right wing
				case 5: // left wing
				case 6: // midfielder
					plpos = Soccer::PlayerPosition::Midfielder; break;

				case 7: // attacker
				default:
					plpos = Soccer::PlayerPosition::Forward; break;
			}

			Soccer::PlayerSkills plskills;
			plskills.KickPower   = (rand() % 800 + 200) * 0.001f * std::min(49, sp.value + 1) / 49.0f;
			plskills.BallControl = (rand() % 800 + 200) * 0.001f * std::min(49, sp.value + 1) / 49.0f;
			plskills.RunSpeed    = (rand() % 800 + 200) * 0.001f * std::min(49, sp.value + 1) / 49.0f;

			std::shared_ptr<Soccer::Player> pl(new Soccer::Player(sp.id,
						mScramble ? scramble(sp.player_name).c_str() : sp.player_name,
						plpos, plskills));
			players.push_back(pl);
			playerdb.insert(std::make_pair(pl->getId(), pl));
		}

		std::shared_ptr<Soccer::Team> t(new Soccer::Team(mCurrentTeamId,
					mScramble ? scramble(st.team_name).c_str() : st.team_name, players));
		league->addT(t);
		mCurrentTeamId++;
	}
	
	Soccer::DataExchange::createTeamDatabase(std::string(mOutputDir + "/Teams.xml").c_str(), teamdb);
	Soccer::DataExchange::createPlayerDatabase(std::string(mOutputDir + "/Players.xml").c_str(), playerdb);

	return 0;
}

class App {
	public:
		App(const std::string& outputDir, const std::string& teamFile,
				bool scramble, const std::vector<std::string>& inputFiles);
		int convert();
	private:
		bool parse_input_file(const std::string& filename);

		std::string mOutputDir;
		std::string mTeamFile;
		std::vector<std::string> mInputFiles;
		int mCurrentPlayerId;
		std::vector<s_team> mTeams;
		bool mScramble;
};

App::App(const std::string& outputDir, const std::string& teamFile,
		bool scramble, const std::vector<std::string>& inputFiles)
	: mOutputDir(outputDir),
	mTeamFile(teamFile),
	mInputFiles(inputFiles),
	mCurrentPlayerId(1),
	mScramble(scramble)
{
}

int App::convert()
{
	bool parsedAtLeastOne = false;
	for(auto& t : mInputFiles) {
		if(parse_input_file(t))
			parsedAtLeastOne = true;
	}

	if(parsedAtLeastOne) {
		FreekickWriter w(mOutputDir, mTeamFile, mScramble, mTeams);
		return w.write();
	}
	else {
		return 1;
	}
}

bool App::parse_input_file(const std::string& filename)
{
	unsigned char c;
	int num_teams;

	std::ifstream in(filename);
	if(!in.is_open())
	{
		std::cerr << "Warning: could not open input file " << filename << " - skipping.\n";
		return false;
	}
	std::cerr << "Parsing file " << filename << " . . . ";
	c = in.get();
	if(c != 0)
	{
		std::cerr << "Error (general): position 0 failed.\n";
		return false;
	}

	c = in.get();
	num_teams = c;
	fprintf(stderr, "%3d teams to parse. ", num_teams);

	for(int i = 0; i < num_teams; i++)
	{
		unsigned char teamblock[team_size];
		in.read((char*)teamblock, team_size);
		if(in.gcount() != team_size)
		{
			std::cerr << "Error (general): team " << (i + 1) << " could not be parsed: EOF.\n";
			break;
		}
		s_team team;
		// std::cerr << "Parsing team " << (i + 1) << endl;
		int retval = SwosParser::parse_team(teamblock, &team, mCurrentPlayerId);
		if(retval != 0)
		{
			std::cerr << "Error (general): team " << (i + 1) << " failed.\n";
			break;
		}
		// std::cerr << "Parsed team " << (i + 1) << endl;
		mCurrentPlayerId += 16;
		mTeams.push_back(team);
	}
	std::cerr << "Success. Player ID at " << mCurrentPlayerId << ".\n";
	return true;
}

void usage(const char* pn)
{
	fprintf(stderr, "Usage: %s [-s] [-t teams mapping file] <output directory> <input file 1> [input file 2 ...]\n",
			pn);
	fprintf(stderr, "\tOutput directory must exist. Existing files will be overwritten.\n");
	fprintf(stderr, "\tInput files are SWOS data files. The country name will be looked up from the\n");
	fprintf(stderr, "\tteam mapping file (defaults to original SWOS teams).\n");
}

int main(int argc, char** argv)
{
	std::string outputDir;
	std::vector<std::string> inputFiles;
	std::string teamFile;

	bool scramble = false;

	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
			exit(0);
		}
		else if(!strcmp(argv[i], "-t")) {
			if(++i >= argc) { fprintf(stderr, "-t requires an argument.\n"); exit(1); }
			teamFile = std::string(argv[i]);
		}
		else if(!strcmp(argv[i], "-s")) {
			scramble = true;
		}
		else {
			if(outputDir.empty()) {
				outputDir = std::string(argv[i]);
			}
			else {
				inputFiles.push_back(std::string(argv[i]));
			}
		}
	}

	if(inputFiles.empty()) {
		usage(argv[0]);
		exit(1);
	}

	int ret = 1;
	try {
		App app(outputDir, teamFile, scramble, inputFiles);
		ret = app.convert();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	catch (...) {
		std::cerr << "Unknown exception.\n";
	}
	return ret;
}
