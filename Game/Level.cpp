#include "LDtkLoader/Project.hpp"
#include "Player.hpp"
#include "Level.hpp"

#include <Graphics/BlendMode.hpp>
#include <Graphics/Color.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Graphics/Image.hpp>

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
        const auto& entities = level.getLayer("Entities");

        // Colected animation effect
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
        player.setPosition(playerStart);
}

void Level::update(float deltaTime)
{
        updateCollisions(deltaTime);
        updatePickups(deltaTime);
        updateEffects(deltaTime);

        Player::State playerState = player.getState();

        //player.setPosition(pos);
     
}

void Level::reset()
{
}

void Level::setCharacter(size_t characterId)
{
}

void Level::draw(Graphics::Image& image, const glm::mat3 transform) 
{
    tileMap.draw(image,  transform);

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
    // Store the previous position so we know if the
   // player moved through a 1-way collider.
    glm::vec2 prevPos = player.getPosition();

    // Update Player.
    player.update(deltaTime);

    // If player is in the "Hit" state, then no collision occurs with the player.
  /*  if (player.getState() == Player::State::Hit)
        return;*/

    // Check player collision
    AABB playerAABB = player.getAABB();

    glm::vec2 vel = player.getVelocity();
    glm::vec2 pos = player.getPosition();

    // Number of pixels padding to account for collisions.
    const float padding = 3.0f;

    bool onGround = false;
    bool onLeftWall = false;
    bool onRightWall = false;
    bool isHit = false;  // Set to true if the player hits a trap.
    for (auto& collider : colliders)
    {
        AABB colliderAABB = collider.aabb;

        // Player is moving down (falling).
        if (vel.y < 0.0f)
        {
            // Check to see if the player is colliding with the top edge of the collider.
            Line topEdge{ { colliderAABB.min.x + padding, colliderAABB.min.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.min.y, 0 } };
            if (playerAABB.intersect(topEdge))
            {
                // We only collide with 1-way colliders if the player's previous position was above the collider.
                if (!collider.isOneWay || prevPos.y < colliderAABB.min.y)
                {
                    // Set the player's position to the top of the AABB.
                    pos.y = colliderAABB.min.y;
                    // And 0 the Y velocity.
                    vel.y = 0.0f;

                    if (collider.isTrap)
                    {
                        isHit = true;
                    }
                    else
                    {
                        // And change state to running (to preserve momentum.)
                        player.setState(Player::State::Running);
                        onGround = true;
                    }

                    continue;
                }
            }
        }
        // Player is moving up (jumping)
        else if (vel.y > 0.0f)
        {
            // Check to see if the player is colliding with the bottom edge of the collider.
            Line bottomEdge{ { colliderAABB.min.x + padding, colliderAABB.max.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.max.y, 0 } };
            if (!collider.isOneWay && playerAABB.intersect(bottomEdge))
            {
                // Set the player's position to the bottom of the collider.
                pos.y = colliderAABB.max.y + playerAABB.height();
                // And 0 the Y velocity.
                vel.y = 0.0f;

                if (collider.isTrap)
                {
                    isHit = true;
                }
                else
                {
                    // And start falling
                    player.setState(Player::State::Falling);
                }

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
                if (collider.isTrap)
                {
                    isHit = true;
                }
                else
                {
                    onGround = true;
                }
            }
        }

        // Check to see if the player is colliding with the left edge of the collider.
        Line leftEdge{ { colliderAABB.min.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.min.x, colliderAABB.max.y - padding, 0 } };
        if (!collider.isOneWay && playerAABB.intersect(leftEdge))
        {
            // Set the player's position to the left edge of the collider.
            pos.x = colliderAABB.min.x - playerAABB.width() * 0.5f;
            // And 0 the X velocity.
            vel.x = 0.0f;

            if (collider.isTrap)
            {
                isHit = true;
            }
            else
            {
                // On a wall that is right of the player.
                onRightWall = true;
            }

            continue;
        }
        // Check to see if the player is colliding with the right edge of the collider.
        Line rightEdge{ { colliderAABB.max.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.max.x, colliderAABB.max.y - padding, 0 } };
        if (!collider.isOneWay && playerAABB.intersect(rightEdge))
        {
            // Set the player's position to the right edge of the collider.
            pos.x = colliderAABB.max.x + playerAABB.width() * 0.5f;
            // And 0 the X velocity.
            vel.x = 0.0f;

            if (collider.isTrap)
            {
                isHit = true;
            }
            else
            {
                // Player is on a wall that is to the left of the player.
                onLeftWall = true;
            }

            continue;
        }
    }
}

void Level::updatePickups(float deltaTime)
{
}

void Level::updateEffects(float deltaTime)
{
}

void Level::updateBoxes(float deltaTime)
{
}
