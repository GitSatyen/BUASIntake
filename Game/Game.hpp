#pragma once

#include "Background.hpp"
#include "Level.hpp"

#include <Graphics/Events.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/Timer.hpp>

#include <Math/Rect.hpp>

#include <LDtkLoader/Project.hpp>

#include <cstdint>

class Game
{
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


    const glm::mat3 transform;
    Graphics::Image image;
    Graphics::Timer timer;

    Level level;
    size_t LevelId = 0u;
    // Which level plays next
    size_t nextLevelId = 0u;

    // Player character
    size_t currentCharacterId = 0u;
};

