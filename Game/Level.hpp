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
    //ColliderType type = ColliderType::Default;
    Math::AABB   aabb;
    bool         isOneWay = false;
    bool         isTrap = false;
};

class Level
{
public:
	Level() = default;
	Level(const ldtk::Project& project, const ldtk::World& world, const ldtk::Level& level);

    void update(float deltaTime);

    //Reset level
    void reset();

    const Player& getPlayer() const noexcept
    {
        return player;
    }

    void draw(Graphics::Image& image, const glm::mat3 transform);

protected:
    
private:
    // Add a pickup with a name, initial position.
    void addPickup(std::string_view name, const glm::vec2& pos);
    // Check collision with a pickup and an AABB collider.
    void checkPickupCollision(const Math::Sphere& pickupCollider, const Math::AABB& colliderAABB, glm::vec2& pos, glm::vec2& vel);
    void updateCollisions(float deltaTime);
    void updatePickups(float deltaTime);
    void updateEffects(float deltaTime);
    void updateBoxes(float deltaTime);

    void testUpdateCollisions(float deltaTime);

    const ldtk::World* world = nullptr;
    const ldtk::Level* level = nullptr;
    

    std::string levelName;

    // Level colliders.
    std::vector<Collider> colliders;

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
};