#include "Player.hpp"
#include <Graphics/Input.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Math/Camera2D.hpp>

#include <map>
#include <string>
#include <fmt/core.h>
#include <iostream>

using namespace Graphics;
using namespace Math;

bool isDead = false;

static std::map <Player::State, std::string> stateMap = {
	{Player::State::None, "None"},
	{Player::State::Idle, "Idle"},
	{Player::State::Running, "Running"},
	{Player::State::Jumping, "Jumping"},
	{Player::State::Falling, "Falling"},
	{Player::State::Attack, "Attack"},
	{Player::State::Hit, "Hit"},
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

	auto hitSprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/06-Hit/Hit.png", 64, 40, 0, 0, BlendMode::AlphaBlend);
	HitAnim = SpriteAnim{ hitSprites, 6 };

	auto deadSprites = ResourceManager::loadSpriteSheet("assets/Treasure Hunters/Player/08-Dead Ground/Dead.png", 64, 40, 0, 0, BlendMode::AlphaBlend);
	DeadAnim = SpriteAnim{ deadSprites, 6 };

	setState(State::Idle);

	//Set anchor points
	transform.setAnchor({ 35,40 });
	transform.setPosition(pos);
}

void Player::update(float deltaTime)
{	
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
	case State::Hit:
		doHit(deltaTime);
		break;
	case State::Dead:
		doDead(deltaTime);
		break;
	}


	// Detects players direction its facing
	if (velocity.x < 0.0f)
	{
		transform.setScale({ -1, 1 });
	}
	else if (velocity.x > 0.0)
	{
		transform.setScale({ 1, 1 });
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
		break;
	case State::Hit:
		image.drawSprite(HitAnim, transform);
		break;
	case State::Dead:
		image.drawSprite(DeadAnim, transform);
	}
#if _DEBUG
	image.drawAABB(getAABB(), Color::Yellow, {}, FillMode::WireFrame);
	auto pos = transform.getPosition();
	image.drawText(Font::Default, stateMap[state], pos.x, pos.y - 50.0f, Color::Cyan);
	image.drawText(Font::Default, fmt::format("HP:{:}", hp), 700, 70, Color::Green);
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
			break;	
		case State::Running:
			break;
		case State::Jumping:
			velocity.y -= jumpSpeed;
			break;
		case State::Falling:
		{int i = 3; }
			break;
		case State::Hit:
			hp--;
			velocity.y -= 5;
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

    velocity.x += Input::getAxis("Horizontal") * accel * deltaTime;

	velocity.x = velocity.x / deltaTime;

	// Drag speed
	if (velocity.x > maxSpeed)
	{
		velocity.x = maxSpeed;
	}

	if (velocity.x < -maxSpeed)
	{
		velocity.x = -maxSpeed;
	}

	initialPos += velocity * deltaTime;

	
	transform.setPosition(initialPos);

	if(Input::getAxis("Horizontal") == 0)
	{
		velocity.x = 0;
	}
}

void Player::doIdle(float deltaTime)
{
	//doMovement(deltaTime);
	
	if (Input::getAxis("Horizontal") != 0.0f) 
	{
		setState(State::Running);
	}

	if (Input::getAxis("Horizontal") == 0.0f)
	{
		velocity.x = 0.0f;
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

	if(velocity.x == 0.0f)
	{
		setState(State::Idle);
	}

	if (Input::getButton("Jump"))
	{
		setState(State::Jumping);
	}

	RunAnim.update(deltaTime);
}

void Player::doJumping(float deltaTime)
{
	doMovement(deltaTime);

	Gravity(deltaTime);

	if(jumpHeight > 50.0f)
	{
		setState(State::Falling);
	}
	
	JumpAnim.update(deltaTime);
}

void Player::doFalling(float deltaTime)
{
	Gravity(deltaTime);
	
	FallAnim.update(deltaTime);
}

void Player::doHit(float deltaTime)
{

	HitAnim.update(deltaTime);
}

void Player::doDead(float deltaTime)
{
	isDead = true;
	velocity.y = 0;
	velocity.x = 0;
	
	DeadAnim.update(deltaTime);
}
