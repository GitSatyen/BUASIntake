#include "Game.hpp"
#include "Background.hpp"

#include <Graphics/Color.hpp>
#include <Graphics/Input.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/ResourceManager.hpp>

#include <glm/vec2.hpp>

#include <fmt/core.h>
#include <string>

using namespace Graphics;

Game::Game(uint32_t screenWidth, uint32_t screenHeight)
    : image{ screenWidth, screenHeight }
    ,transform{}
{
    //ldtk loader level
    try {
        project.loadFromFile("assets/Background/lvl1.ldtk");
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        //return 0;
    }
    loadLevel(0, 0);
}
void Game::update()
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

        std::cout << fps << std::endl;
    }

    //Update Input
    auto elapsedTime = static_cast<float>(timer.elapsedSeconds());
    Input::update();
    //Update level 
    level.update(timer.elapsedSeconds());
    //Clear level image
    image.clear(Color::Black);
    // Draw level
    level.draw(image, camera);
    // Update and draw background
    background.update(timer);
    background.draw(image);

    image.drawText(Font::Default, fps, 10, 10, Color::Magenta);
#if _DEBUG
    image.drawText(Font::Default, fps, 10, 10, Color::Magenta);
#endif
}

void Game::loadLevel(size_t levelId, size_t characterId)
{
    //Parse LDtk project and levels
    auto& world = project.getWorld();
    auto& levels = world.allLevels();

    LevelId = levelId % levels.size();

    level = Level{ project, world, levels[LevelId] };
}
