#pragma once

#include "comp_base.h"
#include <assert.h>	
#include <string>
#include <map>

using namespace std;

class TCompIA;
typedef void (TCompIA::*statehandler)();


class TCompIA : public TCompBase {
public:
	TCompIA* ai_controller = nullptr;
	CHandle     h_target;
	std::string target_name;
	float       distance;
	float				alcance;
	float				chase_distance;

	string state;
	// the states, as maps to functions
	map<string, statehandler> statemap;

	DECL_SIBLING_ACCESS();

public:
	void debugInMenu();
	void load(const json& j, TEntityParseContext& ctx);
	void update(float dt);
	void ChangeState(string);	// state we wish to go to
	void AddState(string, statehandler);

	void IdleState();
	void FollowState();
	void ChaseState();
};