#define _USE_MATH_DEFINES
#include <math.h>

class CameraViewModel {

public:
	CameraViewModel() = default;
	~CameraViewModel() = default;

	float theta = M_PI;
	float phi = M_PI_4;
	float radius = 70;
};