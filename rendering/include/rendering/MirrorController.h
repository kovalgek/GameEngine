#pragma once

class Scene;

class MirrorController {
public:
	MirrorController(Scene& scene);
	~MirrorController() = default;

	void createReflections();
	void updateReflections();

private:
	Scene& scene;
};