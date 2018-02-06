#include "mcv_platform.h"
#include "entity/entity_parser.h"
#include "comp_player_controller.h"
#include "comp_transform.h"

DECL_OBJ_MANAGER("player_controller", TCompPlayerController);

void TCompPlayerController::debugInMenu() {  
}

void TCompPlayerController::load(const json& j, TEntityParseContext& ctx) {
	speed = j.value("speed", 1.0f);
	rotation_speed = j.value("rotation_speed", 1.0f);
}

void TCompPlayerController::update(float dt) {

	TCompTransform *c_my_transform = get<TCompTransform>();

	float current_yaw = 0.0f;
	float current_pitch = 0.0f;
	float ammount_rotated = dt * rotation_speed;
	float ammount_moved = dt * speed;
	c_my_transform->getYawPitchRoll(&current_yaw, &current_pitch);

	VEC3 local_speed = VEC3::Zero;
	if (isPressed('A')) {
		local_speed.x -= 1.f;
	}
	if (isPressed('S')) {
		local_speed.z += 1.f;
	}
	if (isPressed('D')) {
		local_speed.x += 1.f;
	}
	if (isPressed('W')) {
		local_speed.z -= 1.f;
	}
	if (isPressed('Q')) {
		current_yaw += ammount_rotated;
	}
	if (isPressed('E')) {
		current_yaw -= ammount_rotated;
	}

	assert(c_my_transform);

	// Sets rotation
	VEC3 world_speed = VEC3::Transform(local_speed, c_my_transform->asMatrix());
	VEC3 my_new_pos = c_my_transform->getPosition() + local_speed * ammount_moved;
	c_my_transform->setYawPitchRoll(current_yaw, current_pitch, 0.f);

	// Sets new position
	c_my_transform->setPosition(my_new_pos);
}

