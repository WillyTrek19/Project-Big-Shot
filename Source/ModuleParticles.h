#ifndef __MODULE_PARTICLES_H__
#define __MODULE_PARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include <vector>
#include <list>
#include "Particle.h"

#define MAX_PARTICLES 5000

class GameObject;
class ResourceMesh;
class EmitterData;

class ModuleParticles : public Module
{
public:
	ModuleParticles(Application* app, bool startEnabled = true);
	~ModuleParticles();

	bool Init();
	bool Start();
	UpdateStatus Update(float dt);
	UpdateStatus PostUpdate();
	bool CleanUp();

	GameObject* CreateEmitter(EmitterData dat);
	std::vector<GameObject*>::iterator DeleteEmitter(GameObject* e);

	EmitterData CreateFireworkData();
	EmitterData CreateSmokeData();

	/*void Load(std::string scene);
	void Save(std::string scene);*/
private:
	void SortParticles(std::vector<Particle*> &particlePool);

public:
	std::vector<GameObject*> emitters;
	ResourceMesh* plane = nullptr;

	GameObject* firework = nullptr;
};

#endif // !__MODULE_PARTICLES_H__