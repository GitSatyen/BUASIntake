#include "Player.hpp"
#include <Graphics/Input.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Math/Camera2D.hpp>
#include <map>

using namespace Graphics;
using namespace Math;

static std::map <Player::State, std::string> stateMap = {
	{Player::State::None, "None"},
	{Player::State::Idle, "Idle"},
	{Player::State::Running, "Running"},
	{Player::State::Attack, "Attack"},
	{Player::State::Dead, "Dead"},
};


Player::Player() = default;
	

Player::Player(const glm::vec2 & pos)
	: position{pos}
	, aabb{ {8, 16, 0}, {24, 38, 0}}
	
{
	auto idle_sprites = ResourceManager::loadSpriteSheet("assets/Spirit Boxer/Idle.png", 137, 44, 0, 0, BlendMode::AlphaBlend);
	IdleAnim = SpriteAnim{ idle_sprites, 6 };
	auto runSprites = ResourceManager::loadSpriteSheet("assets/Spirit Boxer/Run.png", 137, 44, 0, 0, BlendMode::AlphaBlend);
	RunAnim = SpriteAnim{ runSprites, 6 };

	setState(State::Idle);
}

void Player::update(float deltaTime)
{
	/*auto initalPos = transform.getPosition();
	auto newPos = initalPos;

	newPos.x += Input::getAxis("Horizontal"), speed * deltaTime;
	newPos.y -= Input::getAxis("Vertical"), speed* deltaTime;

	velocity = (position - initalPos) / deltaTime;

	transform.setPosition(newPos);

	if(glm::length(velocity) > 0)
	{
		setState(State::Running);
	}
	else {
		setState(State::Idle);
	}
	IdleAnim.update(deltaTime);
	RunAnim.update(deltaTime);*/

	switch (state)
	{
	case State::Idle:
		doIdle(deltaTime);
		break;
	case State::Running:
		doRunning(deltaTime);
		break;
	}
}

void Player::draw(Graphics::Image& image, const glm::mat3& transform)
{
	//Translation matrix
	glm::mat3 t = {
		1, 0, 0,
		0, 1, 0,
		position.x, position.y, 1
	};
	t = transform * t;

	switch (state)
	{
		case State::Idle:
			image.drawSprite(IdleAnim, t);
			break;
		case State::Running:
			image.drawSprite(RunAnim, t);
			break;
	}
#if _DEBUG
	image.drawAABB(transform * getAABB(), Color::Yellow, {}, FillMode::WireFrame);
	image.drawText(Font::Default, stateMap[state], t[2][0], t[2][1], Color::Black);
#endif
}

void Player::setPosition(const glm::vec2& pos)
{
	position = pos;
}

const glm::vec2& Player::getPosition() const
{
	return position;
}

void Player::translate(const glm::vec2& t)
{
	transform.translate(t);
}

const Math::AABB Player::getAABB() const
{
	return aabb + glm::vec3{ position, 0 };
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
	auto initialPos = position;

	position.x += Input::getAxis("Horizontal") * speed * deltaTime;
	position.y -= Input::getAxis("Vertical") * speed * deltaTime;

	velocity = (position - initialPos) / deltaTime;
}

void Player::doIdle(float deltaTime)
{
	//auto initialPos = position;

	//position.x += Input::getAxis("Horizontal"), speed* deltaTime;
	//position.y -= Input::getAxis("Vertical"), speed* deltaTime;

	//velocity = (position - initialPos) / deltaTime;

	doMovement(deltaTime);

	if(glm::length(velocity) > 0)
	{
		setState(State::Running);
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
	RunAnim.update(deltaTime);

}

