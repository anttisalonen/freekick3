#include <fstream>

#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "soccer/Match.h"
#include "soccer/Team.h"

#include "soccer/gui/Menu.h"
#include "soccer/gui/LeagueScreen.h"
#include "soccer/gui/CupScreen.h"
#include "soccer/gui/TournamentScreen.h"
#include "soccer/gui/SeasonScreen.h"

namespace Soccer {

SeasonScreen::SeasonScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<Season> s)
	: Screen(sm),
	mSeason(s),
	mPlanPos(0)
{
	addButton("Back",  Common::Rectangle(0.01f, 0.90f, 0.23f, 0.06f));
	addButton("Save",  Common::Rectangle(0.01f, 0.83f, 0.23f, 0.06f));
	mNextRoundButton  = addButton("Next Round",    Common::Rectangle(0.26f, 0.90f, 0.73f, 0.06f));
	mScrollUpButton   = addButton("Prev",          Common::Rectangle(0.65f, 0.04f, 0.20f, 0.04f));
	mScrollDownButton = addButton("Next",          Common::Rectangle(0.65f, 0.83f, 0.20f, 0.04f));
	mFinishButton     = addButton("Finish Season", Common::Rectangle(0.26f, 0.90f, 0.73f, 0.06f));
	mFinishButton->hide();
	addMatchPlan();
}

void SeasonScreen::onReentry()
{
	addMatchPlan();
}

void SeasonScreen::addMatchPlan()
{
	for(auto l : mMatchPlanLabels) {
		removeButton(l);
	}
	mMatchPlanLabels.clear();

	if(mPlanPos == 0)
		mScrollUpButton->hide();
	else
		mScrollUpButton->show();

	float x = 0.75f;
	float y = 0.10f;
	for(unsigned int i = mPlanPos; i < mSeason->getSchedule().size(); i++) {
		RoundTuple ctr = getRound(i);
		std::stringstream ss;
		switch(std::get<0>(ctr)) {
			case CompetitionType::League:
			{
				ss << "League Round " << (std::get<1>(ctr) + 1);
			}
			break;

			case CompetitionType::Cup:
			{
				ss << "Cup Round " << (std::get<1>(ctr) + 1);
			}
			break;

			case CompetitionType::Tournament:
			{
				ss << "Tournament Round " << (std::get<1>(ctr) + 1);
			}
			break;
		}

		/* r might be a null pointer if the participants aren't clear yet (cup) */
		if(std::get<2>(ctr)) {
			for(auto m : std::get<2>(ctr)->getMatches()) {
				if(m->getTeam(0) == mSeason->getTeam() ||
						m->getTeam(1) == mSeason->getTeam()) {
					CompetitionScreen::addMatchLabels(*m, x, y, 0.6f,
							*this, mMatchPlanLabels, false);
					break;
				}
			}
		}

		mMatchPlanLabels.push_back(addLabel(ss.str().c_str(), x - 0.45f, y,
					TextAlignment::MiddleLeft, 0.6f, Common::Color::White));
		y += 0.03f;
		if(y >= 0.80f) {
			if(i != mSeason->getSchedule().size() - 1) {
				mScrollDownButton->show();
			} else {
				mScrollDownButton->hide();
			}
			return;
		}
	}
	mScrollDownButton->hide();
}

std::string SeasonScreen::ScreenName = std::string("Season Screen");

void SeasonScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreensUntil("Main Menu");
	}
	else if(buttonText == "Save") {
		save();
	}
	else if(buttonText == "Next Round") {
		unsigned int i = 0;
		for(i = 0; i < mSeason->getSchedule().size(); i++) {
			RoundTuple ct = getRound(i);
			assert(std::get<2>(ct));
			assert(std::get<2>(ct)->getMatches().size());
			if(!std::get<2>(ct)->getMatches()[std::get<2>(ct)->getMatches().size() - 1]->getResult().Played) {
				if(i > 15)
					mPlanPos = i - 10;
				else
					mPlanPos = 0;

				switch(std::get<0>(ct)) {
					case CompetitionType::League:
					{
						mScreenManager->addScreen(boost::shared_ptr<Screen>(new LeagueScreen(mScreenManager,
										mSeason->getLeague(),
										true)));
					}
					break;

					case CompetitionType::Cup:
					{
						mScreenManager->addScreen(boost::shared_ptr<Screen>(new CupScreen(mScreenManager,
										mSeason->getCup(),
										true)));
					}
					break;

					case CompetitionType::Tournament:
					{
						mScreenManager->addScreen(boost::shared_ptr<Screen>(new TournamentScreen(mScreenManager,
										mSeason->getTournament(),
										true)));
					}
					break;
				}
				break;
			}
		}

		if(i >= mSeason->getSchedule().size() - 1) {
			/* no more matches */
			mNextRoundButton->hide();
			if(mSeason->getLeagueSystem())
				mFinishButton->show();
			/* TODO: display season summary */
		}
	} else if(buttonText == "Finish Season") {
		assert(mSeason->getLeagueSystem());
		mSeason->getLeagueSystem()->promoteAndRelegateTeams();
		mSeason = Season::createSeason(mSeason->getTeam(), mSeason->getLeagueSystem());
		mFinishButton->hide();
		mNextRoundButton->show();
		mPlanPos = 0;
		addMatchPlan();
	} else if(buttonText == "Prev") {
		if(mPlanPos < 10)
			mPlanPos = 0;
		else
			mPlanPos -= 10;
		addMatchPlan();
	} else if(buttonText == "Next") {
		mPlanPos += 10;
		addMatchPlan();
	}
}

const std::string& SeasonScreen::getName() const
{
	return ScreenName;
}

void SeasonScreen::save()
{
	std::string filename(Menu::getSaveDir());
	if(mSeason->getLeagueSystem())
		filename += "/Career.sav";
	else
		filename += "/Season.sav";
	std::ofstream ofs(filename, std::ios::out | std::ios::binary | std::ios::trunc);
	boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
	out.push(boost::iostreams::bzip2_compressor());
	out.push(ofs);
	boost::archive::binary_oarchive oa(out);
	oa << mSeason;
	std::cout << "Saved to " << filename << "\n";
}

SeasonScreen::RoundTuple SeasonScreen::getRound(unsigned int i) const
{
	auto ss = mSeason->getSchedule();
	if(ss.size() <= i) {
		return RoundTuple(CompetitionType::League, 0, nullptr);
	}

	auto ct = ss[i];

	switch(ct.first) {
		case CompetitionType::League:
		{
			const Schedule& s = mSeason->getLeague()->getSchedule();
			const Round* r = s.getRound(ct.second);
			return RoundTuple(CompetitionType::League, ct.second, r);
		}
		break;

		case CompetitionType::Cup:
		{
			const Schedule& s = mSeason->getCup()->getSchedule();
			const Round* r = s.getRound(ct.second);
			return RoundTuple(CompetitionType::Cup, ct.second, r);
		}
		break;

		case CompetitionType::Tournament:
		{
			const Schedule& s = mSeason->getTournament()->getSchedule();
			const Round* r = s.getRound(ct.second);
			return RoundTuple(CompetitionType::Tournament, ct.second, r);
		}
		break;
	}

	assert(0);
	return RoundTuple(CompetitionType::League, 0, nullptr);
}

}

