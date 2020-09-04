#include <array>

struct CD3DX12_STATIC_SAMPLER_DESC;

#pragma once
class TextureStaticSamplers
{
public:
	static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> getStaticSamplers();
};

