#pragma once

#include <Graphics/Image.hpp>
#include <Math/AABB.hpp>
#include <Math/Transform2D.hpp>
#include <Math/Camera2D.hpp>

class Entity
{
public:
	virtual ~Entity() = default;

	virtual void update(float deltaTime) = 0;
	virtual void draw(Graphics::Image& image, const Math::Camera2D& camera) = 0;

	void setPosition(const glm::vec2& pos);
	const glm::vec2& getPosition() const;

	void translate(const glm::vec2& t);

	const Math::AABB getAABB() const;

	virtual bool collides(const Entity& entity) const;

protected:
	Entity() = default;
	Entity(const glm::vec2& pos, const Math::AABB& aabb);

	Math::AABB aabb;
	Math::Transform2D transform;
};