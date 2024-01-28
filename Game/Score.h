#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct Score
{
	int score;
	std::string name;
};
class Score
{
public:
	Score() = default;

	explicit Score(const std::filesystem::path& file);

	void addScore(const Score& score);
};

