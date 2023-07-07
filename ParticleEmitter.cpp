/*****************************************************************//**
 * \file   ParticleEmitter.cpp
 * \brief  Logic for a particle emmitter, used in conjunction with a
 *         custom-made C++ game engine.
 *
 * \par    Copyright 2022 DigiPen (USA) Corporation
 * \author Mike Doeren
 * \date   April 2023
 *********************************************************************/

#include "ParticleEmitter.h"
#include "RenderSystem.h"
#include "Stream.h"
#include "TextureManager.h"

void ParticleEmitter::Deserialize()
{
    std::string filename = "Data/Entity/" + parent->GetSceneName() + "/" + parent->GetName() + ".json";
    StreamSystem* particleStream = new StreamSystem(filename);
}

ParticleEmitter::ParticleEmitter()
{
    type = "ParticleEmitter";
}

ParticleEmitter::ParticleEmitter(emitterType emitterType, glm::vec2 dimensions, std::string textureName, float particleScale)
    : textureName(textureName)
    , emitterDimensions(dimensions)
    , particleScale(particleScale)
{
    type = "ParticleEmitter";
    switch (emitterType_)
    {
    case fog:
        DefineParFogVariables();
        break;
    case fire:
        DefineParFireVariables();
        break;
    }
}

ParticleEmitter::ParticleEmitter(const ParticleEmitter& inst)
    : totalAliveParticles(inst.totalAliveParticles)
    , particleLimit(inst.particleLimit)
    , particles(inst.particles)
    , textureName(inst.textureName)
    , prtclInitAlphaMin(inst.prtclInitAlphaMin)
    , prtclInitAlphaMax(inst.prtclInitAlphaMax)
    , prtclFadepointMin(inst.prtclFadepointMin)
    , prtclFadepointMax(inst.prtclFadepointMax)
    , prtclVelDirMin(inst.prtclVelDirMin)
    , prtclVelDirMax(inst.prtclVelDirMax)
    , prtclVelMagnMin(inst.prtclVelMagnMin)
    , prtclVelMagnMax(inst.prtclVelMagnMax)
    , prtclLifetimeMin(inst.prtclLifetimeMin)
    , prtclLifetimeMax(inst.prtclLifetimeMax)
    , spawnFrequency(inst.spawnFrequency)
    , timeSinceSpawn(inst.timeSinceSpawn)
    , particlesPerSpawn(inst.particlesPerSpawn)
    , emitterDimensions(inst.emitterDimensions)
    , particleScale(inst.particleScale)
{
    type = "ParticleEmitter";
}

void ParticleEmitter::operator=(const ParticleEmitter& inst)
{
    totalAliveParticles = inst.totalAliveParticles;
    particleLimit = inst.particleLimit;
    particles = inst.particles;
    textureName = inst.textureName;
    prtclInitAlphaMin = inst.prtclInitAlphaMin;
    prtclInitAlphaMax = inst.prtclInitAlphaMax;
    prtclFadepointMin = inst.prtclFadepointMin;
    prtclFadepointMax = inst.prtclFadepointMax;
    prtclVelDirMin = inst.prtclVelDirMin;
    prtclVelDirMax = inst.prtclVelDirMax;
    prtclVelMagnMin = inst.prtclVelMagnMin;
    prtclVelMagnMax = inst.prtclVelMagnMax;
    prtclLifetimeMin = inst.prtclLifetimeMin;
    prtclLifetimeMax = inst.prtclLifetimeMax;
    spawnFrequency = inst.spawnFrequency;
    timeSinceSpawn = inst.timeSinceSpawn;
    particlesPerSpawn = inst.particlesPerSpawn;
    emitterDimensions = inst.emitterDimensions;
    particleScale = inst.particleScale;
}

void ParticleEmitter::Clone(Entity* other)
{
    if (other)
    {
        ParticleEmitter* emitter = static_cast<ParticleEmitter*>(other->GetComponent("ParticleEmitter"));
        emitter->totalAliveParticles = totalAliveParticles;
        emitter->particleLimit = particleLimit;
        emitter->particles = particles;
        emitter->textureName = textureName;
        emitter->prtclInitAlphaMin = prtclInitAlphaMin;
        emitter->prtclInitAlphaMax = prtclInitAlphaMax;
        emitter->prtclFadepointMin = prtclFadepointMin;
        emitter->prtclFadepointMax = prtclFadepointMax;
        emitter->prtclVelDirMin = prtclVelDirMin;
        emitter->prtclVelDirMax = prtclVelDirMax;
        emitter->prtclVelMagnMin = prtclVelMagnMin;
        emitter->prtclVelMagnMax = prtclVelMagnMax;
        emitter->prtclLifetimeMin = prtclLifetimeMin;
        emitter->prtclLifetimeMax = prtclLifetimeMax;
        emitter->spawnFrequency = spawnFrequency;
        emitter->timeSinceSpawn = timeSinceSpawn;
        emitter->particlesPerSpawn = particlesPerSpawn;
        emitter->particleScale = particleScale;
    }
}

void ParticleEmitter::Init()
{
    parent = GetParent();
    Deserialize();
}

void ParticleEmitter::Update(float dt)
{
    timeSinceSpawn += dt;

    while (timeSinceSpawn > spawnFrequency)
    {
        timeSinceSpawn -= spawnFrequency;
        if (totalAliveParticles < particleLimit)
        {
            for (int i = 0; i < particlesPerSpawn; ++i)
            {
                AddParticle();
                ++totalAliveParticles;
            }
        }
    }

    for (auto& p : particles)
    {
        // Only update the particle if it's alive
        if (p.alive)
        {
            p.age += dt;

            if (p.age > p.fadeTime)
                p.alpha -= p.alpha / (p.lifetime - p.age) * dt;

            p.position += p.velocity * dt;
        }

        // Draw the particles
        if (p.alive)
            Render::Draw(p);

        if (p.age > p.lifetime)
        {
            p.alive = false;         // Particle is dead
            --totalAliveParticles;
        }
    }
}

bool ParticleEmitter::Particle::operator==(const Particle& inst)
{
    if (&inst == this)
        return true;
    else
        return false;
}

void ParticleEmitter::AddParticle()
{
    TextureData* particleData = TextureManager::GetInstance()->GetTextureData(textureName);

    // Create a particle
    const glm::vec4* tempTransform = dynamic_cast<Transform*>(GetParent()->GetComponent("Transform"))->GetTranslation();
    float angle = glm::radians(genRandomFloat(prtclVelDirMin, prtclVelDirMax));
    float velMag = genRandomFloat(prtclVelMagnMin, prtclVelMagnMax);

    // Range for (x, y) to determine where particles spawn within the emitter
    glm::vec2 pos;
    pos.x = genRandomFloat(-emitterDimensions.x / 2, emitterDimensions.x / 2);
    pos.y = genRandomFloat(-emitterDimensions.y / 2, emitterDimensions.y / 2);

    Particle inst(genRandomFloat(prtclLifetimeMin, prtclLifetimeMax),
        genRandomFloat(prtclFadepointMin, prtclFadepointMax),
        genRandomFloat(prtclInitAlphaMin, prtclInitAlphaMax),
        glm::vec3(tempTransform->x + pos.x, tempTransform->y + pos.y, 0),
        glm::vec3(glm::cos(angle) * velMag, glm::sin(angle) * velMag, 0),
        (int)(particleData->width * particleScale),
        (int)(particleData->height * particleScale),
        particleData->textureID);

    // Check if there's a dead particle that we can overwrite
    for (auto& p : particles)
    {
        if (p.alive == false)
        {
            p = inst;
            return;
        }
    }

    // Otherwise push_back into particles
    particles.push_back(inst);
}

void ParticleEmitter::DestroyParticle(const Particle& inst)
{
    // Create a new iterator - point to deletable elements...
    auto newEnd = std::remove(particles.begin(), particles.end(), inst);

    // ...then erase objects marked for deletion
    particles.erase(newEnd, particles.end());
}

void ParticleEmitter::DefineParFogVariables()
{
    particleLimit = 100;
    particlesPerSpawn = 10;
    spawnFrequency = 1.0f;
    prtclInitAlphaMin = 0.5f;
    prtclInitAlphaMax = 0.5f;
    prtclFadepointMin = 0.0f;
    prtclFadepointMax = 0.0f;
    prtclVelDirMin = 0.0f;
    prtclVelDirMax = 360.0f;
    prtclVelMagnMin = 200.0f;
    prtclVelMagnMax = 50.0f;
    prtclLifetimeMin = 0.5f;
    prtclLifetimeMax = 1.0f;
}

void ParticleEmitter::DefineParFireVariables()
{
    particleLimit = 100;
    particlesPerSpawn = 25;
    spawnFrequency = 0.05f;
    prtclInitAlphaMin = 0.5f;
    prtclInitAlphaMax = 0.5f;
    prtclFadepointMin = 0.0f;
    prtclFadepointMax = 0.0f;
    prtclVelDirMin = 0.0f;
    prtclVelDirMax = 360.0f;
    prtclVelMagnMin = 200.0f;
    prtclVelMagnMax = 50.0f;
    prtclLifetimeMin = 0.5f;
    prtclLifetimeMax = 1.0f;
}

float ParticleEmitter::GenRandomFloat(float min, float max)
{
    float factor = (float)(rand()) / float(RAND_MAX);
    return min + (max - min) * factor;
}
