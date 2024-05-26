#include <string>

#include "AdvancedTankEnemy.hpp"

AdvancedTankEnemy::AdvancedTankEnemy(int x, int y) : 
    Enemy("play/enemy-4.png", x, y, 20, 15, 200, 75) 
{
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
	kill_score = 300;
}

void AdvancedTankEnemy::Update(float deltaTime)
{
	Enemy::Update(deltaTime);
	speed = 15 + 30 * (200-hp) / 200;
}