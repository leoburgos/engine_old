#pragma once

#include "comp_base.h"
#include "geometry/transform.h"
#include <assert.h>	
#include <string>
#include <map>

using namespace std;

class TCompIA;
 
typedef void (TCompIA::*statehandler)();

class TCompIA : public TCompBase {
	CHandle     h_target;
	std::string target_name;
	float       speed = 1.0f;
	float       curr_yaw = 0.f;
	float       radius = 1.0f;
	float       distance;
	float				alcance;
	float				chase_distance;

	string state;
	// the states, as maps to functions
	map<string, statehandler>statemap;

	DECL_SIBLING_ACCESS();

public:

  IAIController* ai_controller = nullptr;

  void debugInMenu();
  void update(float dt);
  void load(const json& j, TEntityParseContext& ctx);

  // Dani
  void ChangeState(string);	// state we wish to go to
  void AddState(string, statehandler);

  void IdleState();
  void FollowState();
  void ChaseState();
};