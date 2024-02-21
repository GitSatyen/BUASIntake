#include <LDtkLoader/Project.hpp>
#include <include/SFML/Graphics.hpp>
#include <include/SFML/Window.hpp>

#include <Graphics/Window.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/Sprite.hpp>
#include <Graphics/Timer.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Graphics/SpriteAnim.hpp>
#include <Graphics/Keyboard.hpp>
#include <Graphics/Input.hpp>
#include <Graphics/TileMap.hpp>
#include <Math/Transform2D.hpp>
#include <Math/Camera2D.hpp>

#include <fmt/core.h>
#include <glm/vec2.hpp>

#include <iostream>
#include <string_view>

#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Background.hpp"

using namespace Graphics;
using namespace Math;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

Window window;
Image image{ SCREEN_WIDTH , SCREEN_HEIGHT};
Sprite sprite;
TileMap grassTiles;
Camera2D camera;
ldtk::Project project;
glm::mat3 transform;

float _x = SCREEN_WIDTH / 2;
float _y = SCREEN_HEIGHT / 2 ;

glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

size_t CurrLevelId = 0u;
//Which level plays next
size_t nextLevelId = 0u;

void InitGame()
{
	//glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	//player.setPosition({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });
	//camera.setOrigin(player.getPosition());
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

	Game game{ SCREEN_WIDTH, SCREEN_HEIGHT };

	window.create(L"Gold Fever", SCREEN_WIDTH, SCREEN_HEIGHT);
	window.show();
	window.setFullscreen(true);
	//PlayerTransform.setAnchor({32, 16});
	//window.toggleVSync();

	camera.setSize({ SCREEN_WIDTH, SCREEN_HEIGHT });

	//InitGame();
	//startScreen.loadFromFile("assets/Texture/Startscreen.png");
	auto startImage = ResourceManager::loadImage("assets/Texture/Startscreen.png");
	
	//error if the image fails to load
	if (!ResourceManager::loadImage("assets/Texture/Startscreen.png"))
	{
		std::cout << "Failed loading image" << std::endl;
		return 1;
	}

	auto endImage = ResourceManager::loadImage("assets/Texture/Endscreen.png");	

	Sprite startScr(startImage);
	//startScreen(startImage);
	while(window)
	{	
		// Render loop

		// Display startscreen		
		image.copy(*startImage);
		window.present(image);

		// Display game
		//window.present(game.getImage());
		//Update game
		game.update();

		if(Input::getButton("Reload"))
		{
			InitGame();
		}

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

			//error if the image fails to load
			if (!ResourceManager::loadImage("assets/Texture/Endscreeen.png"))
			{
				std::cout << "Failed loading image" << std::endl;
				return 1;
			}
		}
	}

	std::cout << "Thanks for playing!" << std::endl;
	return 0;
};