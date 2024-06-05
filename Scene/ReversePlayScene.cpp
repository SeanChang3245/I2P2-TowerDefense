#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/HoverImageButton.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/AdvancedMissileTurret.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "ReversePlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/AdvancedTankEnemy.hpp"
#include "Turret/TurretButton.hpp"
#include "Engine/LOG.hpp"
#include "Turret/HoverTurretButton.hpp"
#include "Turret/Shovel.hpp"
#include "DebugMacro.hpp"
#include "Bullet/FireBullet.hpp"
#include "UI/Component/ImageButton.hpp"

bool ReversePlayScene::DebugMode = false;
const std::vector<Engine::Point> ReversePlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
const int ReversePlayScene::MapWidth = 20, ReversePlayScene::MapHeight = 13;
const int ReversePlayScene::BlockSize = 64;
const float ReversePlayScene::DangerTime = 7.61;
const Engine::Point ReversePlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point ReversePlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);

// code to activate cheat mode
const std::vector<int> ReversePlayScene::code = {ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
										  ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT,
										  ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER};

using Engine::LOG;
using Engine::INFO;

Engine::Point ReversePlayScene::GetClientSize()
{
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}

void ReversePlayScene::Initialize()
{
	Engine::LOG(Engine::INFO) << "enter play scene::initialize\n";

    MapId = 1;

	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	lives = 10;
	money = 150;
	total_score = 0;
	SpeedMult = 1;
	remain_time = 90;

	// a scene is a Group, so there is a vector of object in the scene
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(TowerGroup = new Group());
	AddNewObject(EnemyGroup = new Group());
	AddNewObject(BulletGroup = new Group());
	AddNewObject(EffectGroup = new Group());

	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	mapDistance = CalculateBFSDistance();
	ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	preview = nullptr;
	UIGroup->AddNewObject(imgTarget);

	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");

	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
}
void ReversePlayScene::Terminate()
{
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void ReversePlayScene::Update(float deltaTime)
{
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	// if (SpeedMult == 0)
	// 	deathCountDown = -1;
	// else if (deathCountDown != -1)
	// 	SpeedMult = 1;
	// // Calculate danger zone.
	// std::vector<float> reachEndTimes;
	// for (auto &it : EnemyGroup->GetObjects())
	// {
	// 	reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
	// }
	// // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	// std::sort(reachEndTimes.begin(), reachEndTimes.end());
	// float newDeathCountDown = -1;
	// int danger = lives;
	// for (auto &it : reachEndTimes)
	// {
	// 	if (it <= DangerTime)
	// 	{
	// 		danger--;
	// 		if (danger <= 0)
	// 		{
	// 			// Death Countdown
	// 			float pos = DangerTime - it;
	// 			if (it > deathCountDown)
	// 			{
	// 				// Restart Death Count Down BGM.
	// 				AudioHelper::StopSample(deathBGMInstance);
	// 				if (SpeedMult != 0)
	// 					deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
	// 			}
	// 			float alpha = pos / DangerTime;
	// 			alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
	// 			dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
	// 			newDeathCountDown = it;
	// 			break;
	// 		}
	// 	}
	// }
	// deathCountDown = newDeathCountDown;
	// if (SpeedMult == 0)
	// 	AudioHelper::StopSample(deathBGMInstance);
	// if (deathCountDown == -1 && lives > 0)
	// {
	// 	AudioHelper::StopSample(deathBGMInstance);
	// 	dangerIndicator->Tint.a = 0;
	// }
	// if (SpeedMult == 0)
	// 	deathCountDown = -1;
    
	for (int i = 0; i < SpeedMult; i++)
	{
		IScene::Update(deltaTime);		
		UpdateTimer(deltaTime);
	}

}

void ReversePlayScene::Draw() const
{
	IScene::Draw();
	if (DebugMode)
	{
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++)
		{
			for (int j = 0; j < MapWidth; j++)
			{
				if (mapDistance[i][j] != -1)
				{
					// Not elegant nor efficient, but it's quite enough for debugging.
					Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw();
				}
			}
		}
	}
}
void ReversePlayScene::OnMouseDown(int button, int mx, int my)
{
	if ((button == 1) && !imgTarget->Visible && preview)
	{
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}
void ReversePlayScene::OnMouseMove(int mx, int my)
{
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
	{
		imgTarget->Visible = false;
		return;
	}
	imgTarget->Visible = true;
	imgTarget->Position.x = x * BlockSize;
	imgTarget->Position.y = y * BlockSize;
}
void ReversePlayScene::OnMouseUp(int button, int mx, int my)
{
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	// left click
	if (button == 1)
	{ 
		if (mapState[y][x] != TILE_OCCUPIED)
		{
			PlaceTurret(x, y);
		}
	}
	OnMouseMove(mx, my);
}
void ReversePlayScene::OnKeyDown(int keyCode)
{
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB)
	{
		DebugMode = !DebugMode;
	}
	else
	{
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size())
		{
			auto it = keyStrokes.begin();
			for (int c : code)
			{
				if (!((*it == c) ||
					  (c == ALLEGRO_KEYMOD_SHIFT &&
					   (*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
					return;
				++it;
			}

			// successfully enter cheat code
			EarnMoney(10000);
		}
	}
	if (keyCode == ALLEGRO_KEY_Q)
	{
		// Hotkey for Soldier.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W)
	{
		// Hotkey for Plane.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_E)
	{
		// Hotkey for Tank.
		UIBtnClicked(2);
	}
	else if (keyCode == ALLEGRO_KEY_R)
	{
		// Hotkey for AdvancedTank.
		UIBtnClicked(3);
	}
	// TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
	{
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}
void ReversePlayScene::Hit()
{
	LOG(INFO) << "enter Hit()";
	lives--;
	UILives->Text = std::string("Life ") + std::to_string(lives);
	if (lives <= 0)
	{
		Engine::GameEngine::GetInstance().ChangeScene("lose");
	}
	LOG(INFO) << "leave Hit()";
}

int ReversePlayScene::GetScore() const
{
	return total_score;
}

void ReversePlayScene::EarnMoney(int money)
{
	this->money += money;
	UIMoney->Text = std::string("$") + std::to_string(this->money);
}

void ReversePlayScene::EarnScore(int score)
{
	this->total_score += score;
	UIScore->Text = std::string("Score: ") + std::to_string(this->total_score);
}

void ReversePlayScene::ReadMap()
{
	std::string filename = std::string("Resource/maps/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<bool> mapData;
	std::ifstream fin(filename);
	while (fin >> c)
	{
		switch (c)
		{
		case '0':
			mapData.push_back(false);
			break;
		case '1':
			mapData.push_back(true);
			break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted1. map: " + filename);
			break;
		default:
			throw std::ios_base::failure("Map data is corrupted2. map: " + filename);
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted3. map: " + filename);
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	originalMapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++)
	{
		for (int j = 0; j < MapWidth; j++)
		{
			const bool num = mapData[i * MapWidth + j];
			mapState[i][j] = originalMapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			if (num)
				TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			else
				TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
	}
}

void ReversePlayScene::ConstructUI()
{
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

	// Text
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
	UIGroup->AddNewObject(UIScore = new Engine::Label(std::string("Score: ") + std::to_string(total_score), "pirulen.ttf", 24, 1294, 88));
	UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 128));
	UIGroup->AddNewObject(UITime = new Engine::Label(std::string("time") + std::to_string(remain_time), "pirulen.ttf", 24, 1294, 250));

	Engine::HoverImageButton *btn;
	const int information_x = 1294;
	const int information_y = 400;

	// Enemy 1
	btn = new Engine::HoverImageButton("play/floor.png", "play/dirt.png",
		1294, 176,
		information_x, information_y,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);

	// Enemy 2
	btn = new Engine::HoverImageButton("play/floor.png", "play/dirt.png",
		1370, 176,
		information_x, information_y,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);

	// Enemy 3
	btn = new Engine::HoverImageButton("play/floor.png", "play/dirt.png",
		1446, 176,
		information_x, information_y,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn);

	// Enemy 4
	btn = new Engine::HoverImageButton("play/floor.png", "play/dirt.png",
		1522, 176,
		information_x, information_y,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 3));
	UIGroup->AddNewControlObject(btn);


	// Exit button
	Engine::ImageButton *btn2;
	btn2 = new Engine::ImageButton("play/dirt.png", "play/floor.png", 1310, 750, 260, 75);
	btn2->SetOnClickCallback(std::bind(&ReversePlayScene::ExitOnClick, this));
	UIGroup->AddNewControlObject(btn2);
	UIGroup->AddNewObject(new Engine::Label("exit", "pirulen.ttf", 32, 1440, 750 + 75.0/2, 0, 0, 0, 255, 0.5, 0.5));

	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift - 100);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);
}

void ReversePlayScene::UIBtnClicked(int id)
{
    const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
	Enemy* enemy = nullptr;
    
    int cost = 0;
    if (id == 0 && money >= SoldierEnemy::cost)
    {
		EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = SoldierEnemy::cost;
    }
	else if (id == 1 && money >= PlaneEnemy::cost)
    {
        EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = PlaneEnemy::cost;
    }
	else if (id == 2 && money >= TankEnemy::cost)
    {
        EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = TankEnemy::cost;
    }
	else if (id == 3 && money >= AdvancedTankEnemy::cost)
    {
        EnemyGroup->AddNewObject(enemy = new AdvancedTankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = AdvancedTankEnemy::cost;
    }

    if(!enemy)
        return;
    EarnMoney(-cost);

    enemy->UpdatePath(mapDistance);
    enemy->Update(ticks);

}

bool ReversePlayScene::CheckSpaceValid(int x, int y)
{
	// a turret can be placed on TILE_DIRT or TILE_FLOOR

	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return false;
	TileType map00 = mapState[y][x];

	// assume a turret has been placed, will it blocks enemy's path
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = map00;
	if (map[0][0] == -1) // the path from start point to end point is blocked
		return false;
	for (auto &it : EnemyGroup->GetObjects())
	{
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0)
			pnt.x = 0;
		if (pnt.x >= MapWidth)
			pnt.x = MapWidth - 1;
		if (pnt.y < 0)
			pnt.y = 0;
		if (pnt.y >= MapHeight)
			pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1) // the turrect is placed on a enemy
			return false;
	}
	// All enemy have path to exit, hence the turrect can be placed at (x,y)
	mapState[y][x] = TILE_OCCUPIED;
	mapDistance = map;
	for (auto &it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
	return true;
}

std::vector<std::vector<int>> ReversePlayScene::CalculateBFSDistance()
{
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
		return map;
	que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
	map[MapHeight - 1][MapWidth - 1] = 0;

	int cur_dist = 0;
	while (!que.empty())
	{
		// TODO: [BFS PathFinding] (1/1): Implement a BFS starting from the most right-bottom block in the map.
		//               For each step you should assign the corresponding distance to the most right-bottom block.
		//               mapState[y][x] is TILE_DIRT if it is empty.
		int s = que.size();
		while (s--)
		{
			Engine::Point p = que.front();
			que.pop();

			for (const auto &dir : directions)
			{
				int nextX = (dir + p).x;
				int nextY = (dir + p).y;

				if (nextX < 0 || nextX >= MapWidth ||
					nextY < 0 || nextY >= MapHeight ||
					mapState[nextY][nextX] == TILE_FLOOR ||
					mapState[nextY][nextX] == TILE_OCCUPIED ||
					map[nextY][nextX] != -1) // the tile is floor but its shortest distance has been updated
					continue;

				map[nextY][nextX] = cur_dist + 1;
				que.emplace(Engine::Point(nextX, nextY));
			}
		}
		++cur_dist;
	}

	if (PRINT_MAP_DISTANCE)
	{
		Engine::LOG(Engine::INFO) << "map distance information";
		for (const auto &v : map)
		{
			for (const auto &dist : v)
				std::cout << dist << ' ';
			std::cout << '\n';
		}
	}

	return map;
}

void ReversePlayScene::PlaceTurret(const int &x, const int &y)
{
	if (!preview || preview->GetType() != TURRET)
		return;
	// Check if turret can be place at (x,y)
	if (!CheckSpaceValid(x, y))
	{
		Engine::Sprite *sprite;
		GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
		sprite->Rotation = 0;
		return;
	}

	for(auto &it : this->TowerGroup->GetObjects())
	{
		Turret* turret = dynamic_cast<Turret*>(it);
		if(!it)
			return;
	}

	// Purchase.
	EarnMoney(-preview->GetPrice());
	
	// Remove Preview.
	preview->GetObjectIterator()->first = false;
	UIGroup->RemoveObject(preview->GetObjectIterator());
	
	// Construct real turret.
	preview->Position.x = x * BlockSize + BlockSize / 2;
	preview->Position.y = y * BlockSize + BlockSize / 2;
	preview->Enabled = true;
	preview->Preview = false;
	preview->Tint = al_map_rgba(255, 255, 255, 255);
	TowerGroup->AddNewObject(preview);
	
	// To keep responding when paused.
	preview->Update(0);
	
	// Remove Preview.
	preview = nullptr;

	mapState[y][x] = TILE_OCCUPIED;
	// OnMouseMove(mx, my);
}



void ReversePlayScene::ExitOnClick()
{
	Engine::GameEngine::GetInstance().ChangeScene("lose");
}

void ReversePlayScene::UpdateTimer(float deltaTime)
{
	remain_time -= deltaTime;


	int I = remain_time;
	float D = remain_time-I;
	std::string minute = std::to_string(I/60);
	std::string second = std::to_string(I%60);
	std::string decimal = std::to_string(D).substr(2, 2);

	UITime->Text = minute + ":" + second + "." + decimal;
}