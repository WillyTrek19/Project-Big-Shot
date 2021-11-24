#include "ModuleGameObjects.h"
#include "Application.h"

#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"

ModuleGameObjects::ModuleGameObjects(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "game_objects";
}

ModuleGameObjects::~ModuleGameObjects()
{}

bool ModuleGameObjects::Init()
{
	return true;
}

bool ModuleGameObjects::Start()
{
	return true;
}

UpdateStatus ModuleGameObjects::PostUpdate()
{
	std::vector<GameObject*>::iterator item = gameObjectList.begin();
	while (item != gameObjectList.end())
	{
		if (!(*item)->children.empty()) RenderChildren((*item));

		Mesh* m = (*item)->GetComponent<Mesh>();
		if (m != nullptr && m->IsActive())
		{
			m->wire = App->renderer3D->IsWireframe();
			m->axis = App->renderer3D->IsAxis();
			m->Render();
		}
		++item;
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleGameObjects::RenderChildren(GameObject* parent)
{
	std::vector<GameObject*>::iterator item = parent->children.begin();
	while (item != parent->children.end())
	{
		if (!(*item)->children.empty()) RenderChildren((*item));

		Mesh* m = (*item)->GetComponent<Mesh>();
		if (m != nullptr && m->IsActive())
		{
			m->wire = App->renderer3D->IsWireframe();
			m->axis = App->renderer3D->IsAxis();
			m->Render();
		}
		++item;
	}
}

bool ModuleGameObjects::CleanUp()
{
	selectedGameObject = nullptr;

	LOG("Deleting Game Objects");

	std::vector<GameObject*>::reverse_iterator g = gameObjectList.rbegin();
	while (g != gameObjectList.rend())
	{
		(*g)->CleanUp();
		delete (*g);
		g++;
	}
	gameObjectList.clear();

	return true;
}

void ModuleGameObjects::AddGameobject(GameObject* g)
{
	App->scene->GetSceneRoot()->AddChild(g);
	g->parent = App->scene->GetSceneRoot();

	gameObjectList.push_back(g);
}

void ModuleGameObjects::RemoveGameobject(GameObject* g)
{
	if (App->scene->GetSceneRoot()->RemoveChild(g))
	{
		g->DeleteChildren();
	}
	else if (g->parent->RemoveChild(g))
	{
		g->DeleteChildren();
	}

	for (size_t i = 0; i < gameObjectList.size(); i++)
	{
		if (gameObjectList[i] == g)
		{
			gameObjectList.erase(gameObjectList.begin() + i);
		}
	}

	delete g;
	g = nullptr;
}