#include "Game.hpp"
#include "Background.hpp"
#include "States.hpp"

#include <Graphics/Color.hpp>
#include <Graphics/Input.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/ResourceManager.hpp>

#include <glm/vec2.hpp>

#include <fmt/core.h>
#include <thread>
#include <string>

extern bool gameActive;
using namespace Graphics;

static std::map <Status, std::string> statemap = {
    {Status::Start, "State: Start"},
     {Status::Active, "State: Active"},
     {Status::End, "State: End"}
};

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
   
    //Reset level
   /* if(doReset = true)
    {
        if (Input::getKey("enter")) 
        {
            loadLevel(0, 0);
            doReset = false;
        }
    }*/

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
    auto elapsedTime = std::min(static_cast<float>(timer.elapsedSeconds()), 1.0f/30.0f);
    Input::update();
    //Update level 
    if (gameActive == true) 
    {
        level.update(elapsedTime);
    }
    //Clear level image
    image.clear(Color::Black);
    // Draw level
    level.draw(image);

    image.drawText(Font::Default, fps, 700, 10, Color::Magenta);
#if _DEBUG
    //image.drawText(Font::Default, statemap[state], 700, 50, Color::Cyan);
#endif

    // Allows player to reset the level
    if (Input::getKey(KeyCode::Back))
    {
        level.reset();
    }
}

void Game::loadLevel(size_t levelId, size_t characterId)
{
    //Parse LDtk project and levels
    auto& world = project.getWorld();
    auto& levels = world.allLevels();
 
    LevelId = levelId % levels.size();

    level = Level{ project, world, levels[LevelId] };
}
