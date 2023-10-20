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

Timer jumpTimer;

static std::map <Player::State, std::string> stateMap = {
	{Player::State::None, "None"},
	{Player::State::Idle, "Idle"},
	{Player::State::Running, "Running"},
	{Player::State::Jumping, "Jumping"},
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
	JumpAnim = SpriteAnim{ runSprites, 6 };

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

	if (glm::length(velocity) > 0)
	{
		setState(State::Running);
	}
	else if (glm::length(velocity.y) > 0)
	{
		setState(State::Jumping);
	}
	else
	{
		setState(State::Idle);
	}

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
	}
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
	}
#if _DEBUG
	image.drawAABB(camera * getAABB(), Color::Yellow, {}, FillMode::WireFrame);
	//image.drawText(Font::Default, stateMap[state], t[2][0], t[2][1], Color::Black);
#endif
}

void Player::Gravity(float deltaTime)
{
	
	velocity.y += gravity * deltaTime;
	
}

void Player::Jump()
{
	velocity.y -= jumpSpeed * jumpHeight;
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
			//Jump(/*deltaTime*/);
			/*while (true)
			{
				gravity = 0.0f;
			}*/
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
	//newPos.y -= Input::getAxis("Vertical") * speed * deltaTime;

	velocity = (newPos - initialPos) / deltaTime;

	Gravity(deltaTime); 
	if (Input::getButton("Jump")) {
		Jump();
	}

	initialPos += velocity * deltaTime;

	transform.setPosition(initialPos);
}

void Player::doIdle(float deltaTime)
{
	doMovement(deltaTime);

	if(glm::length(velocity) > 0)
	{
		setState(State::Running);
	}

	if (Input::getButton("Jump") && glm::length(velocity.y) > 0)
	{
		//Jump();
		setState(State::Jumping);
	}

	IdleAnim.update(deltaTime);
}

void Player::doRunning(float deltaTime)
{
	doMovement(deltaTime);

	if(glm::length(velocity) == 0.0f)
	{
		setState(State::Idle);
	}

	if (Input::getButton("Jump") && glm::length(velocity.y) > 0)
	{
		setState(State::Jumping);
	}

	RunAnim.update(deltaTime);

}

void Player::doFalling(float deltaTime)
{
}

void Player::doJumping(float deltaTime)
{
	doMovement(deltaTime);
	JumpAnim.update(deltaTime);
}
