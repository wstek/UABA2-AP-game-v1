#ifndef GAMEENGINE_IAUDIOPLAYERCREATOR_H
#define GAMEENGINE_IAUDIOPLAYERCREATOR_H


#include <string>
#include "../entities/Entity.h"
#include "../audio/AudioResource.h"

class IEntityAudioCreator {
private:

public:
    virtual unsigned int loadSound(const std::string &filename) = 0;

    virtual unsigned int loadMusic(const std::string &filename) = 0;

    virtual void createEntityAudio(std::shared_ptr<Entity> entity) = 0;
};


#endif //GAMEENGINE_IAUDIOPLAYERCREATOR_H
