#include "LDtkLoader/Project.hpp"
#include "Player.hpp"
#include "Level.hpp"

#include <Graphics/BlendMode.hpp>
#include <Graphics/Color.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/Sprite.hpp>
#include <Graphics/TileMap.hpp>

#include <fmt/core.h>
#include <map>
#include <string>

using namespace Math;
using namespace Graphics;

bool Finished = false;
//ldtk::Project project;

static std::map <Level::State, std::string> statemap = {
    {Level::State::None, "State: None"},
     {Level::State::StartState, "State: Startstate"},
     {Level::State::EndState, "State: Endstate"}
};

Level::Level(const ldtk::Project& project, const ldtk::World& world, const ldtk::Level& level)
    : world{ &world }
    ,level{ &level }
    ,levelName{ level.name }

{
        const std::filesystem::path projectPath = project.getFilePath().directory();

        //Parse Layers
        //Entitie layer
        const auto& entities = level.getLayer("Entities");
        //Collision layer
        const auto& collisions = entities.getEntitiesByName("Collider");
        //Get position and size of the colliders
        for(auto& collision: collisions)
        {
            
            auto& e = collision.get();
            auto& pos = e.getPosition();
            auto size = e.getSize();

            //Wrap AABB around the colliders 
            Collider collider
            {
                .aabb = AABB { {pos.x, pos.y, 0.0 }, {pos.x + size.x - 1, pos.y + size.y - 1, 0.0} }
            };
            colliders.push_back(collider);
        }


        // Collected animation effect
        {
            //auto spriteSheet = ResourceManager::loadSpriteSheet(projectPath / "Treasure Hunters/Items/Coin Effect/1.png", 16, 16, 0, 0, BlendMode::AlphaBlend);
            //pickupCollected = SpriteAnim{ spriteSheet, 20 };
        }

        //Loads coin sprites
        const auto& tilesets = project.allTilesets();
        for (auto& tileset : tilesets)
        {
            if (tileset.name == "Coin")
            {
                auto sprites = ResourceManager::loadSpriteSheet(projectPath / tileset.path, tileset.tile_size, tileset.tile_size, static_cast<uint32_t>(tileset.padding), static_cast<uint32_t>(tileset.spacing), BlendMode::AlphaBlend);
                coinSprites[tileset.name] = std::move(sprites);
            }
        }

        // Parse the level tile map
        {
            const auto& TerrainLayer = level.getLayer("Terrain");
            const auto& intGrid = level.getLayer("Terrain");

            /*const auto& backgroundLayer = level.getLayer("Background");
            const auto& backgroundGrid = level.getLayer("Background");*/

            const auto& gridSize = TerrainLayer.getGridSize();
            const auto& tileSet = intGrid.getTileset();

            auto spriteSheet = ResourceManager::loadSpriteSheet(projectPath / tileSet.path, tileSet.tile_size, tileSet.tile_size, tileSet.padding, tileSet.spacing, BlendMode::AlphaBlend);
            tileMap = TileMap(spriteSheet, gridSize.x, gridSize.y);

            for (auto& tile : intGrid.allTiles())
            {
                const auto& gridPos = tile.getGridPosition();
                tileMap(gridPos.y, gridPos.x) = tile.tileId;
            }

            for (auto& tile : TerrainLayer.allTiles())
            {
                const auto& gridPos = tile.getGridPosition();
                tileMap(gridPos.y, gridPos.x) = tile.tileId;
            }
        }

        //Parse collectables 
        const auto& pickups = entities.getEntitiesByName("Pickup");
        for (auto& pickup : pickups)
        {
            auto& e = pickup.get();
            auto& p = e.getPosition();
            auto& s = e.getSize();
            auto& gridP = e.getGridPosition();
            

            auto& coinSprite = coinSprites["Coin"];
            Sphere collider{ { p.x, p.y, 0 }, 4.5f };

            allPickups.emplace_back(coinSprite, collider);
        }

        //Player start position
        const auto& startPos = entities.getEntitiesByName("Start")[0].get();
        playerStart = { startPos.getPosition().x, startPos.getPosition().y };
        player = Player{ playerStart };
}

void Level::update(float deltaTime)
{
    updateCollisions(deltaTime);
    updatePickups(deltaTime);
}

void Level::reset()
{
}

void Level::draw(Graphics::Image& image)
{
    tileMap.draw(image);

    for (auto& pickup : allPickups)
    {
        pickup.draw(image);
    }

   /* for (auto& effect : effects)
    {
        effect.draw(image);
    }*/ 
    player.draw(image);
    
    //Draw score on screen
    scoreCount = fmt::format("Gold: {:0} /56", score);
    image.drawText(Font::Default, scoreCount, 700, 30, Color::Yellow);
    //image.drawText(Font::Default, statemap[state], 10, 50, Color::Cyan);

#if _DEBUG
    for (const auto& collider : colliders)
    {
        image.drawAABB(collider.aabb, Color::Red, BlendMode::Disable, FillMode::WireFrame);
    }
    //image.drawText(Font::Default, statemap[state], 10, 50, Color::Cyan);
#endif
}


void Level::checkPickupCollision(const Math::Sphere& pickupCollider, const Math::AABB& colliderAABB, glm::vec2& pos, glm::vec2& vel)
{

    if(colliderAABB.intersect(pickupCollider))
    {

    }

    //Check to see if the pickup is colliding with the top edge of the collider
    Line topEdge{ { colliderAABB.min.x, colliderAABB.min.y, 0 }, { colliderAABB.max.x, colliderAABB.min.y, 0 } };
    if (pickupCollider.intersect(topEdge))
    {
        // Set the position of the pickup to the top edge of the collider
        pos.y = colliderAABB.min.y - pickupCollider.radius;
        // And negate the velocity
        vel.y = -vel.y;

        return;
    }

    //Check to see if the pickup is colliding with the bottom edge of the collider
    Line bottomEdge{ { colliderAABB.min.x, colliderAABB.max.y, 0 }, { colliderAABB.max.x, colliderAABB.max.y, 0 } };
    if (pickupCollider.intersect(bottomEdge))
    {
        //Set the position of the pickup to the bottom edge of the collider
        pos.y = colliderAABB.max.y + pickupCollider.radius;
        //And negate the velocity
        vel.y = -vel.y;

        return;
    }

    //Check to see if the pickup is colliding with the left edge of the collider
    Line leftEdge{ { colliderAABB.min.x, colliderAABB.min.y, 0 }, { colliderAABB.min.x, colliderAABB.max.y, 0 } };
    if (pickupCollider.intersect(leftEdge))
    {
        //Set the position of the pickup to the left edge of the collider
        pos.x = colliderAABB.min.x - pickupCollider.radius;
        // And negate the x velocity
        vel.x = -vel.x;

        return;
    }

    //Check to see if the pickup is colliding with the right edge of the collider
    Line rightEdge{ { colliderAABB.max.x, colliderAABB.min.y, 0 }, { colliderAABB.max.x, colliderAABB.max.y, 0 } };
    if (pickupCollider.intersect(rightEdge))
    {
        //Set the position of the pickup to the right edge of the collider
        pos.x = colliderAABB.max.x + pickupCollider.radius;
        //And negate the x velocity
        vel.x = -vel.x;

        return;
    }
}

void Level::updateCollisions(float deltaTime)
{
    //Store the previous position
    glm::vec2 prevPos = player.getPosition();

    //Update player
    player.update(deltaTime);

    //Check player collision
    //Get player AABB
    AABB playerAABB = player.getAABB();

    glm::vec2 vel = player.getVelocity();
    glm::vec2 pos = player.getPosition();

    // Number of pixels padding to account for collisions
    const float padding = 3.0f;

   // bool onGround = false;
   
    for (auto& collider : colliders)
    {
        //Collider AABB
        AABB colliderAABB = collider.aabb;

        // Player is falling
        if (vel.y < 0.0f)
        {
            //Check if player collides with top edghe of collider 
            Line topEdge{ { colliderAABB.min.x + padding, colliderAABB.min.y, 0 }, { colliderAABB.max.x -padding, colliderAABB.min.y, 0 } };
            if (playerAABB.intersect(topEdge))
            {
                // Set the player's position to the top of the AABB
                pos.y = colliderAABB.min.y;
                // And 0 the Y velocity
                vel.y = 0.0f;

                // Change to running state
                player.setState(Player::State::Running);
                onGround = true;
            }
        }
        // Player is jumping
        else if (vel.y > 0.0f)
        {
            // Check to see if the player is colliding with the bottom edge of the colliders
            Line bottomEdge{ { colliderAABB.min.x + padding, colliderAABB.max.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.max.y, 0 } };
            if (playerAABB.intersect(bottomEdge))
            {
                // Set the player's position to the bottom of the collider
                pos.y = colliderAABB.max.y + playerAABB.height();
                // And 0 the Y velocity
                vel.y = 0.0f;

                // And start falling
                player.setState(Player::State::Falling);
                continue;
            }
        }
        //Player is idle or running
        else
        {
            //Check to see if the player is colliding with the top edge of the collider
            Line topEdge{ { colliderAABB.min.x + padding, colliderAABB.min.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.min.y, 0 } };
            if (playerAABB.intersect(topEdge))
            {
                    onGround = true;   
                    player.setState(Player::State::Idle);
            }
            else if (onGround = false) 
            {

            }
        }

        if(vel.x > 0)
        {
            //Check to see if the player is colliding with the left edge of the collider
            Line leftEdge{ { colliderAABB.min.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.min.x, colliderAABB.max.y - padding, 0 } };
            if (playerAABB.intersect(leftEdge))
            {
                //Set the player's position to the left edge of the collider
                pos.x = colliderAABB.min.x - playerAABB.width() * 0.5f;
                //And 0 the X velocity
                vel.x = 0.0f;

                continue;
            }
        }

        else if(vel.x < 0)
        {
            //Check to see if the player is colliding with the right edge of the collider
            Line rightEdge{ { colliderAABB.max.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.max.x, colliderAABB.max.y - padding, 0 } };
            if (playerAABB.intersect(rightEdge))
            {
                //Set the player's position to the right edge of the collider
                pos.x = colliderAABB.max.x + playerAABB.width() * 0.5f;
                //and 0 the X velocity
                vel.x = 0.0f;

                continue;
            }
        }
    }

    player.setPosition(pos);
    player.setVelocity(vel);
}

void Level::updatePickups(float deltaTime)
{ 
    //Check player collision
    //Get player AABB
    AABB playerAABB = player.getAABB();

    //Check if player collides with the pickup
    for (auto pickups = allPickups.begin(); pickups != allPickups.end();)
    {
        if (pickups->collides(player))
        {
            //Removes object from screen
            pickups = allPickups.erase(pickups);
            //Ads point to the score on collision 
            score++;
            //Keep track of the score
            std::cout << "Score: " << score << std::endl;
        }
        else ++pickups;
    }

    if (score == 56)
    {
        Finished = true;
    }
   
}

