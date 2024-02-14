#pragma once

#include <Graphics/Image.hpp>

class StartEnd
{
public:
	StartEnd(uint32_t screenWidth, uint32_t screenHeight);
	~StartEnd() = default;

	const Graphics::Image& getImage() const noexcept
	{
		return image;
	}

protected:
	Graphics::Image image;
};

