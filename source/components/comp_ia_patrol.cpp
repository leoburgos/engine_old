#include "mcv_platform.h"
#include "comp_ia_patrol.h"
#include "modules/module_ia.h"
#include "mcv_platform.h"
#include "entity/entity_parser.h"
#include "comp_transform.h"
#include "geometry\SimpleMath.h"
#include <iostream>

DECL_OBJ_MANAGER("ia_patrol", TCompIAPatrol);

void TCompIAPatrol::debugInMenu() {
	ImGui::Text("State %s", state.c_str());
	ImGui::Text("Distance %f", distance);
	ImGui::Text("In fov %s", is_in_fov ? "Si" : "No");	
}

void TCompIAPatrol::load(const json& j, TEntityParseContext& ctx) {
	alcance = j.value("alcance", 1.0f);
	chase_distance = j.value("chase_distance", 7.0f);
	min_distance = j.value("min_distance", 0.5f);
	max_wpoints = j.value("min_wpoints", 4);
	speed = j.value("speed", 1) * 0.001f;
	current_wpoint_index = 0;
	auto& w_points = j["wpoints"];
	for (auto& json_gs : w_points)
	{
		VEC3 pos = loadVEC3(json_gs);
		wpoints.push_back(pos);
	}
		

	h_target = ctx.findEntityByName(target_name);
	AddState("idle", (statehandler)&TCompIAPatrol::IdleState);
	AddState("chase", (statehandler)&TCompIAPatrol::ChaseState);
	AddState("patrol", (statehandler)&TCompIAPatrol::PatrolState);
	ChangeState("patrol");
	//wpoints.push_back(VEC3(0, 0, 2));
	//wpoints.push_back(VEC3(5, 0, 5));
	//wpoints.push_back(VEC3(-3, 0, 3));
	//wpoints.push_back(VEC3(1, 0, -7));
	current_wpoint = wpoints[0];
}

void TCompIAPatrol::update(float dt) {
	TCompTransform *c_my_transform = get<TCompTransform>();
	distance = Vector3::Distance(c_my_transform->getPosition(), current_wpoint);	
	(this->*statemap[state])();
}

void TCompIAPatrol::ChangeState(std::string newstate)
{
	// try to find a state with the suitable name
	if (statemap.find(newstate) == statemap.end())
	{
		// the state we wish to jump to does not exist. we abort
		exit(-1);
	}
	state = newstate;
}

void TCompIAPatrol::AddState(std::string name, statehandler sh)
{
	// try to find a state with the suitable name
	if (statemap.find(name) != statemap.end())
	{
		// the state we wish to jump to does exist. we abort
		exit(-1);
	}
	statemap[name] = sh;
}

void TCompIAPatrol::IdleState()
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

void TCompIAPatrol::ChaseState()
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

void TCompIAPatrol::PatrolState() {
	TCompTransform *c_my_transform = get<TCompTransform>();
	VEC3 step = current_wpoint - c_my_transform->getPosition();
	step.Normalize();
	VEC3 my_new_pos = c_my_transform->getPosition() + step * speed;
	float y, r, p;
	c_my_transform->getYawPitchRoll(&y, &p, &r);
	if (c_my_transform->isInLeft(current_wpoint))
	{
		y += 0.001f;
	}
	else
	{
		y -= 0.001f;
	}
	c_my_transform->setYawPitchRoll(y, p, r);

	if (distance > min_distance) 
		c_my_transform->setPosition(my_new_pos);
	else {
		current_wpoint_index = current_wpoint_index == max_wpoints - 1 ? 0 : current_wpoint_index + 1;
		current_wpoint = wpoints[current_wpoint_index];
	}
}