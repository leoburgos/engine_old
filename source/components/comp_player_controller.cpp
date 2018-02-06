#include "mcv_platform.h"
#include "entity/entity_parser.h"
#include "comp_player_controller.h"
#include "comp_transform.h"

DECL_OBJ_MANAGER("player_controller", TCompPlayerController);

void TCompPlayerController::debugInMenu() {
  
}

void TCompPlayerController::load(const json& j, TEntityParseContext& ctx) {
}

void TCompPlayerController::update(float dt) {

	TCompTransform *c_my_transform = get<TCompTransform>();

	VEC3 my_new_pos = c_my_transform->getPosition();

	if (GetAsyncKeyState('A') != 0) {
		my_new_pos = VEC3(my_new_pos.x -0.01f, my_new_pos.y, my_new_pos.z);
	}
	if (GetAsyncKeyState('S') != 0) {
		my_new_pos = VEC3(my_new_pos.x, my_new_pos.y, my_new_pos.z + 0.01f);
	}
	if (GetAsyncKeyState('D') != 0) {
		my_new_pos = VEC3(my_new_pos.x+0.01f, my_new_pos.y, my_new_pos.z);
	}
	if (GetAsyncKeyState('W') != 0) {
		my_new_pos = VEC3(my_new_pos.x, my_new_pos.y, my_new_pos.z - 0.01f);
	}


	assert(c_my_transform);
	c_my_transform->setPosition( my_new_pos);

}

