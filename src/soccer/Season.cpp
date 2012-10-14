#include "Season.h"

namespace Soccer {

Season::Season(boost::shared_ptr<Team> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c)
	: mTeam(t),
	mLeague(l),
	mCup(c)
{
}

Season::Season()
{
}

boost::shared_ptr<Team> Season::getTeam()
{
	return mTeam;
}

boost::shared_ptr<StatefulLeague> Season::getLeague()
{
	return mLeague;
}

boost::shared_ptr<StatefulCup> Season::getCup()
{
	return mCup;
}

}

