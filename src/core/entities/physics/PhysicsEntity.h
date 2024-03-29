#ifndef GAMEENGINE_PHYSICSENTITY_H
#define GAMEENGINE_PHYSICSENTITY_H


#include "../Entity.h"
#include "../../physics/Ray.h"
#include "../ui/UIEntity.h"

class PhysicsEntity : public Entity {
protected:
    bool _is_static;
    bool _collided;

    // physics
    float _mass;
    Vector2f _force;
    Vector2f _velocity;
    Vector2f _acceleration;

    Vector2f _gravitational_acceleration;
    Vector2f _min_neg_velocity;
    Vector2f _max_pos_velocity;
    Vector2f _drag;
    Vector2f _friction;

    bool _passthrough;

    unsigned int _max_hit_points;
    unsigned int _current_hit_points;
    std::shared_ptr<UIEntity> _hp_bar;
    bool _can_shoot;
    float _shoot_delay_time_passed;

public:
    PhysicsEntity(const Vector2f &position, std::shared_ptr<Camera> camera, const Vector2f &viewSize,
                  AnimationPlayer animation_player = {}, AudioPlayer audio_player = {},
                  bool is_static = false);

    ~PhysicsEntity() = default;

    void setupPlayerPhysics(float jump_dt, float jump_height);

    void update(double t, float dt) override;

    void setPosition(const Vector2f &position) override;

    bool isIsStatic() const;

    void setIsStatic(bool isStatic);

    float getMass() const;

    void setMass(float mass);

    const Vector2f &getForce() const;

    void setForce(const Vector2f &force);

    void addForce(const Vector2f &force);

    const Vector2f &getVelocity() const;

    void setVelocity(const Vector2f &velocity);

    void addVelocity(const Vector2f &velocity);

    const Vector2f &getAcceleration() const;

    void setAcceleration(const Vector2f &acceleration);

    const Vector2f &getGravitationalAcceleration() const;

    const Vector2f &getMaxVelocity() const;

    const Vector2f &getDrag() const;

    const Vector2f &getFriction() const;

    void applyGravity();

    void applyFriction();

    void applyDrag();

    bool isPassthrough() const;

    void setPassthrough(bool passthrough);

    void applySideScrolling();

    virtual void resolveCollision(PhysicsEntity &other, bool resolve = true, bool set_collided = true);

    bool isCollided() const;

    void setCollided();

    void disappear();

    unsigned int getMaxHitPoints() const;

    void setMaxHitPoints(unsigned int maxHitPoints);

    unsigned int getCurrentHitPoints() const;

    void setCurrentHitPoints(unsigned int currentHitPoints);

    void addHitPoints(unsigned int hit_points);

    void subtractHitPoints(unsigned int hit_points);

    bool canShoot() const;

    void setCanShoot(bool can_shoot);

    void setHPBar(const std::shared_ptr<UIEntity> &hp_bar);
};


#endif //GAMEENGINE_PHYSICSENTITY_H
