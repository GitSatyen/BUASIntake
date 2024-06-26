#pragma once

#include "Player.hpp"
#include "Pickup.hpp"
#include "Effect.hpp"
#include "States.hpp"

#include <Audio/Sound.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/TileMap.hpp>
#include <Math/Camera2D.hpp>
#include <Math/AABB.hpp>

#include <LDtkLoader/Level.hpp>
#include <LDtkLoader/World.hpp>

using namespace Math;
using namespace Graphics;

struct Collider
{
    Math::AABB aabb;
    bool Spike;
};

class Level
{
public:

    enum class groundState
    {
        None,
        OnGround,
        OffGround
    };

    //Default constructor
	Level() = default;
	Level(const ldtk::Project& project, const ldtk::World& world, const ldtk::Level& level);

    void update(float deltaTime);

    void parseCollectables();

    //Reset level
    void reset();

    const Player& getPlayer() const noexcept
    {
        return player;
    }

    const Status& getStatus() const noexcept
    {
        return status;
    }

    void draw( Graphics::Image& image );

    void clearLevel(Graphics::Image& image);

protected:
    
private:
    // Check collision with a pickup and an AABB collider
    void checkPickupCollision(const Math::Sphere& pickupCollider, const Math::AABB& colliderAABB, glm::vec2& pos, glm::vec2& vel);
    void updateCollisions(float deltaTime);
    void updatePickups(float deltaTime);

    const ldtk::World* world = nullptr;
    const ldtk::Level* level;
    
    std::string levelName;

    // Level colliders
    std::vector<Collider> colliders;

    //Coin sprites
    std::map<std::string, std::shared_ptr<Graphics::SpriteSheet>> coinSprites;

    // All pickups
    std::vector<Pickup> allPickups;
    std::vector<Effect> effects;
    // Sprite animation to play when a pickup is collected.
    //Graphics::SpriteAnim pickupCollected;

    // Level tile map
    TileMap tileMap; // Terrain layer
    TileMap bg_Map; // Background layer
    TileMap spikeMap; // Spikes layer   

    Audio::Sound deathSound;
    Audio::Sound collectSound;
    Audio::Sound winSound;

    Player    player;

    glm::vec2 playerStart{ 0 };
    glm::vec2 pos = player.getPosition();

    uint64_t score = 0;
    std::string scoreCount = "Gold: ";

    Status status = Status::Start;
};