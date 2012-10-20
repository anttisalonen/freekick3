#include "soccer/gui/TournamentScreen.h"

using namespace Common;

namespace Soccer {

TournamentScreen::TournamentScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulTournament> l,
		bool onlyOneRound)
	: CompetitionScreen(sm, "Tournament", l, onlyOneRound),
	mTournament(l),
	mScrollPosition(0)
{
	mScrollUpButton   = addButton("Prev",       Common::Rectangle(0.25f, 0.04f, 0.20f, 0.04f));
	mScrollDownButton = addButton("Next",       Common::Rectangle(0.25f, 0.83f, 0.20f, 0.04f));
	mScrollUpButton->hide();
	mScrollDownButton->hide();
	updateScreenElements();
}

void TournamentScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Prev" && mScrollPosition) {
		mScrollPosition--;
		updateScreenElements();
	} else if(buttonText == "Next") {
		mScrollPosition++;
		updateScreenElements();
	} else {
		setScrollPositionToActiveGroup();
		CompetitionScreen::buttonPressed(button);
	}
}

void TournamentScreen::saveCompetition(boost::archive::binary_oarchive& oa) const
{
	oa << mTournament;
}

void TournamentScreen::setScrollPositionToActiveGroup()
{
	if(!mScrollDownButton->hidden())
		mScrollPosition = mActiveScrollPosition;
}

void TournamentScreen::drawTable()
{
	for(auto lbl : mTableLabels) {
		removeButton(lbl);
	}
	mTableLabels.clear();

	boost::shared_ptr<StatefulTournamentStage> st = mTournament->getCurrentStage();
	unsigned int totalnumrows = 0;
	unsigned int skipgroups = mScrollPosition;
	unsigned int groupindex = 0;
	if(st) {
		const std::vector<boost::shared_ptr<StatefulCompetition>>& groups = st->getGroups();
		for(auto g : groups) {
			groupindex++;
			if(g == st->getCurrentTournamentGroup()) {
				mActiveScrollPosition = groupindex - 1;
			}

			boost::shared_ptr<StatefulLeague> l = boost::dynamic_pointer_cast<StatefulLeague>(g);
			if(l) {
				if(skipgroups) {
					skipgroups--;
					continue;
				}
				unsigned int numteams = l->getNumberOfTeams();
				if(numteams + totalnumrows > 24) {
					if(mScrollPosition)
						mScrollUpButton->show();
					mScrollDownButton->show();
				} else {
					LeagueScreen::drawTable(*this, mTableLabels, *l, 0.05f, 0.09f + totalnumrows * 0.03f);
				}
				totalnumrows += numteams + 2;
			}
		}
	}
	if(!mScrollPosition && totalnumrows <= 24) {
		mScrollUpButton->hide();
		mScrollDownButton->hide();
	}
}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Soccer::TournamentStage);
BOOST_CLASS_EXPORT_IMPLEMENT(Soccer::GroupStage);
BOOST_CLASS_EXPORT_IMPLEMENT(Soccer::KnockoutStage);
BOOST_CLASS_EXPORT_IMPLEMENT(Soccer::StatefulTournamentStage);
BOOST_CLASS_EXPORT_IMPLEMENT(Soccer::StatefulTournament);


