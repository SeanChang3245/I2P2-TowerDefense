#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Bullet/Bullet.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/ReversePlayScene.hpp"
#include "Turret/Turret.hpp"

using Engine::LOG;
using Engine::INFO;

PlayScene* Enemy::getPlayScene() {
	PlayScene *playscene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
	return playscene;
}

void Enemy::OnExplode() {



	LOG(INFO) << "onexplode 1";
	if(getPlayScene() == nullptr)
		LOG(INFO) << "play scene nullptr";
	if(getPlayScene()->EffectGroup == nullptr)
		LOG(INFO) << "effect group nullptr";

	getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
	LOG(INFO) << "onexplode 2";
	std::random_device dev;
	std::mt19937 rng(dev());
	LOG(INFO) << "onexplode 3";
	std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
	LOG(INFO) << "onexplode 4";
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
	LOG(INFO) << "onexplode 5";
	for (int i = 0; i < 10; i++) {
		// Random add 10 dirty effects.
		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
		LOG(INFO) << "onexplode 6";
	}
}

Enemy::Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money) :
	Engine::Sprite(img, x, y), speed(speed), hp(hp), money(money) {
	CollisionRadius = radius;
	reachEndTime = 0;
	froze_count_down = 0;
}

void Enemy::Hit(float damage) {
	LOG(INFO) << "enter enemy hit";
	hp -= damage;
	if (hp <= 0) {
		LOG(INFO) << "before OnExplode";
		OnExplode();
		LOG(INFO) << "after OnExplode";
		// Remove all turret's reference to target.
		for (auto& it: lockedTurrets)
			it->Target = nullptr;
		LOG(INFO) << "after unlock turret";
		for (auto& it: lockedBullets)
			it->Target = nullptr;
		LOG(INFO) << "after unlock bullet";
		getPlayScene()->EarnScore(kill_score);
		getPlayScene()->EarnMoney(money);
		getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
		AudioHelper::PlayAudio("explosion.wav");
	}
}

void Enemy::UpdatePath(const std::vector<std::vector<int>>& mapDistance) {
	int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
	int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));

	if (x < 0) x = 0;
	if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
	if (y < 0) y = 0;
	if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
	
	Engine::Point pos(x, y);
	int num = mapDistance[y][x];
	if (num == -1) {
		num = 0;
		Engine::LOG(Engine::ERROR) << "Enemy path finding error";
	}
	path = std::vector<Engine::Point>(num + 1);
	while (num != 0) {
		std::vector<Engine::Point> nextHops;
		for (auto& dir : PlayScene::directions) {
			int x = pos.x + dir.x;
			int y = pos.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
				continue;
			nextHops.emplace_back(x, y);
		}
		// There might be multiple shortest path to the end point
		// Choose arbitrary one.
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
		pos = nextHops[dist(rng)];
		path[num] = pos;
		num--;
	}
	path[0] = PlayScene::EndGridPoint;
}
void Enemy::Update(float deltaTime) {
	// Pre-calculate the velocity.
	LOG(INFO) << 1;

	float remainSpeed = speed * deltaTime;
	froze_count_down -= deltaTime;
	if(froze_count_down > 0)
		remainSpeed = 0.001;
	else
		froze_count_down = 0;
	LOG(INFO) << 2;

	while (remainSpeed != 0) {
		LOG(INFO) << 3;
		if (path.empty()) {
			LOG(INFO) << "enter path empty";
			// Reach end point.
			Hit(hp);
			getPlayScene()->Hit();
			reachEndTime = 0;
			return;
		}
		LOG(INFO) << 4;
		Engine::Point target = path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
		Engine::Point vec = target - Position;
		// Add up the distances:
		// 1. to path.back()
		// 2. path.back() to border
		// 3. All intermediate block size
		// 4. to end point
		reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
		Engine::Point normalized = vec.Normalize();
		if (remainSpeed - vec.Magnitude() > 0) {
			Position = target;
			path.pop_back();
			remainSpeed -= vec.Magnitude();
		}
		else {
			Velocity = normalized * remainSpeed / deltaTime;
			remainSpeed = 0;
		}
		LOG(INFO) << 5;
	}
	Rotation = atan2(Velocity.y, Velocity.x);
	Sprite::Update(deltaTime);
	LOG(INFO) << 6;
}

void Enemy::Draw() const {
	Sprite::Draw();
	if (PlayScene::DebugMode) {
		// Draw collision radius.
		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
	}
}

int Enemy::get_kill_score() const
{
	return this->kill_score;
}

void Enemy::set_froze_timer(float duration)
{
	this->froze_count_down = duration;
}