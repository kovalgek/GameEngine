#define _USE_MATH_DEFINES
#include <math.h>

#include "ViewModel.h"

class CameraViewModel : public ViewModel
{
public:
	CameraViewModel() = default;
	~CameraViewModel() = default;

	void update();

	float theta = M_PI;
	float phi = M_PI_4;
	float radius = 70;
};