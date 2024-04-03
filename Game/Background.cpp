#include "Background.hpp"
#include <Graphics/ResourceManager.hpp>

using namespace Graphics;

Background::Background(const std::filesystem::path& path, float scale, const glm::vec2& scrollDir, float scrollSpeed)
	: background{ Graphics::ResourceManager::loadImage(path) }
	, scrollDir{ scrollDir }
	, scrollSpeed{ scrollSpeed }
	, scale{ scale }
{}

void Background::update(const Graphics::Timer& timer)
{
	textureOffset -= scrollDir * scrollSpeed * static_cast<float>(timer.elapsedSeconds());
}

void Background::draw(Graphics::Image& image/*, const glm::vec2& offset*/) const
{
}
