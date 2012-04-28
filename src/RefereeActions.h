#ifndef REFEREEACTIONS_H
#define REFEREEACTIONS_H

class Match;
class Referee;

class RefereeAction {
	public:
		virtual ~RefereeAction() { }
		virtual void applyRefereeAction(Match& match, const Referee& p, double time) = 0;
};

class IdleRA : public RefereeAction {
	public:
		void applyRefereeAction(Match& match, const Referee& p, double time);
};

#endif

