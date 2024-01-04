#pragma once

#include "Entity.hpp"
#include <Graphics/SpriteAnim.hpp>
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
		Dead
	};
	// Default constructor
	Player() {};
	//Player() = default;
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
	virtual void draw(Graphics::Image& image, const Math::Camera2D& camera) override;
	virtual void Gravity(float deltaTime);
	virtual void Jump();

	void setVelocity(const glm::vec2& vel) noexcept
	{
		velocity = vel;
	}
	const glm::vec2& getVelocity() const noexcept
	{
		return velocity;
	}

	void setPosition(const glm::vec2& pos);	
	const glm::vec2& getPosition() const;

	void translate(const glm::vec2& t);
	const Math::AABB getAABB() const;

private:
	
	float doHorizontalMovement(float deltaTime);
	void doMovement(float delatime);
	void doIdle(float deltaTime);
	void doRunning(float deltaTime);
	void doFalling(float deltaTime);
	void doJumping(float deltaTime);


	Math::Transform2D transform;
	//glm::vec2 position{ 0 };
	glm::vec2 velocity { 0 };
	float speed{ 60.0f };
	bool onGround = true;
	static inline const float jumpHeight = 55.0f;
	static inline  float jumpTime = 0.5f;
	static inline const float landTime = 0.2f;
	static inline  float gravity = 10 * jumpHeight / (jumpTime * jumpTime);
	static inline const float jumpSpeed = std::sqrt(2.0f * jumpHeight * gravity);
	float fallTimer = 0.1f;
	float jumpTimer = 0.25f;



	Graphics::Sprite* Backside;

	State state = State::None;
	Graphics::SpriteAnim IdleAnim;
	Graphics::SpriteAnim RunAnim;	
	Graphics::SpriteAnim JumpAnim;
	Graphics::SpriteAnim FallAnim;
	Graphics::SpriteAnim AttackAnim;
	Math::AABB aabb;
};