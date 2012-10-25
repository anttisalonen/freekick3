#ifndef SOCCER_SEASONSCREEN_H
#define SOCCER_SEASONSCREEN_H

#include <tuple>
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

		typedef std::tuple<CompetitionType, unsigned int, const Round*> RoundTuple;
		RoundTuple getRound(unsigned int i) const;

		boost::shared_ptr<Season> mSeason;
		static std::string ScreenName;
		boost::shared_ptr<Button> mMatchButton;
		boost::shared_ptr<Button> mNextRoundButton;
		boost::shared_ptr<Button> mScrollUpButton;
		boost::shared_ptr<Button> mScrollDownButton;
		boost::shared_ptr<Button> mFinishButton;
		std::vector<boost::shared_ptr<Button>> mMatchPlanLabels;

		unsigned int mPlanPos;
};

}

#endif

