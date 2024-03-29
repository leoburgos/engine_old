#include "mcv_platform.h"
#include "entity.h"
#include "entity_parser.h"
#include "components/comp_name.h"

DECL_OBJ_MANAGER("entity", CEntity);

void CEntity::set(uint32_t comp_type, CHandle new_comp) {
  assert(comp_type < CHandle::max_types);
  assert(!comps[comp_type].isValid());
  comps[comp_type] = new_comp;
  new_comp.setOwner(CHandle(this));
}

const char* CEntity::getName() const {
  TCompName* comp_name = get<TCompName>();
  return comp_name->getName();
}

void CEntity::debugInMenu() {
  if (ImGui::TreeNode(getName())) {

    for (int i = 0; i < CHandle::max_types; ++i) {
      CHandle h = comps[i];
      if (h.isValid()) {

        // Open a tree using the name of the component
        if (ImGui::TreeNode(h.getTypeName())) {
          // Do the real show details of the component
          h.debugInMenu();
          ImGui::TreePop();
        }
      }
    }

    ImGui::TreePop();
  }
}

void CEntity::load(const json& j, TEntityParseContext& ctx) {

  for (auto it = j.begin(); it != j.end(); ++it) {

    auto& comp_name = it.key();
    auto& comp_json = it.value();

    auto om = CHandleManager::getByName(comp_name.c_str());
    if (!om) {
      fatal("While parsing file %s. Invalid component named '%s'", ctx.filename.c_str(), comp_name.c_str());
    }
    assert(om);
    
    int comp_type = om->getType();      // 7

    // This is my current handle of this type of component
    CHandle h_comp = comps[comp_type];
    if (h_comp.isValid()) {
      // Give an option to reconfigure the existing comp with the new json
      h_comp.load(comp_json, ctx);
    }
    else 
    {
      // Get a new fresh component of this type for me
      h_comp = om->createHandle();

      // Initialize the comp from the json. You still don't have an owner
      h_comp.load(comp_json, ctx);

      // Bind it to me
      set(comp_type, h_comp);
    }

  }

  // Send a msg to the entity components to let them know
  // the entity is fully loaded.

}
