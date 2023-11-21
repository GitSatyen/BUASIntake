#include "Game.hpp"

#include <Graphics/Color.hpp>
#include <Graphics/Input.hpp>

#include <fmt/core.h>
#include <string>

Game::Game(uint32_t screenWidth, uint32_t screenHeight)
    : image{ screenWidth, screenHeight }
    ,transform{}
{
    project.loadFromFile("assets/Background/lvl1.ldtk");
    loadLevel(0, 0);
}
void Game::Update()
{
    static double      totalTime = 0.0;
    static uint64_t    frames = 0;
    static std::string fps = "FPS: 0";

    timer.tick();
    ++frames;
    totalTime += timer.elapsedSeconds();
    if (totalTime > 1.0)
    {
        fps = fmt::format("FPS: {:.3f}", static_cast<double>(frames) / totalTime);
        frames = 0;
        totalTime = 0.0;
    }
    //Update and draw background
    //background.update(timer);
    //background.draw(image, transform);

    auto elapsedTime = static_cast<float>(timer.elapsedSeconds());
 
    level.draw(image, transform);
}

void Game::loadLevel(size_t levelId, size_t characterId)
{
    auto& world = project.getWorld();
    auto& levels = world.allLevels();

    LevelId = levelId % levels.size();

    level = Level{ project, world, levels[LevelId] };

    level.setCharacter(characterId);
}
