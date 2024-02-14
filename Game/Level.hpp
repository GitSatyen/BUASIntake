#pragma once

#include "Player.hpp"
#include "Pickup.hpp"
#include "Effect.hpp"

#include <Audio/Sound.hpp>

#include <Graphics/Image.hpp>
#include <Graphics/TileMap.hpp>
#include <Math/Camera2D.hpp>
#include <Math/AABB.hpp>

#include <LDtkLoader/Level.hpp>
#include <LDtkLoader/World.hpp>

using namespace Math;

struct Collider
{
    Math::AABB aabb;
};

class Level
{
public:
	Level() = default;
	Level(const ldtk::Project& project, const ldtk::World& world, const ldtk::Level& level);

    void update(float deltaTime);

    //Reset level
    void reset();


    //Checks for collision with the players AABBB for the pickups 
    bool colliding(const Player& player)
    {
        return player.getAABB().intersect(sphere);
    }

    const Player& getPlayer() const noexcept
    {
        return player;
    }

    void draw(Graphics::Image& image, const glm::mat3 transform);

    bool onGround = false;

protected:
    
private:
    // Add a pickup with a name, initial position.
    void addPickup(std::string_view name, const glm::vec2& pos);
    // Check collision with a pickup and an AABB collider.
    void checkPickupCollision(const Math::Sphere& pickupCollider, const Math::AABB& colliderAABB, glm::vec2& pos, glm::vec2& vel);
    void updateCollisions(float deltaTime);
    void updatePickups(float deltaTime);
    void updateEffects(float deltaTime);

    const ldtk::World* world = nullptr;
    const ldtk::Level* level = nullptr;
    
    std::string levelName;

    // Level colliders
    std::vector<Collider> colliders;

    //Coin sprites
    std::map<std::string, std::shared_ptr<Graphics::SpriteSheet>> coinSprites;

    //Pickup collider sphere
    Math::Sphere sphere;
    // All pickups
    std::vector<Pickup> allPickups;
    std::vector<Effect> effects;
    // Sprite animation to play when a pickup is collected.
    /*Graphics::SpriteAnim pickupCollected;
    // Currently playing effects.
    std::vector<Effect> effects;*/

    // Level tile map.
    Graphics::TileMap tileMap;

    Camera2D  camera;
    Player    player;

    glm::vec2 playerStart{ 0 };
    glm::vec2 pos = player.getPosition();

    uint64_t score = 0;
    std::string scoreCount = "Gold: ";
};