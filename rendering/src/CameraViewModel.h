#define _USE_MATH_DEFINES
#include <math.h>

#include "ViewModel.h"

class Scene;

class CameraViewModel : public ViewModel
{
public:
	CameraViewModel(Scene& scene);
	~CameraViewModel() = default;

	void update();

	float theta = M_PI;
	float phi = M_PI_4;
	float radius = 70;
private:
	Scene& scene;
};