#pragma once

#include <Graphics/SpriteAnim.hpp>
#include <glm/vec2.hpp>

#include <Math/AABB.hpp>

using namespace Graphics;

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

	explicit Player(const glm::vec2& pos, const Graphics::SpriteAnim& sprite);

	void update(float deltaTime);

	void draw(Graphics::Image& image);

	void setPosition(const glm::vec2& pos);
	const glm::vec2& getPosition() const;

	void translate(const glm::vec2& t);

	const Math::AABB getAABB() const;

private:
	void setState(State newState);

	State state = State::None;
	glm::vec2 position{ 0 };
	glm::vec2 velocity{ 0 };
	float speed{ 60.0f };
	Graphics::SpriteAnim IdleAnim;
	Graphics::SpriteAnim RunAnim;
	Graphics::SpriteAnim AttackAnim;
	Math::AABB aabb;
};