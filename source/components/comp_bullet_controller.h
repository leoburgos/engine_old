#pragma once

#include "comp_base.h"
#include "geometry/transform.h"
#include "entity/common_msgs.h"

class TCompBulletController : public TCompBase {

  CHandle     h_sender;
  float       speed = 1.0f;
  float		  collision_radius = 0.25f;

  void setBulletOwner(const TMsgAssignBulletOwner& msg);

  DECL_SIBLING_ACCESS();

public:
  static void registerMsgs();
  void debugInMenu();
  void load(const json& j, TEntityParseContext& ctx);
  void update(float dt);
  
};