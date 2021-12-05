#ifndef __MODULE_GAME_OBJECTS_H__
#define __MODULE_GAME_OBJECTS_H__

#include "Module.h"
#include "GameObject.h"
#include "ImGuizmo.h"
#include <vector>

class ModuleGameObjects : public Module
{
public:
	ModuleGameObjects(Application* app, bool startEnabled = true);
	~ModuleGameObjects();

	bool Init();
	bool Start();

	UpdateStatus PreUpdate();
	UpdateStatus Update(float dt);
	UpdateStatus UpdateChildren(GameObject* parent);
	UpdateStatus PostUpdate();

	void GuizmoTransformation();

	bool CleanUp();

	void AddGameobject(GameObject* g);
	void RemoveGameobject(GameObject* g);

	std::vector<GameObject*> GetAllGameObjects();

	void PreorderGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects);

public:
	std::vector<GameObject*> gameObjectList;
	GameObject* selectedGameObject = nullptr;
	GameObject* mainCamera = nullptr;

private:
	void RenderChildren(GameObject* parent);
	ImGuizmo::OPERATION currentGizmoOperation;
	ImGuizmo::MODE currentGizmoMode;
};

#endif // !__MODULE_GUI_MANAGER_H__