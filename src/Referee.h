#ifndef REFEREE_H
#define REFEREE_H

#include <memory>

class Match;
class RefereeAction;

class Referee {
	public:
		Referee();
		void setMatch(Match* m);
		std::shared_ptr<RefereeAction> act();
	private:
		Match* mMatch;
};

#endif

