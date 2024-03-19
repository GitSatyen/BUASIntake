#include "Player.hpp"
#include <Graphics/Input.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/ResourceManager.hpp>
//#include <Graphics/Timer.hpp>
#include <Math/Camera2D.hpp>
#include <map>
#include <string>

using namespace Graphics;
using namespace Math;

// Time inverval in seconds
float jumpTimer = 1.0f;

static std::map <Player::State, std::string> stateMap = {
	{Player::State::None, "None"},
	{Player::State::Idle, "Idle"},
	{Player::State::Running, "Running"},
	{Player::State::Jumping, "Jumping"},
	{Player::State::Falling, "Falling"},
	{Player::State::Attack, "Attack"},
	{Player::State::Dead, "Dead"}
};

Player::Player(const glm::vec2 & pos)
{
	aabb = AABB{ {25, 5, 0}, {45, 40, 0} };

    auto idle_sprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/01-Idle/Idle.png", 64, 40, 0, 0, BlendMode::AlphaBlend);
	IdleAnim = SpriteAnim{ idle_sprites, 6 };

	auto runSprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/02-Run/Run.png", 64, 40, 0, 0, BlendMode::AlphaBlend);
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

	newPos.x += Input::getAxis("Horizontal") * speed * deltaTime;
	//newPos.y -= Input::getAxis("Vertical"), speed* deltaTime;
	//newPos.y += gravity * deltaTime;

	velocity = (newPos - initalPos) / deltaTime;

	// Test function from Jeremiah  
	// Detecing if player position goes out of colliders bounds
	/*if(abs(newPos.x - initalPos.x) >= 16)
	{
 		int i = 3;
	}*/

	transform.setPosition(newPos);

	// Detects players direction its facing
	if(velocity.x < 0.0f)
	{
		transform.setScale({ -1, 1 });
	}
	else if(velocity.x > 0.0)
	{
		transform.setScale({ 1, 1 });
	}

	// Set player to Idle
	/*if (Input::getAxis("Horizontal") == 0 && velocity.x == 0.0f)
	{
		setState(State::Idle);
		doRunning(deltaTime);
	}*/

	// Set player to Running
	/*if ( Input::getAxis("Horizontal") != 0 )
	{
		setState(State::Running);
		doRunning(deltaTime);
	}*/

	// Set player to Jumping
	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
		doJumping(deltaTime);
	}

	// Set player to Falling
	if (velocity.y <= 0.0f)
	{
		setState(State::Falling);
		doFalling(deltaTime);
	}

	Gravity(deltaTime);

	// Switch animation based on state its in
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

	// Update player position
	transform.translate(glm::vec2{ velocity.x, velocity.y } *deltaTime);
}

void Player::draw(Graphics::Image& image)
{
	switch (state)
	{
	case State::Idle:
		image.drawSprite(IdleAnim, transform);
		break;
	case State::Running:
		image.drawSprite(RunAnim, transform);
		break;
	case State::Jumping:
		image.drawSprite(JumpAnim, transform);
		break;
	case State::Falling:
		image.drawSprite(FallAnim, transform);
	}
#if _DEBUG
	image.drawAABB(getAABB(), Color::Yellow, {}, FillMode::WireFrame);
	auto pos = transform.getPosition();
	image.drawText(Font::Default, stateMap[state], pos.x, pos.y - 50.0f, Color::Cyan);
#endif
}

void Player::Gravity(float deltaTime)
{
	velocity.y += gravity * deltaTime;
}

void Player::setState(State newState)
{
	if(newState != state)
	{
		switch (newState)
		{
		//Calls action when traniston away from state
		case State::Idle:
			if (Input::getAxis("Horizontal") != 0)
				newState = State::Running;
			break;	
		case State::Running:
			if (Input::getAxis("Horizontal") == 0 && velocity.x == 0.0f)
				newState = State::Idle;
			break;
		case State::Jumping:
			//velocity.y = jumpSpeed;
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

void Player::doMovement(float deltaTime)
{
	auto initialPos = transform.getPosition();
	auto newPos = initialPos;

    newPos.x += Input::getAxis("Horizontal") * speed * deltaTime;

	velocity = (newPos - initialPos) / deltaTime;

	initialPos += velocity * deltaTime;

	transform.setPosition(initialPos);
}

void Player::doIdle(float deltaTime)
{
	doMovement(deltaTime);
	
	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
	}

	IdleAnim.update(deltaTime);
}

void Player::doRunning(float deltaTime)
{
	doMovement(deltaTime);

	velocity.y -= gravity * deltaTime;

	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
	}

	RunAnim.update(deltaTime);
}

void Player::doJumping(float deltaTime)
{
	doMovement(deltaTime);

	//***Adjust this
	velocity.y -=  jumpSpeed + gravity * deltaTime;

	/*jumpTime += 0.1f;
	std::this_thread::sleep_for(std::chrono::duration<float>(jumpTimer));*/

	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
	}

	//if(jumpTime == 0.5f )
	//{
	//	velocity.y = 0;
	//}

	//if (velocity.y < 0.0f || jumpTime == 0.5f)
	//{
	//	setState(State::Falling);
	//}

	//Test jump timer
	/*else if(jumpTimer.elapsedSeconds() == 0.5f)
	{
		velocity.y = 0;
		doFalling(deltaTime);
	}*/
	JumpAnim.update(deltaTime);
}

void Player::doFalling(float deltaTime)
{
	//The first number is for testing
	//velocity.y -= 80 + gravity * deltaTime;
	Gravity(deltaTime);
	
	FallAnim.update(deltaTime);
}