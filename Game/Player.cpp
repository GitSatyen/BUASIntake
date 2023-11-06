#include "Player.hpp"
#include <Graphics/Input.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Graphics/Timer.hpp>
#include <Math/Camera2D.hpp>
#include <map>
#include <string>

using namespace Graphics;
using namespace Math;


static std::map <Player::State, std::string> stateMap = {
	{Player::State::None, "None"},
	{Player::State::Idle, "Idle"},
	{Player::State::Running, "Running"},
	{Player::State::Jumping, "Jumping"},
	{Player::State::Falling, "Falling"},
	{Player::State::Attack, "Attack"},
	{Player::State::Dead, "Dead"}
};


Player::Player() = default;
	

Player::Player(const glm::vec2 & pos)
	//: Entity{ pos, AABB{{18, 10, 0}, {36, 43, 0}} }
	//: position{ pos }
	: aabb{ {20, 5, 0}, {45, 40, 0} }
{
	auto idle_sprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/01-Idle/Idle.png", 66, 40, 0, 0, BlendMode::AlphaBlend);
	//auto idle_sprites = ResourceManager::loadSpriteSheet("assets/Spirit Boxer/Idle.png", 137, 44, 0, 0, BlendMode::AlphaBlend);
	IdleAnim = SpriteAnim{ idle_sprites, 6 };

	auto runSprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/02-Run/Run.png", 66, 40, 0, 0, BlendMode::AlphaBlend);
	RunAnim = SpriteAnim{ runSprites, 6 };

	auto jumpSprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/03-Jump/Jump.png", 64, 40, 0, 0, BlendMode::AlphaBlend);
	JumpAnim = SpriteAnim{ jumpSprites, 6 };

	auto fallSprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/04-Fall/Fall.png", 64, 40, 0, 0, BlendMode::AlphaBlend);
	FallAnim = SpriteAnim{ fallSprites, 6 };

	setState(State::Idle);

	//Set anchor points
	transform.setAnchor({ 35,40 });
	transform.setPosition(pos);
}

void Player::update(float deltaTime)
{
	auto initalPos = transform.getPosition();
	auto newPos = initalPos;

	newPos.x += Input::getAxis("Horizontal"), speed * deltaTime;
	//newPos.y -= Input::getAxis("Vertical"), speed* deltaTime;
	//newPos.y += gravity * deltaTime;

	velocity = (newPos - initalPos) / deltaTime;

	transform.setPosition(newPos);

	//Detects players direction its facing
	if(velocity.x < 0.0f)
	{
		transform.setScale({ -1, 1 });
	}
	else if(velocity.x > 0.0)
	{
		transform.setScale({ 1, 1 });
	}

	if ( Input::getAxis("Horizontal") != 0  /*Input::getAxis("Horizontal") == 0 && speed == 0.0f && glm::length(velocity) == 0.0f*/ )
	{
		setState(State::Running);
		doRunning(deltaTime);
	}
	if (Input::getButton("Jump") /*&&  velocity.y < 0*/)
	{
		setState(State::Jumping);
	}

	if (velocity.y < 0.0f)
	{
		setState(State::Falling);
		doFalling(deltaTime);
	}

	else 
	{
		setState(State::Idle);
	}

	//Checks if player is on ground
	(aabb.min.x <= 0) ? onGround = true : onGround = false; 


	switch (state)
	{
	case State::Idle:
		doIdle(deltaTime);
		break;
	case State::Running:
		doRunning(deltaTime);
		break;	
	case State::Jumping:
		doJumping(deltaTime);
		break;
	case State::Falling:
		doFalling(deltaTime);
		break;
	}

	// Update player position.
	transform.translate(glm::vec2{ velocity.x, -velocity.y } *deltaTime);


	// Update jump timer.
	jumpTimer += deltaTime;

	//if (currentCharacter)
	//{
	//	currentCharacter->update(deltaTime);
	//}
}

void Player::draw(Graphics::Image& image, const Math::Camera2D& camera)
{
	//Translation matrix
	/*glm::mat3 t = {
		1, 0, 0,
		0, 1, 0,
		position.x, position.y, 1
	};*/
	//t = transform * t;

	switch (state)
	{
	case State::Idle:
		image.drawSprite(IdleAnim, camera * transform);
		break;
	case State::Running:
		image.drawSprite(RunAnim, camera * transform);
		break;
	case State::Jumping:
		image.drawSprite(JumpAnim, camera * transform);
	case State::Falling:
		image.drawSprite(FallAnim, camera * transform);
	}
#if _DEBUG
	image.drawAABB(camera * getAABB(), Color::Yellow, {}, FillMode::WireFrame);
	auto pos = camera * transform;
	image.drawText(Font::Default, stateMap[state], pos[2][0], pos[2][1], Color::Cyan);
#endif
}

void Player::Gravity(float deltaTime)
{
	velocity.y += gravity * deltaTime;
}

void Player::Jump()
{
//	velocity.y -= jumpSpeed * jumpHeight;
}

void Player::setPosition(const glm::vec2& pos)
{
	transform.setPosition(pos);
}

const glm::vec2& Player::getPosition() const
{
	return transform.getPosition();
}

void Player::translate(const glm::vec2& t)
{
	transform.translate(t);
}

const Math::AABB Player::getAABB() const
{
	return transform * aabb;
}

void Player::setState(State newState)
{
	if(newState != state)
	{
		switch (newState)
		{
		//Calls action when traniston away from state
		case State::Idle:
			break;
		case State::Running:
			break;
		case State::Jumping:
			velocity.y = jumpSpeed;
			break;
		case State::Falling:
			break;
		case State::Attack:
			break;
		case State::Dead:
			break;
		}
		state = newState;
	}
}

//float Player::doHorizontalMovement(float deltaTime)
//{
//	const float horizontal = Input::getAxis("Horizontal") * speed * deltaTime;
//
//	if (horizontal < 0.0f)
//	{
//		transform.setScale({ -1, 1 });
//	}
//	else if (horizontal > 0.0f)
//	{
//		transform.setScale({ 1, 1 });
//	}
//
//	return horizontal;
//}

void Player::doMovement(float deltaTime)
{
	auto initialPos = transform.getPosition();
	auto newPos = initialPos;

    newPos.x += Input::getAxis("Horizontal") * speed * deltaTime;
	//newPos.y -= Input::getAxis("Vertical") * speed * deltaTime;

	velocity = (newPos - initialPos) / deltaTime;

	
	//if (Input::getButton("Jump")) { Jump(); }
	

	initialPos += velocity * deltaTime;

	transform.setPosition(initialPos);
}

void Player::doIdle(float deltaTime)
{
	doMovement(deltaTime);

	if(Input::getAxis("Horizontal") != 0)
	{
		setState(State::Running);
	}

	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
	}

	IdleAnim.update(deltaTime);
}

void Player::doRunning(float deltaTime)
{
	doMovement(deltaTime);
	/*const float horizontal = doHorizontalMovement(deltaTime);
	velocity.x += horizontal;*/

	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
	}

	 if (/*horizontal == 0.0f*/ Input::getAxis("Horizontal") == 0 && speed == 0.0f && glm::length(velocity) == 0.0f)
	{
		setState(State::Idle);
	}

	RunAnim.update(deltaTime);

}

void Player::doJumping(float deltaTime)
{
	doMovement(deltaTime);
	/*const float horizontal = doHorizontalMovement(deltaTime);
	velocity.x += horizontal;*/

	velocity.y += gravity * deltaTime;
	//velocity.y -= jumpSpeed * jumpHeight;

	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
	}
	//(aabb.min.x <= 0) ? onGround = true : onGround = false;
	else if (jumpTime > 0.5f)
	{
		setState(State::Falling);
		//Gravity(deltaTime);
	}

	jumpTime += deltaTime;
	JumpAnim.update(deltaTime);
}

void Player::doFalling(float deltaTime)
{
	doMovement(deltaTime);
	/*const float horizontal = doHorizontalMovement(deltaTime);
	velocity.x += horizontal;*/

	velocity.y += gravity * deltaTime;
	//Gravity(deltaTime);
	if (Input::getButtonDown("Jump"))
	{
		// Allow jumping for a short time after starting to fall.
		if (fallTimer < landTime)  
			setState(State::Jumping);
		else { jumpTimer == 0.0f; }
	}
	fallTimer += deltaTime;

	FallAnim.update(deltaTime);
}