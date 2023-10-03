#pragma once

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

class Player
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

	void update(float deltaTime);

	void draw(Graphics::Image& image,const glm::mat3& transform);

	void setPosition(const glm::vec2& pos);
	const glm::vec2& getPosition() const;

	void translate(const glm::vec2& t);

	const Math::AABB getAABB() const;

private:
	void setState(State newState);

	void doMovement(float delatime);
	void doIdle(float deltaTime);
	void doRunning(float deltaTime);

	State state = State::None;
	Math::Transform2D transform;
	glm::vec2 position{ 0 };
	glm::vec2 velocity{ 0 };
	float speed{ 60.0f };
	Graphics::SpriteAnim IdleAnim;
	Graphics::SpriteAnim RunAnim;
	Graphics::SpriteAnim AttackAnim;
	Math::AABB aabb;
};