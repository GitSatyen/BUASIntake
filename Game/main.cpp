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
#include <Audio/Sound.hpp>

#include <fmt/core.h>
#include <glm/vec2.hpp>

#include <iostream>
#include <string_view>

#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "States.hpp"

using namespace Graphics;
using namespace Math;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

float _x = SCREEN_WIDTH / 2;
float _y = SCREEN_HEIGHT / 2;

Window window;
Image image{ SCREEN_WIDTH , SCREEN_HEIGHT};
Sprite sprite;
Camera2D camera;
ldtk::Project project;
glm::mat3 transform;

// Sound effects
Audio::Sound bgMusic;
Audio::Sound winSound;

glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

size_t CurrLevelId = 0u;
// Which level plays next
size_t nextLevelId = 0u;

bool gameActive = false;
extern bool Finished;

Status status = Status::Start;

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

	//Input::update();
	Game game{ SCREEN_WIDTH, SCREEN_HEIGHT };
	Level level;

	window.create(L"Gold Adventure", SCREEN_WIDTH, SCREEN_HEIGHT);
	window.show();
	//window.setFullscreen(true);
	//window.toggleVSync();

	camera.setSize({ SCREEN_WIDTH, SCREEN_HEIGHT });

	auto startScreen = ResourceManager::loadImage("assets/Texture/Startscreen.png");
	
	// error if the image fails to load
	if (!ResourceManager::loadImage("assets/Texture/Startscreen.png"))
	{
		std::cout << "Failed loading image" << std::endl;
		return 1;
	}

	auto endScreen = ResourceManager::loadImage("assets/Texture/Endscreen.png");	

	// Background music
	std::string filePath = "assets\\sounds\\music.mp3";
	if (filePath; NULL != 0)
	{
		std::cerr << "Failed to open sound file" << std::endl;
		{int i = 3; };
	}

	bgMusic.loadMusic(filePath);
	bgMusic.setLooping(true);
	bgMusic.setVolume(0.1f);

	//// Win sound effect
	std::string winFile = "assets\\sounds\\win.mp3";
	if (winFile; NULL != 0)
	{
		std::cerr << "Failed to open sound file" << std::endl;
		{int i = 3; };
	}
	winSound.loadMusic(winFile);
	winSound.setLooping(false);
	winSound.setVolume(1.0f);

	while(window)
	{
		Input::update();
		switch(status)
		{
		case Status::Start:
			if (Input::getKey(KeyCode::Enter))
				status = Status::Active;
			window.present(*startScreen);
			break;
		case Status::Active:
			gameActive = true;
			window.present(game.getImage());
			bgMusic.play();	

			if (Finished == true) 
			{
				status = Status::End;				
				winSound.play();
			}
			break;
		case Status::End:
			window.present(*endScreen);
			if (Input::getKey(KeyCode::Enter))
				window.destroy();
			break;
		}

		// Render loop
		// Update game
		game.update();

		if(Input::getButton("Reload"))
		{
			InitGame();
		}

//Skip game to end screen
#if _DEBUG
		if (Input::getKey(KeyCode::G))
		{
			Finished = true;
		}
#endif
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
					case KeyCode::F11:
						window.toggleFullscreen();
						break;
					}
				}
				break;
			}

			//error if the image fails to load
			if (!ResourceManager::loadImage("assets/Texture/Endscreen.png"))
			{
				std::cout << "Failed loading image" << std::endl;
				return 1;
			}
		}
	}

	// Make sure sound doesn't get called after program gets destroyed 
	bgMusic.reset();
	winSound.reset();
	std::cout << "Thanks for playing!" << std::endl;	
	return 0;
};