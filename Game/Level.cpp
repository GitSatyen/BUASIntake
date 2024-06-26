#include "LDtkLoader/Project.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "States.hpp"

#include <Graphics/BlendMode.hpp>
#include <Graphics/Color.hpp>
#include <Graphics/ResourceManager.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/Sprite.hpp>
#include <Graphics/TileMap.hpp>
#include <Graphics/Input.hpp>
#include <filesystem>

#include <fmt/core.h>
#include <map>
#include <string>

using namespace Math;
using namespace Graphics;

bool Finished = false;
extern bool isDead;

std::string check;
//ldtk::Project project;

static std::map <Level::groundState, std::string> statemap = {
	{Level::groundState::OnGround, "OnGround: True"},
	 {Level::groundState::OffGround, "OffGround: False"}
};

Level::Level(const ldtk::Project& project, const ldtk::World& world, const ldtk::Level& level)
	: world{ &world }
	, level{ &level }
	, levelName{ level.name }

{
	// Load in sound effects
	// Death sound effect
	std::string death_filePath = "assets\\sounds\\death.mp3";
	if (!std::filesystem::exists(death_filePath) || !std::filesystem::is_regular_file(death_filePath))
	{
		std::cerr << "Failed to open sound file" << std::endl;
		{int i = 3; };
	}
	deathSound.loadMusic(death_filePath);
	deathSound.setLooping(false);
	deathSound.setVolume(0.5f);

	const std::filesystem::path projectPath = project.getFilePath().directory();

	// Collect sound effect
	std::string sound_filePath = "assets\\sounds\\collect.mp3";
	if (!std::filesystem::exists(sound_filePath) || !std::filesystem::is_regular_file(sound_filePath))
	{
		std::cerr << "Failed to open sound file" << std::endl;
		{int i = 3; };
	}
	collectSound.loadSound(sound_filePath);
	collectSound.setLooping(false);
	collectSound.setVolume(1.0f);

	//Parse Layers
	//Entitie layer
	const auto& entities = level.getLayer("Entities");
	//Collision layer
	const auto& collisions = entities.getEntitiesByName("Collider");
	//Get position and size of the colliders
	for (auto& collision : collisions)
	{

		auto& e = collision.get();
		auto& pos = e.getPosition();
		auto size = e.getSize();
		bool spike = e.getField<ldtk::FieldType::Bool>("Spike").value_or(false);

		float offset = spike ? 5.0f : 0.0f;

		//Wrap AABB around the colliders 
		Collider collider
		{
			.aabb = AABB{ {pos.x + offset, pos.y + offset, 0.0f }, {pos.x + size.x - offset, pos.y + size.y - offset, 0.0f } },
			.Spike = spike

		};
		colliders.push_back(collider);
	}


	// Collected animation effect
	{
		//auto spriteSheet = ResourceManager::loadSpriteSheet(projectPath / "Treasure Hunters/Items/Coin Effect/1.png", 16, 16, 0, 0, BlendMode::AlphaBlend);
		//pickupCollected = SpriteAnim{ spriteSheet, 20 };
	}

	// Loads coin sprites
	const auto& tilesets = project.allTilesets();
	for (auto& tileset : tilesets)
	{
		if (tileset.name == "Coin")
		{
			auto sprites = ResourceManager::loadSpriteSheet(projectPath / tileset.path, tileset.tile_size, tileset.tile_size, static_cast<uint32_t>(tileset.padding), static_cast<uint32_t>(tileset.spacing), BlendMode::AlphaBlend);
			coinSprites[tileset.name] = std::move(sprites);
		}
	}

	// Parse the level tile map
	{
		const auto& TerrainLayer = level.getLayer("Terrain");
		const auto& intGrid = level.getLayer("Terrain");

		const auto& gridSize = TerrainLayer.getGridSize();
		const auto& tileSet = intGrid.getTileset();

		auto spriteSheet = ResourceManager::loadSpriteSheet(projectPath / tileSet.path, tileSet.tile_size, tileSet.tile_size, tileSet.padding, tileSet.spacing, BlendMode::AlphaBlend);
		tileMap = TileMap(spriteSheet, gridSize.x, gridSize.y);

		for (auto& tile : intGrid.allTiles())
		{
			const auto& gridPos = tile.getGridPosition();
			tileMap(gridPos.y, gridPos.x) = tile.tileId;
		}

		for (auto& tile : TerrainLayer.allTiles())
		{
			const auto& gridPos = tile.getGridPosition();
			tileMap(gridPos.y, gridPos.x) = tile.tileId;
		}
	}

	//Parse background 
	{
		const auto& BG_Layer = level.getLayer("Background");
		const auto& BG_Grid = level.getLayer("Background");
		const auto& tileSet = BG_Grid.getTileset();

		const auto& gridSize = BG_Layer.getGridSize();

		auto spriteSheet = ResourceManager::loadSpriteSheet(projectPath / tileSet.path, tileSet.tile_size, tileSet.tile_size, tileSet.padding, tileSet.spacing, BlendMode::AlphaBlend);
		bg_Map = TileMap(spriteSheet, gridSize.x, gridSize.y);


		for (auto& tile : BG_Grid.allTiles())
		{
			const auto& gridPos = tile.getGridPosition();
			bg_Map(gridPos.y, gridPos.x) = tile.tileId;
		}

		for (const auto& tile : BG_Layer.allTiles())
		{
			const auto& gridPos = tile.getGridPosition();
			bg_Map(gridPos.y, gridPos.x) = tile.tileId;
		}
	}

	// Parse spikes
	{
		const auto& spikeLayer = level.getLayer("Spikes");
		const auto& spikeGrid = level.getLayer("Spikes");
		const auto& tileSet = spikeGrid.getTileset();

		const auto& gridSize = spikeLayer.getGridSize();

		auto spriteSheet = ResourceManager::loadSpriteSheet(projectPath / tileSet.path, tileSet.tile_size, tileSet.tile_size, tileSet.padding, tileSet.spacing, BlendMode::AlphaBlend);
		spikeMap = TileMap(spriteSheet, gridSize.x, gridSize.y);

		for (auto& tile : spikeGrid.allTiles())
		{
			const auto& gridPos = tile.getGridPosition();
			spikeMap(gridPos.y, gridPos.x) = tile.tileId;
		}

		for (const auto& tile : spikeLayer.allTiles())
		{
			const auto& gridPos = tile.getGridPosition();
			spikeMap(gridPos.y, gridPos.x) = tile.tileId;
		}
	}

	parseCollectables();

	//Player start position
	const auto& startPos = entities.getEntitiesByName("Start")[0].get();
	playerStart = { startPos.getPosition().x, startPos.getPosition().y };
	player = Player{ playerStart };
}

void Level::update(float deltaTime)
{
	updateCollisions(deltaTime);
	updatePickups(deltaTime);
	player.update(deltaTime);
}

void Level::parseCollectables()
{
	//Parse collectables 
	const auto& entities = level->getLayer("Entities");
	const auto& pickups = entities.getEntitiesByName("Pickup");
	for (auto& pickup : pickups)
	{
		auto& e = pickup.get();
		auto& p = e.getPosition();
		auto& s = e.getSize();
		auto& gridP = e.getGridPosition();

		auto& coinSprite = coinSprites["Coin"];
		Sphere collider{ { p.x, p.y, 0 }, 4.5f };

		allPickups.emplace_back(coinSprite, collider);
	}
}

void Level::reset()
{
	player = Player{ playerStart };
	isDead = false;
	score = 0;
	allPickups.clear();
	parseCollectables();

	// Reset boolean
	doReset = true;
}

void Level::draw(Graphics::Image& image)
{
	if (Finished == true)
	{
		bg_Map.clear();
		tileMap.clear();
		spikeMap.clear();
	}
	//! Mind that images are drawn over eachother on top to down order
	bg_Map.draw(image);
	tileMap.draw(image);
	spikeMap.draw(image);

	for (auto& pickup : allPickups)
	{
		pickup.draw(image);

		if (player.getState() == Player::State::Dead)
		{
			if (Input::getKey("enter"))
			{
				pickup.draw(image);
			}
		}
	}

	if (isDead == true)
	{
		image.drawText(Font::Default, "You died", 300, 300, Color::Red);
	}

	/* for (auto& effect : effects)
	 {
		 effect.draw(image);
	 }*/
	player.draw(image);

	//Draw score on screen
	image.drawText(Font::Default, fmt::format("Gold: {:0} /46", score), 700, 30, Color::Yellow);
	//image.drawText(Font::Default, statemap[state], 10, 50, Color::Cyan);

#if _DEBUG
	for (const auto& collider : colliders)
	{
		image.drawAABB(collider.aabb, Color::Red, BlendMode::Disable, FillMode::WireFrame);
	}
	//image.drawText(Font::Default, statemap[state], 670, 70, Color::Cyan);
	image.drawText(Font::Default, check, 650, 70, Color::Green);
#endif
}

void Level::updateCollisions(float deltaTime)
{
	//Store the previous position
	glm::vec2 prevPos = player.getPosition();

	if (player.getState() == Player::State::Dead)
	{
		if (Input::getKey("enter"))
		{
			reset();
			score = 0;
		}
		return;
	}

	//Check player collision
	//Get player AABB
	AABB playerAABB = player.getAABB();

	glm::vec2 vel = player.getVelocity();
	glm::vec2 pos = player.getPosition();

	// Number of pixels padding to account for collisions
	const float padding = 3.0f;

	bool onGround = false;



	for (auto& collider : colliders)
	{
		//Collider AABB
		AABB colliderAABB = collider.aabb;

		if (playerAABB.intersect(colliderAABB))
		{
			if (collider.Spike)
			{
				player.setState(Player::State::Dead);
				deathSound.play();
				std::cout << "Player died" << std::endl;
				return;
			}
		}

		// Player is falling
		if (vel.y > 0.0f)
		{
			player.setState(Player::State::Falling);

			//Check if player collides with top edghe of collider 
			Line topEdge{ { colliderAABB.min.x + padding, colliderAABB.min.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.min.y, 0 } };
			if (playerAABB.intersect(topEdge))
			{
				// Set the player's position to the top of the AABB
				pos.y = colliderAABB.min.y;
				// And 0 the Y velocity
				vel.y = 0.0f;

				// Change to running state
				player.setState(Player::State::Running);

				onGround = true;
			}
		}
		// Player is jumping
		else if (vel.y < 0.0f)
		{
			// Check to see if the player is colliding with the bottom edge of the colliders
			Line bottomEdge{ { colliderAABB.min.x + padding, colliderAABB.max.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.max.y, 0 } };
			if (playerAABB.intersect(bottomEdge))
			{
				// Set the player's position to the bottom of the collider
				pos.y = colliderAABB.max.y + playerAABB.height();
				// And 0 the Y velocity
				vel.y = 0.0f;

				// And start falling
				player.setState(Player::State::Falling);
				continue;
			}
		}
		//Player is idle or running
		else
		{
			//Check to see if the player is colliding with the top edge of the collider
			Line topEdge{ { colliderAABB.min.x + padding, colliderAABB.min.y, 0 }, { colliderAABB.max.x - padding, colliderAABB.min.y, 0 } };
			if (playerAABB.intersect(topEdge))
			{
				onGround = true;
			}
		}

		if (vel.x > 0)
		{
			//Check to see if the player is colliding with the left edge of the collider
			Line leftEdge{ { colliderAABB.min.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.min.x, colliderAABB.max.y - padding, 0 } };
			if (playerAABB.intersect(leftEdge))
			{
				//Set the player's position to the left edge of the collider
				pos.x = colliderAABB.min.x - playerAABB.width() * 0.5f;
				//And 0 the X velocity
				vel.x = 0.0f;

				continue;
			}
		}

		else if (vel.x < 0)
		{
			//Check to see if the player is colliding with the right edge of the collider
			Line rightEdge{ { colliderAABB.max.x, colliderAABB.min.y + padding, 0 }, { colliderAABB.max.x, colliderAABB.max.y - padding, 0 } };
			if (playerAABB.intersect(rightEdge))
			{
				//Set the player's position to the right edge of the collider
				pos.x = colliderAABB.max.x + playerAABB.width() * 0.5f;
				//and 0 the X velocity
				vel.x = 0.0f;

				continue;
			}
		}
	}

	if (player.getState() == Player::State::Running && !onGround)
	{
		player.setState(Player::State::Falling);
	}

	player.setPosition(pos);
	player.setVelocity(vel);
}

void Level::updatePickups(float deltaTime)
{
	//Get player AABB
	AABB playerAABB = player.getAABB();

	// Collect sound effect
	std::string sound_filePath = "assets\\sounds\\bit-bounce.wav";
	if (sound_filePath; NULL != 0)
	{
		std::cerr << "Failed to open MP3 file" << std::endl;
		{int i = 3; };
	}


	//Check if player collides with the pickup
	for (auto pickups = allPickups.begin(); pickups != allPickups.end();)
	{
		if (pickups->collides(player))
		{
			//Removes object from screen
			pickups = allPickups.erase(pickups);
			collectSound.replay();
			//Ads point to the score on collision 
			++score;
			//Keep track of the score
			std::cout << "Score: " << score << std::endl;
			return;
		}
		else ++pickups;
	}

	if (allPickups.empty())
	{
		Finished = true;
	}

}

void Level::clearLevel(Graphics::Image& image)
{
	bg_Map.clear();
	tileMap.clear();
	spikeMap.clear();
}

