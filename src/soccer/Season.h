#ifndef SOCCERSEASON_H
#define SOCCERSEASON_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Serialization.h"

#include "soccer/League.h"
#include "soccer/Cup.h"
#include "soccer/Match.h"


namespace Soccer {

class Season {
	public:
		Season(boost::shared_ptr<Team> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c);

	private:
		boost::shared_ptr<Team> mTeam;
		boost::shared_ptr<StatefulLeague> mLeague;
		boost::shared_ptr<StatefulCup> mCup;

		friend class boost::serialization::access;
		Season(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mTeam;
			ar & mLeague;
			ar & mCup;
		}
};

}

#endif

