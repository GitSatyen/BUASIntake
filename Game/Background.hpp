#pragma once

#include <Graphics/Image.hpp>
#include <glm/vec2.hpp>
#include <Graphics/Timer.hpp>

#include <filesystem>
#include <memory>

class Background {
public:
	Background() = default;

	//Scrolling background sprite 
	explicit Background(const std::filesystem::path& path, float scale = 1.0f, const glm::vec2& scrollDir = { 0.0f, 1.0f }, float scrollSpeed = 1.0f);

	void update(const Graphics::Timer& timer);

	void draw(Graphics::Image& image/*, const glm::vec2& offset*/) const;

private:
	std::shared_ptr<Graphics::Image> background;
	glm::vec2 scrollDir{ 0.0f };
	float     scrollSpeed{ 0.0f };
	float scale{ 1.0f };
	glm::vec2 textureOffset { 0.0f };

};