#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleGuiManager.h"

#include "ModuleParticles.h"
#include "ModuleInput.h"
#include "ModuleGameObjects.h"
#include "ModuleScene.h"

PanelHierarchy::PanelHierarchy(Application* app, bool startEnabled) : Panel(app, startEnabled)
{
	name = "Hierarchy";
}

PanelHierarchy::~PanelHierarchy()
{}

UpdateStatus PanelHierarchy::Update()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	GameObject* root = App->scene->GetSceneRoot();
	DisplayChild(root);

	//TODO: Right Click to hierarchy options
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN && ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
	{
		ImGui::OpenPopup("OptionsHierarchy");
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && ImGui::IsPopupOpen("OptionsHierarchy"))
	{
		ImGui::CloseCurrentPopup();
	}

	if (App->gameObjects->selectedGameObject != nullptr && App->gameObjects->selectedGameObject != App->scene->GetSceneRoot())
	{
		if ((App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KeyState::KEY_REPEAT) && ImGui::IsWindowHovered())
		{
			if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_DOWN) MoveUp();
			else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_DOWN) MoveDown();
		}
	}

	if (App->gameObjects->selectedGameObject != nullptr && ImGui::IsWindowFocused() && App->gameObjects->selectedGameObject != App->scene->GetSceneRoot())
	{
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KeyState::KEY_DOWN) Delete();
	}

	RightClickMenu();

	ImGui::End();

	return UpdateStatus::UPDATE_CONTINUE;
}

void PanelHierarchy::DisplayChild(GameObject* g)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (g->GetChildNum() > 0)
	{
		if (g == App->scene->GetSceneRoot()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
		if (g == App->gameObjects->selectedGameObject) flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(g->name.c_str(), flags))
		{
			if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1)) App->gameObjects->selectedGameObject = g;
			if (ImGui::IsItemHovered())
			{
				if (App->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN) App->gameObjects->selectedGameObject = g;
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				ImGui::SetDragDropPayload("DragDropHierarchy", &g, sizeof(GameObject*), ImGuiCond_Once);
				ImGui::Text("%s", g->name.c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
				{
					IM_ASSERT(payload->DataSize == sizeof(GameObject*));
					GameObject* dropped = (GameObject*)*(const int*)payload->Data;
					if (dropped)
					{
						if (dropped->parent != g)
						{
							dropped->parent->RemoveChild(dropped);
							g == App->scene->GetSceneRoot() ? App->gameObjects->AddGameobject(dropped) : g->AddChild(dropped);
							App->gameObjects->selectedGameObject = dropped;
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			for (int i = 0; i < g->GetChildNum(); i++)
			{
				DisplayChild(g->GetChildAt(i));
			}

			ImGui::TreePop();
		}
	}
	else
	{
		flags |= ImGuiTreeNodeFlags_Leaf;

		if (g == App->gameObjects->selectedGameObject) flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(g->name.c_str(), flags))
		{
			if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1)) App->gameObjects->selectedGameObject = g;
			if (ImGui::IsItemHovered())
			{
				if (App->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN) App->gameObjects->selectedGameObject = g;
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				ImGui::SetDragDropPayload("DragDropHierarchy", &g, sizeof(GameObject*), ImGuiCond_Once);
				ImGui::Text("%s", g->name.c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
				{
					IM_ASSERT(payload->DataSize == sizeof(GameObject*));
					GameObject* dropped = (GameObject*)*(const int*)payload->Data;
					if (dropped)
					{
						if (dropped->parent != g)
						{
							dropped->parent->RemoveChild(dropped);
							g == App->scene->GetSceneRoot() ? App->gameObjects->AddGameobject(dropped) : g->AddChild(dropped);
							App->gameObjects->selectedGameObject = dropped;
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::TreePop();
		}
	}
}

bool PanelHierarchy::RightClickMenu()
{
	if (App->gameObjects->selectedGameObject != nullptr && App->gameObjects->selectedGameObject != App->scene->GetSceneRoot())
	{
		if (ImGui::BeginPopupContextItem("OptionsHierarchy"))
		{
			if (ImGui::MenuItem("Move Up"))
			{
				MoveUp();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Move Down"))
			{
				MoveDown();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Delete"))
			{
				Delete();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::BeginMenu("Create child"))
			{
				if (ImGui::MenuItem("Empty"))
				{
					GameObject* g = new GameObject("Empty GameObject");
					App->gameObjects->selectedGameObject->AddChild(g);
					App->gameObjects->selectedGameObject = g;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::BeginMenu("GameObject"))
				{
					if (ImGui::MenuItem("Cube"))
					{
						App->gui->CreateShape(Shape::CUBE, true);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Sphere"))
					{
						App->gui->CreateShape(Shape::SPHERE, true);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Cylinder"))
					{
						App->gui->CreateShape(Shape::CYLINDER, true);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Torus"))
					{
						App->gui->CreateShape(Shape::TORUS, true);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Plane"))
					{
						App->gui->CreateShape(Shape::PLANE, true);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Cone"))
					{
						App->gui->CreateShape(Shape::CONE, true);
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Camera"))
				{
					GameObject* c = new GameObject("Camera");
					c->CreateComponent(ComponentTypes::CAMERA);

					App->gameObjects->AddGameobject(c);
					App->gameObjects->selectedGameObject = c;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Particle Emitter"))
				{
					std::string name = "Emitter_";
					name.append(std::to_string(App->particles->emitters.size()));
					GameObject* e = App->particles->CreateEmitter(EmitterData());
					e->name = name;
					App->gameObjects->AddGameobject(e);
					App->gameObjects->selectedGameObject = e;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}
	return true;
}

void PanelHierarchy::MoveUp()
{
	std::vector<GameObject*> list = App->gameObjects->selectedGameObject->parent->children;
	for (std::vector<GameObject*>::iterator listItem = list.begin(); listItem != list.end(); ++listItem)
	{
		if ((*listItem) == App->gameObjects->selectedGameObject)
		{
			if ((*listItem) != list.front())
			{
				GameObject* tmp = (*listItem);
				listItem = list.erase(listItem);
				list.insert(--listItem, tmp);
				App->gameObjects->selectedGameObject->parent->children = list;
				break;
			}
			else
			{
				break;
			}
		}
	}
}

void PanelHierarchy::MoveDown()
{
	std::vector<GameObject*> list = App->gameObjects->selectedGameObject->parent->children;
	for(std::vector<GameObject*>::iterator listItem = list.begin(); listItem != list.end(); ++listItem)
	{
		if ((*listItem) == App->gameObjects->selectedGameObject)
		{
			if ((*listItem) != list.back())
			{
				GameObject* tmp = (*listItem);
				listItem = list.erase(listItem);
				list.insert(++listItem, tmp);
				App->gameObjects->selectedGameObject->parent->children = list;
				break;
			}
			else
			{
				break;
			}
		}
	}
}

void PanelHierarchy::Delete()
{
	if (App->gameObjects->selectedGameObject->GetComponent<Emitter>())
	{
		App->particles->DeleteEmitter(App->gameObjects->selectedGameObject);
	}
	App->gameObjects->RemoveGameobject(App->gameObjects->selectedGameObject);
	App->gameObjects->selectedGameObject = nullptr;
}