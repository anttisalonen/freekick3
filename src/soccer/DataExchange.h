#ifndef SOCCER_DATAEXCHANGE_H
#define SOCCER_DATAEXCHANGE_H

#include <tinyxml.h>

#include "soccer/Match.h"
#include "soccer/Player.h"

namespace Soccer {

class DataExchange {
	public:
		static std::shared_ptr<Player> parsePlayer(const TiXmlElement* pelem);
		static std::shared_ptr<Match> parseMatchDataFile(const char* fn);
		static void createMatchDataFile(const Match& m, const char* fn);
};


}


#endif

