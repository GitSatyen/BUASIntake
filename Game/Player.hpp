#pragma once

#include "Entity.hpp"
#include <Graphics/SpriteAnim.hpp>
#include <glm/vec2.hpp>

#include <Math/AABB.hpp>
namespace Math
{
	class Camera2D;
}

namespace Graphics
{
	class Image;
}

class Player : public Entity
{
public:

	enum class State
	{
		None,
		Idle,
		Running,
		Attack,
		Dead
	};
	// Default constructor
	Player();

	explicit Player(const glm::vec2& pos);

	virtual void update(float deltaTime) override;
	//void draw(Graphics::Image& image, const glm::mat3& transform);
	virtual void draw(Graphics::Image& image, const Math::Camera2D& camera) override;

	void setPosition(const glm::vec2& pos);
	const glm::vec2& getPosition() const;

	void translate(const glm::vec2& t);

	const Math::AABB getAABB() const;

private:
	void setState(State newState);

	void doMovement(float delatime);
	void doIdle(float deltaTime);
	void doRunning(float deltaTime);

	
	Math::Transform2D transform;
	glm::vec2 position{ 0 };
	glm::vec2 velocity{ 0 };
	float speed{ 60.0f };

	State state = State::None;
	Graphics::SpriteAnim IdleAnim;
	Graphics::SpriteAnim RunAnim;
	Graphics::SpriteAnim AttackAnim;
	Math::AABB aabb;
};