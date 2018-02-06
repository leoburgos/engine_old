#include "mcv_platform.h"
#include "entity/entity_parser.h"
#include "comp_bullet_controller.h"
#include "comp_transform.h"

DECL_OBJ_MANAGER("bullet_controller", TCompBulletController);

void TCompBulletController::debugInMenu() {
  ImGui::DragFloat("Speed", &speed, 0.1f, 0.f, 20.f);  
}

void TCompBulletController::setBulletOwner(const TMsgAssignBulletOwner& msg) {
	h_sender = msg.h_owner;

	TCompTransform * c_my_tmx = get<TCompTransform>();
	CEntity* e_sender = h_sender;
	TCompTransform *c_sender = e_sender->get<TCompTransform>();
	assert(c_sender);
	c_my_tmx->setPosition(c_sender->getPosition());
	c_my_tmx->setRotation(c_sender->getRotation());
}

void TCompBulletController::registerMsgs() {
  DECL_MSG(TCompBulletController, TMsgAssignBulletOwner, setBulletOwner);
}

void TCompBulletController::load(const json& j, TEntityParseContext& ctx) {
  speed = j.value("speed", 1.0f);  
}

void TCompBulletController::update(float dt) {  
  TCompTransform *c_my_transform = get<TCompTransform>();

  VEC3 front = c_my_transform->getFront(); 
  float amount_moved = dt * speed;

  TCompTransform * c_tmx = get<TCompTransform>();
  c_tmx->setPosition(c_tmx->getPosition() + c_tmx->getFront() * amount_moved);
  auto om_tmx = getObjectManager<TCompTransform>();
  CHandle h_near;
  om_tmx->forEach([this, &h_near, c_tmx](TCompTransform* c) {
	  if (c_tmx == c)
		  return;
	  float distance_to_c = VEC3::Distance(c->getPosition(), c_tmx->getPosition());
	  if (distance_to_c < this->collision_radius) {
		  h_near = CHandle(c).getOwner();
	  }
  });

  if (h_near.isValid()) {
	  CEntity* e_near = h_near;

	  TMsgDamage msg;
	  msg.h_sender = h_sender;
	  msg.h_bullet = CHandle(this);
	  e_near->sendMsg(msg);
  }

}



