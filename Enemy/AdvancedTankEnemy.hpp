#ifndef ADVANCEDTANKENEMY_HPP
#define ADVANCEDTANKENEMY_HPP
#include "Enemy.hpp"

class AdvancedTankEnemy : public Enemy {
public:
	static const int cost = 20;
	AdvancedTankEnemy(int x, int y);
	void Update(float deltaTime) override;
};
#endif 