#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/IScene.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:
	Engine::IScene *sceneType;
 	std::vector<Engine::Point> path;
	float speed;
	float hp;
	float froze_count_down;
	// in normal mode, the monye you get if you kill the enemy
	int money;
	// in normal mode, the score you get if you kill the enemy
	int kill_score;

	PlayScene* getPlayScene();
	virtual void OnExplode();
public:
	// static const int cost = 0;
	float reachEndTime;
	std::list<Turret*> lockedTurrets;
	std::list<Bullet*> lockedBullets;
	Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money);
 	void Hit(float damage);
	void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	void Update(float deltaTime) override;
	void Draw() const override;
	int get_kill_score() const;
	void set_froze_timer(float duration);
};
#endif // ENEMY_HPP
