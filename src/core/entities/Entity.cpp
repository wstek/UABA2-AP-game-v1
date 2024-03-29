#include "Entity.h"

#include <utility>

Entity::Entity(const Vector2f &position, std::shared_ptr<Camera> camera, const Vector2f &view_size,
               AnimationPlayer animation_player, AudioPlayer audio_player)
        : _position(position), _scale({1, 1}), _rotation(0), _camera(std::move(camera)), _view_size(view_size),
          _hitbox(nullptr), _animation_player(std::move(animation_player)),
          _audio_player(std::move(audio_player)) {

}

void Entity::update(double t, float dt) {
    _animation_player.advanceAnimation();
    updateAnimationFrame();

    std::shared_ptr<Vector2f> audio_listener_position = _audio_player.getAudioListenerPosition();
    if (audio_listener_position != nullptr) {
        if ((*audio_listener_position - _position).length() <= _audio_player.getMaxDistance()) {
            setAudioVolume(static_cast<unsigned int>((1 - (*audio_listener_position - _position).length() /
                                                          _audio_player.getMaxDistance()) * 100));
        } else {
            setAudioVolume(0);
        }
    }

    updateView();
}

void Entity::updateView() {
    notifyObservers();
}

Vector2f Entity::getPosition() const {
    return _position;
}

Vector2f Entity::getScreenPosition() const {
    return _camera->projectCoordCoreToGame(_position);
}

void Entity::setPosition(const Vector2f &position) {
    if (_hitbox) {
        _hitbox->setPosition(position);
    }

    for (const auto &ray: _rays) {
        ray->move(position - _position);
    }

    _position = position;
}

void Entity::move(const Vector2f &vector) {
    setPosition(_position + vector);
}

Vector2f Entity::getScale() const {
    return _scale;
}

void Entity::setScale(const Vector2f &scale) {
    if (!(_scale.x == 0 || _scale.y == 0)) {
        Vector2f scale_mult(scale.x / _scale.x, scale.y / _scale.y);

        if (_hitbox) {
            _hitbox->scale(scale_mult);
        }

        for (const auto &ray: _rays) {
            ray->scale(scale_mult, _position);
        }
    }
    _scale = scale;
}

void Entity::scale(const Vector2f &scale) {
    setScale({_scale.x * scale.x, _scale.y * scale.y});
}

float Entity::getRotation() const {
    return _rotation;
}

void Entity::setRotation(float rotation) {
    _rotation = rotation;
}

void Entity::rotate(float rotation) {
    setRotation(_rotation + rotation);
}

Vector2f Entity::getViewSize() const {
    return _view_size;
}

Vector2f Entity::getScreenViewSize() const {
    return _camera->projectSizeCoreToGame(_view_size);
}

void Entity::setViewSize(const Vector2f &view_size) {
    _view_size = view_size;
}

const std::shared_ptr<Hitbox> &Entity::getHitbox() {
    return _hitbox;
}

std::shared_ptr<Hitbox> Entity::getScreenHitbox() {
    if (!_hitbox) return nullptr;
    return std::make_shared<Hitbox>(Hitbox(_camera->projectCoordCoreToGame(_hitbox->getPosition()),
                                           _camera->projectSizeCoreToGame(_hitbox->getSize())));
}

void Entity::setHitbox(const Vector2f &position, const Vector2f &size) {
    _hitbox = std::make_shared<Hitbox>(position, size);
}

void Entity::setHitbox(const Hitbox &hitbox) {
    _hitbox = std::make_shared<Hitbox>(hitbox);
}

void Entity::setHitbox(const std::shared_ptr<Hitbox> &hitbox) {
    _hitbox = hitbox;
}

const std::vector<std::shared_ptr<Ray>> &Entity::getRays() const {
    return _rays;
}

std::vector<std::shared_ptr<Ray>> Entity::getScreenRays() const {
    std::vector<std::shared_ptr<Ray>> new_rays;

    for (const auto &ray: _rays) {
        new_rays.push_back(std::make_shared<Ray>(Ray(_camera->projectCoordCoreToGame(ray->getOriginPoint()),
                                                     _camera->projectCoordCoreToGame(ray->getEndPoint()))));
    }

    return new_rays;
}

void Entity::setRays(const std::vector<std::shared_ptr<Ray>> &rays) {
    _rays = rays;
}

std::string Entity::getTextureGroupName() const {
    return _animation_player.getName();
}

bool Entity::isHorizontalMirror() const {
    return _animation_player.isHorizontalMirror();
}

unsigned int Entity::getCurrentTextureIndex() const {
    return _animation_player.getCurrentTextureIndex();
}

void Entity::updateAnimationFrame() {
    notifyObservers(_animation_player.getCurrentTextureIndex(), isHorizontalMirror());
}

void Entity::playAnimation(const std::string &animation_name) {
    if (_animation_player.getCurrentAnimationName() != animation_name) {
        _animation_player.startAnimation(animation_name);
        updateAnimationFrame();
    }
}

void Entity::playSound(const std::string &sound_name, bool finish, bool loop) {
    notifyObservers(finish, 5);
    notifyObservers(loop, 6);
    notifyObservers(_audio_player.getSoundId(sound_name), 3);
}

void Entity::playMusic(const std::string &music_name, bool finish, bool loop) {
    notifyObservers(finish, 5);
    notifyObservers(loop, 6);
    notifyObservers(_audio_player.getMusicId(music_name), 4);
}

void Entity::stopSound() {
    notifyObservers(0, 7);
}

void Entity::stopMusic() {
    notifyObservers(1, 7);
}

void Entity::replayAudio() {
    setAudioVolume(_audio_player.getCurrentVolume());

    if (!_audio_player.getCurrentSound().empty()) {
        playSound(_audio_player.getCurrentSound());
    }
    if (!_audio_player.getCurrentMusic().empty()) {
        playMusic(_audio_player.getCurrentMusic());
    }
}

void Entity::setAudioVolume(unsigned int volume) {
    if (volume > 100) {
        volume = 100;
    }

    _audio_player.setCurrentVolume(volume);

    notifyObservers(volume, 8);
}
