#pragma once

#include <vector>
#include <memory>
#include "Components.h"

class GameObject {
public:
	std::vector<Component*> allComponents();
//private:
	std::vector<std::unique_ptr<Component>> components;
};