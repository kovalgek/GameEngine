#include <vector>
#include <memory>
#include "GeometryStorage.h"

struct RenderItem;
//class GeometryStorage;

#pragma once
class ObjectsDataProvider
{
public:
	ObjectsDataProvider(std::unique_ptr <GeometryStorage> geometryStorage);
	~ObjectsDataProvider();
	std::vector<std::shared_ptr<RenderItem>> renderItems();

private:
	std::unique_ptr <GeometryStorage> geometryStorage;
};

