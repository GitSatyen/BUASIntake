#include "StartEnd.hpp"
#include "Graphics/Image.hpp"
#include <Graphics/ResourceManager.hpp>

using namespace Graphics;

Image startScreen;

StartEnd::StartEnd(uint32_t screenWidth, uint32_t screenHeight)
	:image{screenWidth, screenHeight}
{
	 auto startScreen = ResourceManager::loadImage("assets/Texture/Startscreen.png");
}
