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

void Background::draw(Graphics::Image& image/*, const glm::vec2& offset*/)
{
	if (!background)
		return;
	// Cover the entire image with the tiled background image.
	const float scaleWidth = static_cast<float>(image.getWidth()) / static_cast<float>(background->getWidth()) / scale;
	const float scaledHeight = static_cast<float>(image.getHeight()) / static_cast<float>(background->getHeight()) / scale;

	// Quad vertices.
	const Vertex v[] = {
		Vertex { { 0, 0 }, { textureOffset.x, textureOffset.y } },                                                         // Top-left
		Vertex { { image.getWidth(), 0 }, { scaleWidth + textureOffset.x, textureOffset.y } },                               // Top-right
		Vertex { { image.getWidth(), image.getHeight() }, { scaleWidth + textureOffset.x, scaledHeight + textureOffset.y } },  // Bottom-right
		Vertex { { 0, image.getHeight() }, { textureOffset.x, scaledHeight + textureOffset.y } }                             // Bottom-left
	};

	image.drawQuad(v[0], v[1], v[2], v[3], *background, AddressMode::Wrap);
}
