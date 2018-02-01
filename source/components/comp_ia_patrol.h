#pragma once

#include "comp_base.h"
#include "geometry/transform.h"
#include <assert.h>	
#include <string>
#include <map>

using namespace std;

class TCompIAPatrol;
 
typedef void (TCompIAPatrol::*statehandler)();

class TCompIAPatrol : public TCompBase {
	CHandle     h_target;
	std::string target_name;
	float       speed;
	float       curr_yaw = 0.f;
	float       radius = 1.0f;
	bool        is_in_fov;
	float       distance;
	float		alcance;
	float		chase_distance;
	float		min_distance;
	std::vector<VEC3> wpoints;
	VEC3		current_wpoint;
	int			max_wpoints;
	int			current_wpoint_index;

	string state;
	// the states, as maps to functions
	map<string, statehandler>statemap;

	DECL_SIBLING_ACCESS();

public:

  void debugInMenu();
  void update(float dt);
  void load(const json& j, TEntityParseContext& ctx);

  // Dani
  void ChangeState(string);	// state we wish to go to
  void AddState(string, statehandler);

  void IdleState();
  void ChaseState();
  void PatrolState();
};