#include "SpringBonus.h"

SpringBonus::SpringBonus(const Vector2f &position, std::shared_ptr<Camera> camera, const Vector2f &viewSize,
                         AnimationPlayer animationPlayer, AudioPlayer audioPlayer, bool isStatic)
        : Bonus(position, std::move(camera), viewSize, std::move(animationPlayer), std::move(audioPlayer), isStatic) {
    _passthrough = true;
}

void SpringBonus::applyEntity(const std::weak_ptr<PhysicsEntity> &other) {
    Bonus::applyEntity(other);
    // todo: constants
    notifyObservers(100, 25);
}

void SpringBonus::applyEffect() {
    std::shared_ptr<PhysicsEntity> affected_entity = _affected_entity.lock();
    if (_active && affected_entity) {
        affected_entity->setVelocity({0, (2 * (constants::player::jump_height *
                                               constants::bonus::spring_jump_velocity_multiplier) /
                                          constants::player::jump_dt)});
    }
    _active = false;
}
