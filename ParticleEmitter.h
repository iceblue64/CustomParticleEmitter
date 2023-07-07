/*****************************************************************//**
 * \file   ParticleEmitter.h
 * \brief  Logic for a particle emmitter, used in conjunction with a
 *         custom-made C++ game engine.
 *
 * \par    Copyright 2022 DigiPen (USA) Corporation
 * \author Mike Doeren
 * \date   April 2023
 *********************************************************************/

#pragma once
#include "Entity.h"
#include "Transform.h"
#include <glm/matrix.hpp>
#include <vector>

class ParticleEmitter : public Component
{
public:
    // Enum to determine the type of emitter
    enum emitterType
    {
        fog, fire
    };

    static emitterType StringToEmitterType(std::string et)
    {
        if (et == "fire")
            return fire;
        else
            return fog;
    }

    void Deserialize();

    ParticleEmitter();
    ParticleEmitter(emitterType emitterType, glm::vec2 dimensions, std::string textureName, float particleScale);
    ParticleEmitter(const ParticleEmitter& inst);
    void operator=(const ParticleEmitter& inst);
    void Clone(Entity* other);

    void Init() override;
    void Update(float dt) override;

    // A struct container to hold information for particles
    struct Particle
    {
        Particle(float m_lifetime, float m_fadePoint, float m_alpha, glm::vec3 m_pos, glm::vec3 m_vel, int m_width, int m_height, unsigned m_textureID)
            : lifetime(m_lifetime)
            , fadeTime(m_fadePoint)
            , alpha(m_alpha)
            , position(m_pos)
            , velocity(m_vel)
            , age(0.0f)
            , width(m_width)
            , height(m_height)
            , alive(true)
            , textureID(m_textureID) {}

        // Variables
        glm::vec3 position;
        glm::vec3 velocity;
        int width;
        int height;
        float lifetime;          // Total time for particle to live
        float age;
        float alpha;
        float fadeTime;          // The position in the particle's lifespan where it begins to fade
        bool alive;
        unsigned textureID;

        bool operator==(const Particle& inst);
    };

    // Helper functions
    void AddParticle();
    void DestroyParticle(const Particle& inst);
    void Serialize() override {}
    std::string GetEmitterTexture() { return textureName; }
    float GetParticleScale() { return particleScale; }
    glm::vec2 GetEmitterSize() { return emitterDimensions; }

private:
    // Functions
    float GenRandomFloat(float min, float range);
    void DefineParFogVariables();
    void DefineParFireVariables();

    // Variables
    std::vector<Particle> particles;
    std::string textureName;

    int totalAliveParticles;     // Track the total number of particles
    int particleLimit;           // The limit to how many particles we can spawn
    float spawnFrequency;        // How much time is between particles spawns
    float timeSinceSpawn;        // The number of seconds since a particle has spawned
    int particlesPerSpawn;       // The number of particles per spawn

    float prtclInitAlphaMin;
    float prtclInitAlphaMax;

    float prtclFadepointMin;
    float prtclFadepointMax;

    float prtclVelDirMin;        // In degrees
    float prtclVelDirMax;        // In degrees
    float prtclVelMagnMin;
    float prtclVelMagnMax;

    float prtclLifetimeMin;      // Determine when a particle fades/when it stops getting drawn
    float prtclLifetimeMax;

    float particleScale;         // Determine particle size on screen

    glm::vec2 emitterDimensions; // Defines the area that the particles will spawn in
};