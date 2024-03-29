#include "PhysicsEntity.h"

PhysicsEntity::PhysicsEntity(const Vector2f &position, std::shared_ptr<Camera> camera, const Vector2f &viewSize,
                             AnimationPlayer animation_player, AudioPlayer audio_player, bool is_static) :
        Entity(position, std::move(camera), viewSize, std::move(animation_player), std::move(audio_player)),
        _is_static(is_static), _mass(1), _gravitational_acceleration({0, 0}), _passthrough(false), _collided(false),
        _max_hit_points(0), _current_hit_points(0), _can_shoot(true), _shoot_delay_time_passed(0) {
    _hitbox = std::make_shared<Hitbox>(_position, _view_size);

}

void PhysicsEntity::setupPlayerPhysics(float jump_dt, float jump_height) {
    _gravitational_acceleration = {0, -2 * jump_height / (jump_dt * jump_dt)};
    _min_neg_velocity = {-1.5f, -(constants::player::jump_velocity * 1.5f)};
    _max_pos_velocity = {1.5f, constants::player::jump_velocity * 5.f};
    _drag = {0.15f, 0};
    _friction = {5.f, 0};
}

void PhysicsEntity::update(double t, float dt) {
    if (_is_static) {
        Entity::update(t, dt);
        return;
    }

    // update hp_bar
    if (_hp_bar) {
        _hp_bar->update(t, dt);
    }

    // shooting
    if (!_can_shoot) {
        _shoot_delay_time_passed += dt;
    }

    if (_shoot_delay_time_passed > constants::bullet::time_delay) {
        _shoot_delay_time_passed = 0;
        _can_shoot = true;
    }

    // max velocity
    _velocity = {std::clamp(_velocity.x, _min_neg_velocity.x, _max_pos_velocity.x),
                 std::clamp(_velocity.y, _min_neg_velocity.y, _max_pos_velocity.y)};

    // force acceleration
    _acceleration += _force / _mass;

    // position(t): position + velocity * timestep + (acceleration / 2 * timestep^2)
    move(_velocity * dt + _acceleration / 2 * dt * dt);
    // verlet integration
    _velocity += _acceleration * dt;

    // todo: make min_value a constant
    float min_value = 1.e-4f;
    // remove small velocities
    if (_velocity.x < min_value && _velocity.x > -min_value)
        _velocity.x = 0;
    if (_velocity.y < min_value && _velocity.y > -min_value)
        _velocity.y = 0;

    // clear forces/accelerations
    _force = {0, 0};
    _acceleration = {0, 0};

    Entity::update(t, dt);
}

void PhysicsEntity::setPosition(const Vector2f &position) {
    if (_hp_bar) {
        _hp_bar->move(position - _position);
    }

    Entity::setPosition(position);
}

bool PhysicsEntity::isIsStatic() const {
    return _is_static;
}

void PhysicsEntity::setIsStatic(bool isStatic) {
    _is_static = isStatic;
}

float PhysicsEntity::getMass() const {
    return _mass;
}

void PhysicsEntity::setMass(float mass) {
    _mass = mass;
}

const Vector2f &PhysicsEntity::getForce() const {
    return _force;
}

void PhysicsEntity::setForce(const Vector2f &force) {
    _force = force;
}

void PhysicsEntity::addForce(const Vector2f &force) {
    _force += force;
}

const Vector2f &PhysicsEntity::getVelocity() const {
    return _velocity;
}

void PhysicsEntity::setVelocity(const Vector2f &velocity) {
    _velocity = velocity;
}

void PhysicsEntity::addVelocity(const Vector2f &velocity) {
    _velocity += velocity;
}

const Vector2f &PhysicsEntity::getAcceleration() const {
    return _acceleration;
}

void PhysicsEntity::setAcceleration(const Vector2f &acceleration) {
    _acceleration = acceleration;
}

const Vector2f &PhysicsEntity::getGravitationalAcceleration() const {
    return _gravitational_acceleration;
}

const Vector2f &PhysicsEntity::getMaxVelocity() const {
    return _min_neg_velocity;
}

const Vector2f &PhysicsEntity::getDrag() const {
    return _drag;
}

const Vector2f &PhysicsEntity::getFriction() const {
    return _friction;
}

void PhysicsEntity::applyGravity() {
    _acceleration += _gravitational_acceleration;
}

void PhysicsEntity::applyFriction() {
    Vector2f friction_force = {_velocity.x * _friction.x, _velocity.y * _friction.y};
    if (_velocity.length() < 0.1) {
        friction_force *= 3;
    }
    _acceleration -= friction_force;
}

void PhysicsEntity::applyDrag() {
    Vector2f drag_force = _velocity.x * _velocity.x * _drag;
    _acceleration -= drag_force;
}

bool PhysicsEntity::isPassthrough() const {
    return _passthrough;
}

void PhysicsEntity::setPassthrough(bool passthrough) {
    _passthrough = passthrough;
}

void PhysicsEntity::applySideScrolling() {
    if (_position.x <= constants::world_x_min) {
        setPosition({constants::world_x_max, _position.y});
    } else if (_position.x >= constants::world_x_max) {
        setPosition({constants::world_x_min, _position.y});
    }
}

void PhysicsEntity::resolveCollision(PhysicsEntity &other, bool resolve, bool set_collided) {
    Vector2f displacement = _hitbox->getDisplacementToCollision(*other._hitbox);
    Vector2f move_vector;

    Vector2f new_velocity_this = _velocity;
    Vector2f new_velocity_other = other._velocity;

    if (other._passthrough) {
        if (_velocity.y < 0 && displacement.y > 0 && displacement.y < 0.05) {
            move_vector.y = displacement.y;

            if (resolve) {
                move(move_vector);
                _velocity.y = 0;

                updateView();
            }

            if (set_collided) {
                _collided = true;
                other._collided = true;
            }
        }

        return;
    }

    if (set_collided) {
        _collided = true;
        other._collided = true;
    }

    if (!resolve) return;

    if (std::abs(displacement.x) == std::abs(displacement.y)) {
        move_vector = {displacement.x, displacement.y};
    } else if (std::abs(displacement.x) < std::abs(displacement.y)) {
        move_vector = {displacement.x, 0};

        new_velocity_this.x = 0;
        new_velocity_other.x = 0;
    } else {
        move_vector = {0, displacement.y};

        new_velocity_this.y = 0;
        new_velocity_other.y = 0;
    }

    if (!_is_static && other._is_static) {
        // this is dynamic
        move(move_vector);
        _velocity = new_velocity_this;

        updateView();

    } else if (_is_static && !other._is_static) {
        // other is dynamic
        other.move(move_vector * -1);
        other._velocity = new_velocity_other;

        other.updateView();

    } else if (!_is_static && !other._is_static) {
        // both are dynamic
        float alpha;

        if (other._mass < _mass) {
            alpha = other._mass / _mass;
        } else {
            alpha = 1 - (_mass / other._mass);
        }

        move(lerp({0, 0}, move_vector, alpha));
        other.move(lerp({0, 0}, move_vector * -1, 1 - alpha));

        Vector2f new_velocity;

        if (_velocity.length() + other._velocity.length() < (_velocity + other._velocity).length()) {
            new_velocity = _velocity + other._velocity;
        } else {
            new_velocity = _velocity.length() > other._velocity.length() ? _velocity : other._velocity;
        }

        _velocity = new_velocity;
        other._velocity = new_velocity;

        updateView();
        other.updateView();
    }
}

bool PhysicsEntity::isCollided() const {
    return _collided;
}

void PhysicsEntity::setCollided() {
    _collided = true;
}

void PhysicsEntity::disappear() {
    setHitbox({0, 0}, {0, 0});
    setViewSize({0, 0});
    _rays.clear();
    _collided = false;
    _hp_bar = nullptr;
}

unsigned int PhysicsEntity::getMaxHitPoints() const {
    return _max_hit_points;
}

void PhysicsEntity::setMaxHitPoints(unsigned int maxHitPoints) {
    _max_hit_points = maxHitPoints;
}

unsigned int PhysicsEntity::getCurrentHitPoints() const {
    return _current_hit_points;
}

void PhysicsEntity::setCurrentHitPoints(unsigned int currentHitPoints) {
    _current_hit_points = currentHitPoints;
}

void PhysicsEntity::addHitPoints(unsigned int hit_points) {
    _current_hit_points += hit_points;
    if (_current_hit_points > _max_hit_points) {
        _current_hit_points = _max_hit_points;
    }
}

void PhysicsEntity::subtractHitPoints(unsigned int hit_points) {
    if (_current_hit_points <= hit_points) {
        _current_hit_points = 0;
    } else {
        _current_hit_points -= hit_points;
    }
}

bool PhysicsEntity::canShoot() const {
    return _can_shoot;
}

void PhysicsEntity::setCanShoot(bool can_shoot) {
    _can_shoot = can_shoot;
}

void PhysicsEntity::setHPBar(const std::shared_ptr<UIEntity> &hp_bar) {
    _hp_bar = hp_bar;
}