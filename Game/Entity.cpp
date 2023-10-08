#include "Entity.hpp"
#include <iostream>

Entity::Entity(const glm::vec2& pos, const Math::AABB& aabb)
	: transform{ pos }
	, aabb{ aabb }
{}

void Entity::setPosition(const glm::vec2& pos)
{
	transform.setPosition(pos);
}

const glm::vec2& Entity::getPosition() const
{
	return transform.getPosition();
}

void Entity::translate(const glm::vec2& t)
{
	transform.translate(t);
}

const Math::AABB Entity::getAABB() const
{
	return transform * aabb;
}

bool Entity::collides(const Entity& entity) const
{
	return getAABB().intersect(entity.getAABB());
}
