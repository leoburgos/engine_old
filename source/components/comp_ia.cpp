#include "mcv_platform.h"
#include "comp_ia.h"
#include "modules/module_ia.h"
#include "entity/entity_parser.h"
#include "comp_circular_controller.h"
#include "comp_transform.h"

DECL_OBJ_MANAGER("ia", TCompIA);

void TCompIA::debugInMenu() {
	ImGui::Text("State %s", state.c_str());
	if (h_target.isValid()) {
		if (ImGui::TreeNode("Target")) {
			h_target.debugInMenu();
			ImGui::TreePop();
		}
	}
}

void TCompIA::update(float dt) {
	if (!h_target.isValid())
		return;

	(this->*statemap[state])();

}

void TCompIA::load(const json& j, TEntityParseContext& ctx) {
	radius = j.value("radius", 1.0f);
	speed = j.value("speed", 1.0f);
	target_name = j.value("target", "");
	h_target = ctx.findEntityByName(target_name);

	// insert all states in the map
	AddState("idle", (statehandler)&TCompIA::IdleState);
	AddState("follow", (statehandler)&TCompIA::FollowState);

	// reset the state
	ChangeState("idle");
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
	if (GetAsyncKeyState('Q') != 0) ChangeState("follow");
}


void TCompIA::FollowState()
{
	if (GetAsyncKeyState('E') != 0) ChangeState("idle");

	curr_yaw += speed * 5;

	CEntity* e_target = h_target;
	assert(e_target);

	TCompTransform *c_target = e_target->get<TCompTransform>();
	assert(c_target);

	TCompTransform *c_my_transform = get<TCompTransform>();

	float my_y = c_my_transform->getPosition().y;
	//VEC3 my_new_pos = c_target->getPosition() + getVectorFromYaw(curr_yaw) * radius;
	//my_new_pos.y = my_y;

	assert(c_my_transform);
	c_my_transform->lookAt(c_my_transform->getPosition(), c_target->getPosition());
}