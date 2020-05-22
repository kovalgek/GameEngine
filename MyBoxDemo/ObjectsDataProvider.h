#include <vector>
#include <memory>

struct RenderItem;
class GeometryStorage;

#pragma once
class ObjectsDataProvider
{
public:
	ObjectsDataProvider(std::unique_ptr <GeometryStorage> geometryStorage);
	~ObjectsDataProvider();

	void onMouseDown(int x, int y);
	std::vector<std::shared_ptr<RenderItem>> renderItems();

private:
	std::unique_ptr <GeometryStorage> geometryStorage;
	std::vector<std::shared_ptr<RenderItem>> allRitems;
	float delta;
};

