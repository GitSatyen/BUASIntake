#include <LDtkLoader/Project.hpp>
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
#include <glm/vec2.hpp>
#include <Math/Camera2D.hpp>
#include "Game.hpp"

#include "Player.hpp"

using namespace Graphics;
using namespace Math;

Window window;
Image image;
Sprite sprite;
Sprite background;
TileMap grassTiles;
Camera2D camera;
ldtk::Project project;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
//Math::Transform2D PlayerTransform;
float Player_x = SCREEN_WIDTH / 2;
float Player_y = SCREEN_HEIGHT / 2 ;
//float player_speed = 60.0f;

Player player;
Level level;

size_t CurrLevelId = 0u;
//Which level plays next
size_t nextLevelId = 0u;

void InitGame()
{
	//glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	player.setPosition({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });
	//camera.setOrigin(player.getPosition());
}

void loadLevel(size_t levelId, size_t characterId)
{
	auto& world = project.getWorld();
	auto& levels = world.allLevels();

	CurrLevelId = levelId % levels.size();

	level = Level{ project, world, levels[CurrLevelId] };

	level.setCharacter(characterId);
}

int main()
{
	Input::mapButton( "Reload", [](std::span<const GamePadStateTracker> gamePadStates, const KeyboardStateTracker& keyboardState, const MouseStateTracker& mouseState) {
		bool b = false;

		for (auto& GamePadState : gamePadStates)
		{
			b = b || GamePadState.b == ButtonState::Pressed;
		}

		const bool r = keyboardState.isKeyPressed(KeyCode::R);
		const bool enter = keyboardState.isKeyPressed(KeyCode::Enter);

		return b || enter || r;
	});

	

	//image.resize(SCREEN_WIDTH, SCREEN_HEIGHT);
	Game game{ SCREEN_WIDTH, SCREEN_HEIGHT };

	window.create(L"Pixel World", SCREEN_WIDTH, SCREEN_HEIGHT);
	window.show();
	window.setFullscreen(true);
	//PlayerTransform.setAnchor({32, 16});
	//window.toggleVSync();

	//auto idle_sprites = ResourceManager::loadSpriteSheet("assets/Spirit Boxer/Idle.png", 137, 44, 0,0, BlendMode::AlphaBlend);	
	player = Player({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });
	camera.setSize({ SCREEN_WIDTH, SCREEN_HEIGHT });
	camera.setPosition(player.getPosition());

	// Load Tilemap 
	//auto backgroundmap = ResourceManager::loadImage("assets/Map.png");
	//background = Sprite{ backgroundmap };

	//auto grass_sprites = ResourceManager::loadSpriteSheet("assets/PixelArt/Texture/TX Tileset Grass.png", /*16, 16 */137, 44, 0, 0, BlendMode::AlphaBlend);
	//grassTiles = TileMap(grass_sprites, 30, 30);

	//for(int i = 0; i < 30; ++i)
	//{
	//	for(int j = 0; j < 30; ++j)
	//	{
	//		grassTiles(i, j) = (i * grass_sprites->getNumColumns() + j) % grass_sprites->getNumSprites();
	//	}
	//}

	//ldtk loader level
	//try {
	//	project.loadFromFile("assets/Background/lvl1.ldtk");
	//}
	//catch (std::exception& ex) {
	//	std::cerr << ex.what() << std::endl;
	//	return 0;
	//}
	//loadLevel(0, 0);

	Timer       timer;
	double      totalTime = 0.0;
	uint64_t    frameCount = 0ull;
	std::string fps = "FPS: 0";

	InitGame();

	while(window)
	{	
		// Render loop
		image.clear(Color::White);
		//Update game
		game.Update();

		window.present(game.getImage());
		//timer.tick();
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
		//camera.setPosition(player.getPosition());

		if(Input::getButton("Reload"))
		{
			InitGame();
		}

		//image.drawSprite(background, camera);
		//level.draw(image, camera);
		//player.draw(image, camera);
		image.drawText(Font::Default, fps, 10, 10, Color::Black);

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
		
		
		/*++frameCount;

		totalTime += timer.elapsedSeconds();
		if (totalTime > 1.0)
		{
			fps = fmt::format("FPS: {:.3f}", static_cast<double>(frameCount) / totalTime);

			std::cout << fps << std::endl;

			frameCount = 0;
			totalTime = 0.0;
		}*/
	}

	std::cout << "Thanks for playing!" << std::endl;
	return 0;
};