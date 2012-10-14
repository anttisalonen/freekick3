#ifndef SOCCER_SEASONSCREEN_H
#define SOCCER_SEASONSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Season.h"

#include "soccer/gui/Screen.h"

namespace Soccer {

class SeasonScreen : public Screen {
	public:
		SeasonScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<Season> s);
		virtual void buttonPressed(boost::shared_ptr<Button> button) override;
		virtual const std::string& getName() const override;
		virtual void onReentry() override;

	private:
		void save();
		void addMatchPlan();

		boost::shared_ptr<Season> mSeason;
		static std::string ScreenName;
		boost::shared_ptr<Button> mMatchButton;
		boost::shared_ptr<Button> mNextRoundButton;
		std::vector<boost::shared_ptr<Button>> mMatchPlanLabels;
};

}

#endif

