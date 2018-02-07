#ifndef _AIC_PATROL
#define _AIC_PATROL

#include "ai_controller.h"
#include "entity/common_msgs.h"

class CAIPatrol : public IAIController
{
  std::vector<VEC3> _waypoints;
  int   currentWaypoint;
  int   size;
  float min_distance = 2.5f;
  float orbit_chance = 0.05f;
  float orbit_max = 0.f;
  float orbit_amount = 0.f;
  float life = 50.f;

  DECL_SIBLING_ACCESS();

  void onDamageReceived(const TMsgDamage& msg);

public:
  void load(const json& j, TEntityParseContext& ctx);
  void debugInMenu();

  void IdleState();
  void IdleWarState();
  void SeekWptState();
  void NextWptState();
  void ChaseState();
  void OrbitLeftState();
  void OrbitRightState();
  void StepBackState();
  void HitReceivedState();
  void Init();

  void addWaypoint(VEC3 waypoint) { _waypoints.push_back(waypoint); };
  VEC3 getWaypoint() { return _waypoints[currentWaypoint]; }
  static void registerMsgs();
 
};

#endif