#ifndef SOCCER_CUPSCREEN_H
#define SOCCER_CUPSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Cup.h"

#include "soccer/gui/CompetitionScreen.h"
#include "soccer/gui/TeamTacticsScreen.h"

namespace Soccer {

class CupScreen : public CompetitionScreen {
	public:
		CupScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulCup> l,
				bool onlyOneRound = false);

	protected:
		virtual void saveCompetition(boost::archive::binary_oarchive& oa) const override;

	private:
		boost::shared_ptr<StatefulCup> mCup;
};

}

#endif



