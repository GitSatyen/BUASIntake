#pragma once

#include "Background.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <Graphics/Events.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/Timer.hpp>
#include <Graphics/Sprite.hpp>

#include <Math/Camera2D.hpp>
#include <Math/Rect.hpp>

#include <LDtkLoader/Project.hpp>

#include <cstdint>

using namespace Math;

class Game
{
public:
    Game(uint32_t screenWidth, uint32_t screenHeight);
    ~Game() = default;

    Game(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&) = delete;

    void Update();

    const Graphics::Image& getImage() const noexcept
    {
        return image;
    }

    void loadLevel(size_t levelId, size_t characterId);

protected:
    ldtk::Project project;
    Background background;
    Camera2D camera;
   // Sprite backgroundImage;

    const glm::mat2 transform;
   /* glm::vec2 Player_pos{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }; 
    float Player_x = SCREEN_WIDTH / 2;
    float Player_y = SCREEN_HEIGHT / 2;*/

    Graphics::Image image;
    Graphics::Timer timer;

    Player player;
    Level level;
    size_t LevelId = 0u;
    // Which level plays next
    size_t nextLevelId = 0u;

    // Player character
    size_t currentCharacterId = 0u;
};

