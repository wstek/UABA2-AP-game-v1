#ifndef GAMEENGINE_IENTITYVIEWCREATOR_H
#define GAMEENGINE_IENTITYVIEWCREATOR_H


#include <memory>
#include <vector>
#include <map>
#include "Entity.h"
#include "ui/TextBox.h"
#include "../animation/Animation.h"

class IEntityViewCreator {
public:
    virtual void loadTextureGroup(const std::string &texture_resource_name,
                                  const std::vector<std::string> &texture_filenames) = 0;

    virtual void createEntitySpriteView(std::shared_ptr<Entity> entity, unsigned int layer) = 0;

    virtual void createEntityTextView(std::shared_ptr<TextBox> entity_text_box) = 0;
};


#endif //GAMEENGINE_IENTITYVIEWCREATOR_H
