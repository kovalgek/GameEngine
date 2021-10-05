#pragma once

#include "ConstantBufferUpdating.h"

class MainPassDataProvider;
template<typename T> class UploadBuffer;
struct PassConstants;
class GameTimer;
struct MainPassData;

class MainPassConstantBufferUpdater : public ConstantBufferUpdating
{
public:
	MainPassConstantBufferUpdater(MainPassDataProvider& mainPassDataProvider);
	~MainPassConstantBufferUpdater() = default;

	void update(FrameResource& frameResource, const GameTimer& gameTimer);

private:
	MainPassDataProvider& mainPassDataProvider;

	void updateMainPassConstantBuffer(UploadBuffer<PassConstants>* mainPassConstantBuffer, MainPassData mainPassData, const GameTimer& gameTimer);
	PassConstants passConstantsFromMainPassData(MainPassData mainPassData, const GameTimer& gameTimer);
	PassConstants updateReflectedPassCB(PassConstants mainPassCB, const GameTimer& gameTimer);
};