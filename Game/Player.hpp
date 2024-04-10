#pragma once

#include "Entity.hpp"
#include <Graphics/SpriteAnim.hpp>
#include <Graphics/Timer.hpp>
#include <glm/vec2.hpp>

#include <Math/AABB.hpp>
#include <vector>
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
		Jumping,
		Falling,
		Landing,
		Attack,
		Hit,
		Dead
	};
	// Default constructor
	Player() = default;
	//Copy constructor;
	/*Player(const Player&) = default;
	Player(Player&&) = default;
	Player& operator=(const Player&) = default;
	Player& operator=(Player&&) = default;*/

	explicit Player(const glm::vec2& pos);

	void setState(State newState);

	State getState() const noexcept
	{
		return state;
	}

	virtual void update(float deltaTime) override;
	virtual void draw(Graphics::Image& image) override;
	virtual void Gravity(float deltaTime);

	void setVelocity(const glm::vec2& vel) noexcept
	{
		velocity = vel;
	}
	const glm::vec2& getVelocity() const noexcept
	{
		return velocity;
	}

private:
	void doMovement(float delatime);
	void doIdle(float deltaTime);
	void doRunning(float deltaTime);
	void doFalling(float deltaTime);
	void doJumping(float deltaTime);
	void doHit(float deltaTime);
	void doDead(float deltaTime);

	int hp = 3;
	glm::vec2 velocity { 0 };
	float accel{ 180.0f };
	float maxSpeed = 90.0f;
	static inline const float jumpHeight = 50.0f;
	static inline const float jumpTime = 0.3f;
	static inline const float gravity = 3.0f * jumpHeight / (jumpTime * jumpTime);
	static inline const float jumpSpeed = std::sqrt(2.0f * jumpHeight * gravity);	

	State state = State::None;
	Graphics::SpriteAnim IdleAnim;
	Graphics::SpriteAnim RunAnim;	
	Graphics::SpriteAnim JumpAnim;
	Graphics::SpriteAnim FallAnim;
	Graphics::SpriteAnim HitAnim;
	Graphics::SpriteAnim DeadAnim;
};