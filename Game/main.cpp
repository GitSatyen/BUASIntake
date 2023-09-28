#include <Graphics/Window.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/Sprite.hpp>
#include <Graphics/Timer.hpp>
#include <iostream>
#include <string_view>
#include <fmt/core.h>
#include <Graphics/Font.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Graphics/SpriteAnim.hpp>
#include <Graphics/Keyboard.hpp>
#include <Graphics/Input.hpp>
#include <Math/Transform2D.hpp>
#include <Graphics/TileMap.hpp>

#include "Player.hpp"

using namespace Graphics;

Window window;
Image image;
Sprite sprite;
TileMap grassTiles;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
//Math::Transform2D PlayerTransform;
float Player_x = SCREEN_WIDTH / 2;
float Player_y = SCREEN_HEIGHT / 2 ;
//float player_speed = 60.0f;

Player player;

void InitGame()
{
	//glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	player.setPosition({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });
	//int Player_x = SCREEN_WIDTH / 2;
	//int Player_y = SCREEN_HEIGHT / 2;
}

int main()
{
	Input::mapButton( "Reload", [](std::span<const GamePadStateTracker> gamePadStates, const KeyboardStateTracker& keyboardState, const MouseStateTracker& MouseState) {
		bool b = false;

		for (auto& GamePadState : gamePadStates)
		{
			b = b || GamePadState.b == ButtonState::Pressed;
		}

		const bool r = keyboardState.isKeyPressed(KeyCode::R);
		const bool enter = keyboardState.isKeyPressed(KeyCode::Enter);

		return b || enter || r;
	});

	image.resize(SCREEN_WIDTH, SCREEN_HEIGHT);

	window.create(L"Pixel World", SCREEN_WIDTH, SCREEN_HEIGHT);
	window.show();
	window.setFullscreen(true);
	//PlayerTransform.setAnchor({32, 16});
	//window.toggleVSync();

	auto idle_sprites = ResourceManager::loadSpriteSheet("assets/Spirit Boxer/Idle.png", 137, 44, 0,0, BlendMode::AlphaBlend);	
	player = Player({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, SpriteAnim{ idle_sprites, 6 });

	// Load Tilemap 
	auto grass_sprites = ResourceManager::loadSpriteSheet("assets/PixelArt/Texture/TX Tileset Grass.png", 16, 16 /*137, 44, 0, 0, BlendMode::AlphaBlend*/);
	grassTiles = TileMap(grass_sprites, 30, 30);

	for(int i = 0; i < 30; ++i)
	{
		for(int j = 0; j < 30; ++j)
		{
			grassTiles(i, j) = (i * grass_sprites->getNumColumns() + j) % grass_sprites->getNumSprites();
		}
	}

	Timer       timer;
	double      totalTime = 0.0;
	uint64_t    frameCount = 0ull;
	std::string fps = "FPS: 0";

	InitGame();

	while(window)
	{
		//Update loop
		Input::update();
		//Animates sprite 
		player.update(timer.elapsedSeconds());
		//Check collisions with player
		//Screen space collsion
		{
			auto aabb = player.getAABB();
			glm::vec2 correction{ 0 };
			if(aabb.min.x < 0)
			{
				correction.x = -aabb.min.x;
			}
			if(aabb.min.y < 0)
			{
				correction.y = -aabb.min.y;
			}
			if(aabb.max.x >= image.getWidth())
			{
				correction.x = image.getWidth() - aabb.max.x;
			}
			if(aabb.max.y >= image.getHeight())
			{
				correction.y = image.getHeight() - aabb.max.y;
			}
			player.translate(correction);
		}


		if(Input::getButton("Reload"))
		{
			InitGame();
		}


		// Render loop
		image.clear(Color::White);

		grassTiles.draw(image);

		player.draw(image);

		image.drawText(Font::Default, fps, 10, 10, Color::Black);

		window.present(image);

		Event e;
		while (window.popEvent(e))
		{
			switch (e.type)
			{
				case Graphics::Event::Close:
					window.destroy();
					break;
				case Graphics::Event::KeyPressed:
				{
					switch (e.key.code)
					{
					case Graphics::KeyCode::Escape:
						window.destroy();
						break;
					case KeyCode::V:
						window.toggleVSync();
						break;
					}
				}
				break;
			}
		}
		
		timer.tick();
		++frameCount;

		totalTime += timer.elapsedSeconds();
		if (totalTime > 1.0)
		{
			fps = fmt::format("FPS: {:.3f}", static_cast<double>(frameCount) / totalTime);

			std::cout << fps << std::endl;

			frameCount = 0;
			totalTime = 0.0;
		}
	}

	std::cout << "Thanks for playing!" << std::endl;
	return 0;
};