#include "GameObject.h"

std::vector<Component*> GameObject::allComponents()
{
	std::vector<Component*> result;
	
	for (auto &component : components) {
		result.push_back(component.get());
	}

	return result;
}