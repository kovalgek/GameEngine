#pragma once

struct FrameResource;
class GameTimer;

class ConstantBufferUpdating {
public:
	virtual void update(FrameResource& frameResource, const GameTimer& gameTimer) = 0;
};