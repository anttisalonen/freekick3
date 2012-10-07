#include <algorithm>
#include <fstream>

#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "soccer/Match.h"
#include "soccer/DataExchange.h"
#include "soccer/gui/Menu.h"
#include "soccer/gui/CupScreen.h"

namespace Soccer {

CupScreen::CupScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulCup> l)
	: CompetitionScreen(sm, "Cup", l)
{
	updateScreenElements();
}

}


