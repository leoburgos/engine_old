#include "mcv_platform.h"
#include "comp_ia.h"
#include "modules/module_ia.h"
#include "mcv_platform.h"
#include "entity/entity_parser.h"
#include "comp_transform.h"
#include "geometry\SimpleMath.h"
#include <iostream>

DECL_OBJ_MANAGER("ia", TCompIA);

void TCompIA::debugInMenu() {
	ImGui::Text("State %s", state.c_str());
	ImGui::Text("Distance %f", distance);
	ImGui::Text("In fov %s", is_in_fov ? "Si" : "No");
	if (h_target.isValid()) {
		if (ImGui::TreeNode("Target")) {
			h_target.debugInMenu();
			ImGui::TreePop();
			
		}
	}
}

void TCompIA::load(const json& j, TEntityParseContext& ctx) {
	target_name = j.value("target", "");
	alcance = j.value("alcance", 1.0f);
	chase_distance = j.value("chase_distance", 1.0f);
	min_distance = j.value("min_distance", 1.0f);

	h_target = ctx.findEntityByName(target_name);
	AddState("idle", (statehandler)&TCompIA::IdleState);
	AddState("follow", (statehandler)&TCompIA::FollowState);
	AddState("chase", (statehandler)&TCompIA::ChaseState);
	ChangeState("idle");
}

void TCompIA::update(float dt) {
	
	if (!h_target.isValid())
		return;

	CEntity* e_target = h_target;
	assert(e_target);

	TCompTransform *c_target = e_target->get<TCompTransform>();
	assert(c_target);

	TCompTransform *c_my_transform = get<TCompTransform>();

	distance = Vector3::Distance(c_my_transform->getPosition(), c_target->getPosition());

	bool in_fov = c_my_transform->isInFov(c_target->getPosition(), deg2rad(60));
	is_in_fov = in_fov;

	(this->*statemap[state])();
}



void TCompIA::ChangeState(std::string newstate)
{
	// try to find a state with the suitable name
	if (statemap.find(newstate) == statemap.end())
	{
		// the state we wish to jump to does not exist. we abort
		exit(-1);
	}
	state = newstate;
}


void TCompIA::AddState(std::string name, statehandler sh)
{
	// try to find a state with the suitable name
	if (statemap.find(name) != statemap.end())
	{
		// the state we wish to jump to does exist. we abort
		exit(-1);
	}
	statemap[name] = sh;
}

void TCompIA::IdleState()
{
	//if (distance < alcance) ChangeState("follow");
	if (!h_target.isValid())
		return;

	CEntity* e_target = h_target;
	assert(e_target);

	TCompTransform *c_target = e_target->get<TCompTransform>();
	assert(c_target);

	TCompTransform *c_my_transform = get<TCompTransform>();

	bool is_in_fov = c_my_transform->isInFov(c_target->getPosition(), deg2rad(60));
	if (is_in_fov && distance <= chase_distance) ChangeState("chase");
}


void TCompIA::FollowState()
{
	if (distance >= alcance) ChangeState("idle");

	if (!h_target.isValid())
		return;

	CEntity* e_target = h_target;
	assert(e_target);

	TCompTransform *c_target = e_target->get<TCompTransform>();
	assert(c_target);

	TCompTransform *c_my_transform = get<TCompTransform>();

	bool is_in_fov = c_my_transform->isInFov(c_target->getPosition(), deg2rad(60));
	if (is_in_fov && distance <= chase_distance) ChangeState("chase");

	assert(c_my_transform);

	c_my_transform->lookAt(c_my_transform->getPosition(), c_target->getPosition());
}



void TCompIA::ChaseState()
{
	if (distance >= chase_distance+2.f) ChangeState("idle");

	if (!h_target.isValid())
		return;

	CEntity* e_target = h_target;
	assert(e_target);

	TCompTransform *c_target = e_target->get<TCompTransform>();
	assert(c_target);

	TCompTransform *c_my_transform = get<TCompTransform>();

	assert(c_my_transform);

	VEC3 step = c_target->getPosition() - c_my_transform->getPosition();
	step.Normalize();
	VEC3 my_new_pos = c_my_transform->getPosition() + step *0.001f;
	float y, r, p;
	c_my_transform->getYawPitchRoll(&y, &p, &r);
	if (c_my_transform->isInLeft(c_target->getPosition()))
	{
		y += 0.001f;
	}
	else
	{
		y -= 0.001f;
	}
	c_my_transform->setYawPitchRoll(y, p, r);

	if (distance > min_distance) c_my_transform->setPosition(my_new_pos);
	

}