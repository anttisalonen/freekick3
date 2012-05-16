#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "soccer/DataExchange.h"
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
		FreekickWriter(const std::string& outputDir, const std::vector<s_team>& teams);
		int write();

	private:
		std::string mOutputDir;
		const std::vector<s_team>& mTeams;
		int mCurrentTeamId;
};

FreekickWriter::FreekickWriter(const std::string& outputDir, const std::vector<s_team>& teams)
	 : mOutputDir(outputDir),
	 mTeams(teams),
	 mCurrentTeamId(1)
{
}

int FreekickWriter::write()
{
	Soccer::TeamDatabase teamdb;
	Soccer::PlayerDatabase playerdb;

	for(auto& st : mTeams) {
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
						sp.player_name, plpos, plskills));
			players.push_back(pl);
			playerdb.insert(std::make_pair(pl->getId(), pl));
		}

		teamdb.insert(std::make_pair(mCurrentTeamId, std::shared_ptr<Soccer::Team>(new
					Soccer::Team(mCurrentTeamId,
						st.team_name, players))));
		mCurrentTeamId++;
	}
	
	Soccer::DataExchange::createTeamDatabase(std::string(mOutputDir + "/Teams.xml").c_str(), teamdb);
	Soccer::DataExchange::createPlayerDatabase(std::string(mOutputDir + "/Players.xml").c_str(), playerdb);

	return 0;
}

class App {
	public:
		App(const std::string& outputDir, const std::vector<std::string>& inputFiles);
		int convert();
	private:
		void parse_input_file(std::istream& in);

		std::string mOutputDir;
		std::vector<std::string> mInputFiles;
		int mCurrentPlayerId;
		std::vector<s_team> mTeams;
};

App::App(const std::string& outputDir, const std::vector<std::string>& inputFiles)
	: mOutputDir(outputDir),
	mInputFiles(inputFiles),
	mCurrentPlayerId(1)
{
}

int App::convert()
{
	bool parsedAtLeastOne = false;
	for(auto& t : mInputFiles) {
		std::ifstream input_file(t);
		if(!input_file.is_open())
		{
			std::cerr << "Warning: could not open input file " << t << " - skipping.\n";
			continue;
		}
		std::cerr << "Parsing file " << t << " . . . ";
		parse_input_file(input_file);
		parsedAtLeastOne = true;
	}

	if(parsedAtLeastOne) {
		FreekickWriter w(mOutputDir, mTeams);
		return w.write();
	}
	else {
		return 1;
	}
}

void App::parse_input_file(std::istream& in)
{
	unsigned char c;
	int num_teams;

	c = in.get();
	if(c != 0)
	{
		std::cerr << "Error (general): position 0 failed.\n";
		return;
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
}

void usage(const char* pn)
{
	fprintf(stderr, "Usage: %s <output directory> <input file 1> [input file 2 ...]\n",
			pn);
	fprintf(stderr, "\tOutput directory must exist. Existing files will be overwritten.\n");
	fprintf(stderr, "\tInput files are SWOS data files. The country will be inferred based\n");
	fprintf(stderr, "\ton file name.\n");
}

int main(int argc, char** argv)
{
	std::string outputDir;
	std::vector<std::string> inputFiles;

	if(argc < 3) {
		usage(argv[0]);
		exit(1);
	}
	if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		usage(argv[0]);
		exit(1);
	}

	outputDir = std::string(argv[1]);
	for(int i = 2; i < argc; i++)
		inputFiles.push_back(std::string(argv[i]));

	int ret = 1;
	try {
		App app(outputDir, inputFiles);
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
