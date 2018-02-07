#include "mcv_platform.h"
#include "handle/handle.h"
#include "ai_patrol.h"
#include "entity/entity_parser.h"
#include "components/comp_transform.h"
#include "render/render_utils.h"
#include <ctime>
#include <cstdlib>

DECL_OBJ_MANAGER("ai_patrol", CAIPatrol);

void CAIPatrol::registerMsgs() {
	DECL_MSG(CAIPatrol, TMsgDamage, onDamageReceived);
}

void CAIPatrol::Init()
{
  // insert all states in the map
  AddState("idle", (statehandler)&CAIPatrol::IdleState);
  AddState("seekwpt", (statehandler)&CAIPatrol::SeekWptState);
  AddState("nextwpt", (statehandler)&CAIPatrol::NextWptState);
  AddState("chase", (statehandler)&CAIPatrol::ChaseState);
  AddState("idlewar", (statehandler)&CAIPatrol::IdleWarState);
  AddState("orbitleft", (statehandler)&CAIPatrol::OrbitLeftState);
  AddState("orbitright", (statehandler)&CAIPatrol::OrbitRightState);
  AddState("stepback", (statehandler)&CAIPatrol::StepBackState);
  AddState("hitreceived", (statehandler)&CAIPatrol::HitReceivedState);

  // reset the state
  ChangeState("idle");
  currentWaypoint = 0;
}

void CAIPatrol::debugInMenu() {
	ImGui::Text("Life %f", life);
  if (ImGui::TreeNode("Waypoints")) {
    for (auto& v : _waypoints) {
      ImGui::PushID(&v);
      ImGui::DragFloat3("Point", &v.x, 0.1f, -20.f, 20.f);
      ImGui::PopID();
    }
    ImGui::TreePop();
  }

  for (size_t i = 0; i < _waypoints.size(); ++i)
    renderLine(_waypoints[i], _waypoints[(i + 1) % _waypoints.size()], VEC4(0, 1, 0, 1));
}

void CAIPatrol::load(const json& j, TEntityParseContext& ctx) {
  setEntity(ctx.current_entity);
  size = j.value("size", 1.0f);
  life = j.value("life", 50.0f);
  Init();

  auto& j_waypoints = j["waypoints"];
  for (auto it = j_waypoints.begin(); it != j_waypoints.end(); ++it) {
    VEC3 p = loadVEC3(it.value());
    addWaypoint(p);
  }

}

void CAIPatrol::IdleState()
{
  CEntity *player = (CEntity *)getEntityByName("The Player");
  TCompTransform *mypos = getMyTransform();
  TCompTransform *ppos = player->get<TCompTransform>();
  bool in_fov = mypos->isInFov(ppos->getPosition(), deg2rad(60));
  if (in_fov) ChangeState("seekwpt");
}

void CAIPatrol::IdleWarState()
{
	CEntity *player = (CEntity *)getEntityByName("The Player");
	TCompTransform *mypos = getMyTransform();
	TCompTransform *ppos = player->get<TCompTransform>();

	float distance_to_c = VEC3::Distance(mypos->getPosition(), ppos->getPosition());
	if (distance_to_c > min_distance + 2) {
		ChangeState("chase");
	}
	int random = rand() % 10; 
	if (random > orbit_chance * 10) {
		int side = rand() % 2;
		orbit_amount = 0;
		orbit_max = deg2rad(30 + (rand() % 30));
		if (side)
			ChangeState("orbitright");
		else
			ChangeState("orbitleft");
	}

}

void CAIPatrol::OrbitLeftState()
{
	CEntity *player = (CEntity *)getEntityByName("The Player");
	TCompTransform *mypos = getMyTransform();
	TCompTransform *ppos = player->get<TCompTransform>();

	float distance_to_c = VEC3::Distance(mypos->getPosition(), ppos->getPosition());
	if (distance_to_c > min_distance + 2) {
		ChangeState("chase");
	}
	if (distance_to_c < 2) {
		ChangeState("stepback");
	}

	VEC3 move_vector;
	move_vector = ppos->getPosition() - mypos->getPosition();
	float y, r, p;
	mypos->getYawPitchRoll(&y, &p, &r);

	mypos->setPosition(ppos->getPosition());
	y -= 0.001;
	orbit_amount += 0.001;
	mypos->setYawPitchRoll(y, p, r);

	mypos->setPosition(mypos->getPosition() - mypos->getFront() * distance_to_c);
	if (orbit_amount > orbit_max)
		ChangeState("idlewar");	
}

void CAIPatrol::OrbitRightState()
{
	CEntity *player = (CEntity *)getEntityByName("The Player");
	TCompTransform *mypos = getMyTransform();
	TCompTransform *ppos = player->get<TCompTransform>();

	float distance_to_c = VEC3::Distance(mypos->getPosition(), ppos->getPosition());
	if (distance_to_c > min_distance + 2) {
		ChangeState("chase");
		return;
	}
	if (distance_to_c < 2.5f) {
		ChangeState("stepback");
		return;
	}

	VEC3 move_vector;
	move_vector = ppos->getPosition() - mypos->getPosition();
	float y, r, p;
	mypos->getYawPitchRoll(&y, &p, &r);

	mypos->setPosition(ppos->getPosition());
	y += 0.001;
	orbit_amount += 0.001;
	mypos->setYawPitchRoll(y, p, r);

	mypos->setPosition(mypos->getPosition() - mypos->getFront() * distance_to_c);
	if (orbit_amount > orbit_max)
		ChangeState("idlewar");
	
}

void CAIPatrol::StepBackState()
{
	TCompTransform *mypos = getMyTransform();
	CEntity *player = (CEntity *)getEntityByName("The Player");
	TCompTransform *ppos = player->get<TCompTransform>();
	
	float y, r, p;
	mypos->getYawPitchRoll(&y, &p, &r);
	if (mypos->isInLeft(ppos->getPosition()))
	{
		y += 0.001f;
	}
	else
	{
		y -= 0.001f;
	}
	mypos->setYawPitchRoll(y, p, r);
	VEC3 vp = mypos->getPosition();
	VEC3 vfwd = mypos->getFront();
	vfwd.Normalize();
	vp = vp - 0.01f*vfwd;
	mypos->setPosition(vp);

	float distance_to_c = VEC3::Distance(mypos->getPosition(), ppos->getPosition());
	if (distance_to_c > 3) {
		ChangeState("idlewar");
	}
}

void CAIPatrol::SeekWptState()
{
  TCompTransform *mypos = getMyTransform();
  float y, r, p;
  mypos->getYawPitchRoll(&y, &p, &r);
  if (mypos->isInLeft(getWaypoint()))
  {
    y += 0.001f;
  }
  else
  {
    y -= 0.001f;
  }
  mypos->setYawPitchRoll(y, p, r);
  VEC3 vp = mypos->getPosition();
  VEC3 vfwd = mypos->getFront();
  vfwd.Normalize();
  vp = vp + 0.001f*vfwd;
  mypos->setPosition(vp);
  // next wpt
  if (VEC3::Distance(getWaypoint(), vp) < 1) ChangeState("nextwpt");

  // chase
  CEntity *player = (CEntity *)getEntityByName("The Player");
  TCompTransform *ppos = player->get<TCompTransform>();
  bool in_fov = mypos->isInFov(ppos->getPosition(), deg2rad(60));
  float distance_to_c = VEC3::Distance(mypos->getPosition(), ppos->getPosition());
  if (in_fov && distance_to_c < 5) ChangeState("chase");
}

void CAIPatrol::NextWptState()
{
  currentWaypoint = (currentWaypoint + 1) % _waypoints.size();
  ChangeState("seekwpt");
}

void CAIPatrol::ChaseState()
{
  TCompTransform *mypos = getMyTransform();
  CEntity *player = (CEntity *)getEntityByName("The Player");
  TCompTransform *ppos = player->get<TCompTransform>();

  float distance_to_c = VEC3::Distance(mypos->getPosition(), ppos->getPosition());
  if (distance_to_c > 5) {
	  ChangeState("seekwpt");
	  return;
  }
  if (distance_to_c <= min_distance) {
	  ChangeState("idlewar");
	  return;
  }

  float y, r, p;
  mypos->getYawPitchRoll(&y, &p, &r);
  if (mypos->isInLeft(ppos->getPosition()))
  {
    y += 0.001f;
  }
  else
  {
    y -= 0.001f;
  }
  mypos->setYawPitchRoll(y, p, r);
  VEC3 vp = mypos->getPosition();
  VEC3 vfwd = mypos->getFront();
  vfwd.Normalize();
  vp = vp + 0.001f*vfwd;
  mypos->setPosition(vp);

  bool in_fov = mypos->isInFov(ppos->getPosition(), deg2rad(60));
  if (!in_fov) ChangeState("seekwpt");

  //min_distance = ((TCompPlayerController*)player + size) / 2;

}

void CAIPatrol::onDamageReceived(const TMsgDamage& msg) {
	ChangeState("hitreceived");
}

void CAIPatrol::HitReceivedState() {
	life -= 10;
	if (life < 0)
		CHandle(this).getOwner().destroy();
	ChangeState("seekwpt");
}