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
		Season(boost::shared_ptr<StatefulTeam> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c);
		boost::shared_ptr<StatefulTeam> getTeam();
		boost::shared_ptr<StatefulLeague> getLeague();
		boost::shared_ptr<StatefulCup> getCup();

	private:
		boost::shared_ptr<StatefulTeam> mTeam;
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

