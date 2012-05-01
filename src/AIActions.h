#ifndef AIACTIONS_H
#define AIACTIONS_H

#include <string>
#include <memory>
#include <vector>

#include "Player.h"
#include "PlayerActions.h"

class AIAction;

class AIActionChooser {
	public:
		AIActionChooser(const std::vector<std::shared_ptr<AIAction>>& actions);
		std::shared_ptr<AIAction> getBestAction();
	private:
		std::shared_ptr<AIAction> mBestAction;
};

class AIAction {
	public:
		AIAction(const char* name, const Player* p);
		std::shared_ptr<PlayerAction> getAction() const;
		double getScore() const;
		const char* getName() const;
	protected:
		const char* mName;
		const Player* mPlayer;
		double mScore;
		std::shared_ptr<PlayerAction> mAction;
};

class AINullAction : public AIAction {
	public:
		AINullAction(const Player* p);
		static const char* mActionName;
};

class AIShootAction : public AIAction {
	public:
		AIShootAction(const Player* p);
		static const char* mActionName;
};

class AIDribbleAction : public AIAction {
	public:
		AIDribbleAction(const Player* p);
		static const char* mActionName;
};

class AIPassAction : public AIAction {
	public:
		AIPassAction(const Player* p);
		static const char* mActionName;
};

#endif
