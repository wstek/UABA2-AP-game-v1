#ifndef GAMEENGINE_ENTITYVIEW_H
#define GAMEENGINE_ENTITYVIEW_H


#include <memory>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
#include "core/observer_pattern_interface/IObserver.h"
#include "core/entities/Entity.h"
#include "core/animation/AnimationStateMachine.h"
#include "core/math/math_funcs.h"
#include <filesystem>

class EntityView : public IObserver {
protected:
    std::weak_ptr<Entity> _entity;

public:
    explicit EntityView(std::weak_ptr<Entity> entity);

    ~EntityView() = default;
};


#endif //GAMEENGINE_ENTITYVIEW_H