#ifndef MATCHSIMULATOR_H
#define MATCHSIMULATOR_H

#include <boost/shared_ptr.hpp>
#include <map>

#include "common/Texture.h"
#include "common/Color.h"
#include "common/Rectangle.h"
#include "common/Vector3.h"

#include "match/MatchGUI.h"
#include "match/Clock.h"

class MatchSimulator : public MatchGUI {
	public:
		MatchSimulator(boost::shared_ptr<Match> match, int ticksPerSec,
				bool randomise);
		bool play();
	private:
		float mFixedFrameTime;
		bool mRandomise;
};

#endif
