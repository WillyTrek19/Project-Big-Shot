#include "ModuleParticles.h"
#include "Application.h"

#include "ModuleInput.h"
#include "ModuleGameObjects.h"
#include "ModuleResources.h"

ModuleParticles::ModuleParticles(Application* app, bool startEnabled): Module(app, startEnabled)
{}

ModuleParticles::~ModuleParticles()
{}

bool ModuleParticles::Init()
{
    LOG_CONSOLE("Initializing Particles handler");
    plane = (ResourceMesh*)App->resources->GetShape(Shape::PLANE);
    return true;
}

bool ModuleParticles::Start()
{
    return true;
}

UpdateStatus ModuleParticles::Update(float dt)
{
    if (App->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN)
    {
        if (firework)
        {
            LOG_CONSOLE("Created firework!");
        }
    }

    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
    {
        (*it)->GetComponent<Emitter>()->Update(dt, App);
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleParticles::PostUpdate()
{
    SortParticles();

    for (uint i = 0; i < MAX_PARTICLES; ++i)
    {
        if (particles[i].active)
        {
            particles[i].Draw();
        }
    }
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
    for (uint i = 0; i < MAX_PARTICLES; ++i)
    {
        particles[i].active = false;
        particles[i].owner = nullptr;
    }

    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
    {
        DeleteEmitter((*it));
        if (emitters.empty()) break;
    }
    emitters.clear();

    plane = nullptr;
    if (firework != nullptr)
    {
        firework->RemoveComponent(firework->GetComponent<Emitter>());
        firework->CleanUp();
        delete firework;
        firework = nullptr;
    }
    return true;
}

GameObject* ModuleParticles::CreateEmitter(EmitterData data)
{
    GameObject* go = new GameObject("Emitter");
    Emitter* e = (Emitter*)go->CreateComponent(ComponentTypes::EMITTER, data);
    for (uint i = 0; i < MAX_PARTICLES; ++i)
    {
        if (particles[i].active && particles[i].owner != nullptr)
        {
            e->allParticles[i] = particles[i];
        }
    }
    //e->allParticles = particles;
    emitters.push_back(go);
    return go;
}

void ModuleParticles::DeleteEmitter(GameObject* e)
{
    Emitter* emitter = e->GetComponent<Emitter>();
    emitter->ClearEmitter();
    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
    {
        if ((*it) == e)
        {
            emitters.erase(it);
            break;
        }
    }
}

void ModuleParticles::SortParticles()
{
    uint swaps = 0;
    for (uint i = 0; i < MAX_PARTICLES; ++i)
    {
        if (i + 1 <= MAX_PARTICLES && particles[i].camDistance > particles[i + 1].camDistance)
        {
            Swap<Particle>(particles[i], particles[i + 1]);
            swaps++;
        }
    }
    if (swaps > 0) SortParticles();
}