#pragma once

#include "comp_base.h"
#include "geometry/transform.h"

class TCompPlayerController : public TCompBase {

  DECL_SIBLING_ACCESS();

public:
  void debugInMenu();
  void load(const json& j, TEntityParseContext& ctx);
  void update(float dt);
};