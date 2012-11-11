#include "common/Math.h"

#include "match/ai/AITacticParameters.h"

/* make value (between 0 and 1) closer to 1. */
static float mildify(float v)
{
	return pow(v, 0.1f);
}

AITacticParameters::AITacticParameters(const Soccer::StatefulTeam& tt)
{
	const Soccer::TeamTactics& t = tt.getTactics();
	PassActionCoefficient      = mildify(Common::clamp(0.1f, t.FastPassing * (1.0f - t.LongBalls), 0.9f));
	PassRiskCoefficient        = mildify(Common::clamp(0.3f, t.Pressure * 0.5f + 0.25f, 0.7f));
	LongPassActionCoefficient  = mildify(Common::clamp(0.1f, (t.FastPassing + t.LongBalls) * 0.5f, 0.9f));
	DribbleActionCoefficient   = mildify(Common::clamp(0.1f, (1.0f - t.FastPassing) * t.Pressure, 0.9f));
	ShootActionCoefficient     = mildify(Common::clamp(0.1f, 1.0f - t.ShootClose, 0.9f));
	/* This value is excellent for configuring whether the team
	 * tries to score from afar, or perfects the shot position. Setting
	 * this too high (> 0.2) may lead to excessive
	 * trying-to-kick-the-ball-through-opponents. */
	ShootCloseCoefficient      = Common::clamp(0.1f, t.ShootClose * 1.0f, 0.9f);
	ClearActionCoefficient     = Common::clamp(0.3f, (1.0f - t.Pressure) * 0.5f + 0.25f, 0.7f);
	FetchBallActionCoefficient = mildify(Common::clamp(0.1f, t.Pressure, 0.9f));
	GuardActionCoefficient     = Common::clamp(0.1f, t.Pressure, 0.9f);
	BlockActionCoefficient     = Common::clamp(0.1f, t.Pressure, 0.9f);
	BlockPassActionCoefficient = Common::clamp(0.1f, t.Pressure, 0.9f);
	GuardAreaActionCoefficient = Common::clamp(0.1f, 1.0f - t.Pressure, 0.9f);
	TackleActionCoefficient    = Common::clamp(0.1f, t.Pressure, 0.9f);
}

