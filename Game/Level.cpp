#include "LDtkLoader/Project.hpp"
#include "Player.hpp"
#include "Level.hpp"

#include <Graphics/BlendMode.hpp>
#include <Graphics/Color.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/TileMap.hpp>

#include <numbers>

using namespace Math;
using namespace Graphics;

//ldtk::Project project;

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
        for(auto& collision: collisions)
        {
            
            auto& get = collision.get();
            auto& pos = get.getPosition();
            auto size = get.getSize();

            Collider collider
            {
                .aabb = AABB { {pos.x, pos.x, 0.0 }, {pos.x + size.x - 1, pos.y + size.y - 1, 0.0} }
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
            if (tileset.hasTag("Coin"))
            {
                auto sprites = ResourceManager::loadSpriteSheet(projectPath / tileset.path, tileset.tile_size, tileset.tile_size, static_cast<uint32_t>(tileset.padding), static_cast<uint32_t>(tileset.spacing), BlendMode::AlphaBlend);
                //Sprites[tileset.name] = std::move(sprites);
            }
        }

        // Parse the level tile map.
        {
            const auto& tilesLayer = level.getLayer("Terrain");
            const auto& intGrid = level.getLayer("Terrain");

            const auto& gridSize = tilesLayer.getGridSize();
            const auto& tileSet = intGrid.getTileset();

            auto spriteSheet = ResourceManager::loadSpriteSheet(projectPath / tileSet.path, tileSet.tile_size, tileSet.tile_size, tileSet.padding, tileSet.spacing, BlendMode::AlphaBlend);
            tileMap = TileMap(spriteSheet, gridSize.x, gridSize.y);

            for (auto& tile : intGrid.allTiles())
            {
                const auto& gridPos = tile.getGridPosition();
                tileMap(gridPos.y, gridPos.x) = tile.tileId;
            }

            for (auto& tile : tilesLayer.allTiles())
            {
                const auto& gridPos = tile.getGridPosition();
                tileMap(gridPos.y, gridPos.x) = tile.tileId;
            }
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
        updateEffects(deltaTime);
        //testUpdateCollisions(deltaTime);
}

void Level::reset()
{
}

void Level::draw(Graphics::Image& image, const glm::mat3 transform) 
{
    tileMap.draw(image,  transform);

#if _DEGUB
    for (const auto& collider : colliders)
    {
        image.drawAABB(collider.aabb, Color::Yellow, BlendMode:Disable, FillMode::WireFrame);
    }
#endif

   /* for (auto& effect : effects)
    {
        effect.draw(image);
    }*/
    player.draw(image, camera);
    
}

void Level::addPickup(std::string_view name, const glm::vec2& pos)
{
}

void Level::checkPickupCollision(const Math::Sphere& pickupCollider, const Math::AABB& colliderAABB, glm::vec2& pos, glm::vec2& vel)
{
}

void Level::updateCollisions(float deltaTime)
{
    // Store the previous position
    glm::vec2 prevPos = player.getPosition();

    //Update player
    player.update(deltaTime);

    // Check player collision
    //Get player AABB
    AABB playerAABB = player.getAABB();

    glm::vec2 vel = player.getVelocity();
    glm::vec2 pos = player.getPosition();

    // Number of pixels padding to account for collisions
    const float padding = 3.0f;

    bool onGround = false;
   
    for (auto& collider : colliders)
    {
        //Collider AABB
        AABB colliderAABB = collider.aabb;

        // Player is falling
        if (vel.y < 0.0f)
        {
            //Check if player collides with top edghe of collider 
            Line topEdge{ { colliderAABB.min.x + padding, colliderAABB.min.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.min.y, 0 } };
            if (playerAABB.intersect(topEdge))
            {
                // We only collide if the player's previous position was above the collider
                if (prevPos.y < colliderAABB.min.y)
                {
                    // Set the player's position to the top of the AABB.
                    pos.y = colliderAABB.min.y;
                    // And 0 the Y velocity.
                    vel.y = 0.0f;

                        // Change to running state
                        player.setState(Player::State::Running);
                        onGround = true;

                    continue;
                }
            }
        }
        // Player is jumping
        else if (vel.y > 0.0f)
        {
            // Check to see if the player is colliding with the bottom edge of the collider.
            Line bottomEdge{ { colliderAABB.min.x + padding, colliderAABB.max.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.max.y, 0 } };
            if (playerAABB.intersect(bottomEdge))
            {
                // Set the player's position to the bottom of the collider.
                //pos.y = colliderAABB.max.y + playerAABB.height();
                // And 0 the Y velocity.
                vel.y = 0.0f;

                // And start falling
                player.setState(Player::State::Falling);
                continue;
            }
        }
        // Player is idle or running.
        else
        {
            // Check to see if the player is colliding with the top edge of the collider.
            Line topEdge{ { colliderAABB.min.x + padding, colliderAABB.min.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.min.y, 0 } };
            if (playerAABB.intersect(topEdge))
            {
                    onGround = true;   
            }
        }

        // Check to see if the player is colliding with the left edge of the collider.
        Line leftEdge{ { colliderAABB.min.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.min.x, colliderAABB.max.y - padding, 0 } };
        if (playerAABB.intersect(leftEdge))
        {
            // Set the player's position to the left edge of the collider.
            pos.x = colliderAABB.min.x - playerAABB.width() * 0.5f;
            // And 0 the X velocity.
            vel.x = 0.0f;

            // On a wall that is right of the player.
            // onRightWall = true;
            

            continue;
        }
        // Check to see if the player is colliding with the right edge of the collider.
        Line rightEdge{ { colliderAABB.max.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.max.x, colliderAABB.max.y - padding, 0 } };
        if (playerAABB.intersect(rightEdge))
        {
            // Set the player's position to the right edge of the collider.
            pos.x = colliderAABB.max.x + playerAABB.width() * 0.5f;
            //and 0 the X velocity
            vel.x = 0.0f;

            // Player is on a wall that is to the left of the player.
            // onLeftWall = true;
            

            continue;
        }
    }

    player.setPosition(pos);
    player.setVelocity(vel);
}

void Level::updatePickups(float deltaTime)
{
}

void Level::updateEffects(float deltaTime)
{
}

void Level::testUpdateCollisions(float deltaTime)
{
    bool onGround = false;
    //Update player
    player.update(deltaTime);

    AABB playerAABB = player.getAABB();

    //Get player velocity and position
    glm::vec2 vel = player.getVelocity();
    glm::vec2 pos = player.getPosition();

    for (auto& collider : colliders) 
    {
        //Collider AABB
        AABB colliderAABB = collider.aabb;

        if(colliderAABB.min.x < 0)
        {
            vel.y = 0.0f;
            pos.y = colliderAABB.min.y;
            // Change to running state
            player.setState(Player::State::Running);
            onGround = true;

            continue;
        }
    }
}
