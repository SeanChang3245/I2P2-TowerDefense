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
#include "UI/Component/EnemyButton.hpp"
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

// bool ReversePlayScene::DebugMode = false;
// const std::vector<Engine::Point> ReversePlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
// const int ReversePlayScene::MapWidth = 20, ReversePlayScene::MapHeight = 13;
// const int ReversePlayScene::BlockSize = 64;
// const Engine::Point ReversePlayScene::SpawnGridPoint = Engine::Point(-1, 0);
// const Engine::Point ReversePlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);

// // code to activate cheat mode
// const std::vector<int> ReversePlayScene::code = {ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
// 										  ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT,
// 										  ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER};

const float ReversePlayScene::PlaceTurretDuration = 10.0;

using Engine::LOG;
using Engine::INFO;

void ReversePlayScene::Initialize()
{
	Engine::LOG(Engine::INFO) << "enter ReversePlayScene::initialize\n";
	PlayScene::Initialize();

	remain_time = 180 / difficulty;
	placeTurretCountDown = PlaceTurretDuration;
	playing_danger_bgm = false;
	cur_turret = nullptr;
	turret_pos.x = turret_pos.y = -1;
}

void ReversePlayScene::Update(float deltaTime)
{
	UpdateDangerIndicator();
	for (int i = 0; i < SpeedMult; i++)
	{
		IScene::Update(deltaTime);		
		UpdateTimer(deltaTime);
		UpdatePlaceTurret(deltaTime);
	
		// Check if run out of time, if so then lose
		if(remain_time < 0)
		{
			remain_time = 0;
			Engine::GameEngine::GetInstance().ChangeScene("lose");
			return;
		}
	}	
}

void ReversePlayScene::OnMouseDown(int button, int mx, int my)
{
	IScene::OnMouseDown(button, mx, my);
}

void ReversePlayScene::OnMouseMove(int mx, int my)
{
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
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
	OnMouseMove(mx, my);
}
void ReversePlayScene::OnKeyDown(int keyCode)
{
	PlayScene::OnKeyDown(keyCode);
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
}

void ReversePlayScene::ConstructUI()
{
	PlayScene::ConstructUI();

	// Text
	UIGroup->AddNewObject(UITime = new Engine::Label(std::string("time") + std::to_string(remain_time), "pirulen.ttf", 24, 1294, 168));

	Engine::EnemyButton *btn;
	const int information_x = 1294;
	const int information_y = 400;

	// Soldier Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-1.png",
		1294, 216,
		information_x, information_y,
		64, 64, 
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 0));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(SoldierEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(SoldierEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(SoldierEnemy::Speed)));
	btn->SetCostValue(SoldierEnemy::Cost);
	UIGroup->AddNewControlObject(btn);
	

	// Plane Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-2.png",
		1370, 216,
		information_x, information_y,
		64, 64,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 1));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(PlaneEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(PlaneEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(PlaneEnemy::Speed)));
	btn->SetCostValue(PlaneEnemy::Cost);
	UIGroup->AddNewControlObject(btn);

	// Tank Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-3-full.png",
		1446, 216,
		information_x, information_y,
		64, 64,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 2));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(TankEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(TankEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(TankEnemy::Speed)));
	btn->SetCostValue(TankEnemy::Cost);
	UIGroup->AddNewControlObject(btn);

	// Advanced Tank Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-4.png",
		1522, 216,
		information_x, information_y,
		64, 64,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&ReversePlayScene::UIBtnClicked, this, 3));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(AdvancedTankEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(AdvancedTankEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(AdvancedTankEnemy::Speed)));
	btn->SetCostValue(AdvancedTankEnemy::Cost);
	UIGroup->AddNewControlObject(btn);


	// Background
	// UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

	// // Text
	// UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	// UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
	// UIGroup->AddNewObject(UIScore = new Engine::Label(std::string("Score: ") + std::to_string(total_score), "pirulen.ttf", 24, 1294, 88));
	// UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 128));
	
	// // Exit button
	// Engine::ImageButton *btn2;
	// btn2 = new Engine::ImageButton("play/dirt.png", "play/floor.png", 1310, 750, 260, 75);
	// btn2->SetOnClickCallback(std::bind(&ReversePlayScene::ExitOnClick, this));
	// UIGroup->AddNewControlObject(btn2);
	// UIGroup->AddNewObject(new Engine::Label("exit", "pirulen.ttf", 32, 1440, 750 + 75.0/2, 0, 0, 0, 255, 0.5, 0.5));
}

void ReversePlayScene::UIBtnClicked(int id)
{
    const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
	Enemy* enemy = nullptr;
    
    int cost = 0;
    if (id == 0 && money >= SoldierEnemy::Cost)
    {
		EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = SoldierEnemy::Cost;
    }
	else if (id == 1 && money >= PlaneEnemy::Cost)
    {
        EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = PlaneEnemy::Cost;
    }
	else if (id == 2 && money >= TankEnemy::Cost)
    {
        EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = TankEnemy::Cost;
    }
	else if (id == 3 && money >= AdvancedTankEnemy::Cost)
    {
        EnemyGroup->AddNewObject(enemy = new AdvancedTankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = AdvancedTankEnemy::Cost;
    }

    if(!enemy)
        return;
    EarnMoney(-cost);

    enemy->UpdatePath(mapDistance);
    enemy->Update(ticks);

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


void ReversePlayScene::DeconstructTurret(const int &x, const int &y)
{
	// if (!preview || preview->GetType() != TOOL)
	// 	return;
	// // Check if turret can be place at (x,y)
	// Engine::Point cur_mouse_position(x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2);

	// Turret *target_turret = nullptr;
	// for(auto &it : this->TowerGroup->GetObjects())
	// {
	// 	Turret* turret = dynamic_cast<Turret*>(it);
	// 	if(!turret->Visible)
	// 		continue;
	// 	if(cur_mouse_position == turret->Position)
	// 	{
	// 		target_turret = turret;
	// 		break;
	// 	}
	// }
	
	// if(target_turret == nullptr)
	// 	return;

	// // Get money back.
	// EarnMoney(target_turret->GetPrice() / 2);

	// // Remove Preview.
	// preview->GetObjectIterator()->first = false;
	// UIGroup->RemoveObject(preview->GetObjectIterator());

	// // Delete target turret.
	// TowerGroup->RemoveObject(target_turret->GetObjectIterator());

	// // To keep responding when paused.
	// preview->Update(0);
	
	// // Remove Preview.
	// preview = nullptr;

	// // Give Back Space
	// mapState[y][x] = originalMapState[y][x];
}

void ReversePlayScene::Hit()
{
	PlayScene::Hit();
	if (lives <= 0)
	{
		total_score += remain_time * 1000;
		Engine::GameEngine::GetInstance().ChangeScene("win");
	}
}

void ReversePlayScene::UpdateDangerIndicator()
{
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	if(remain_time <= DangerTime)
	{
		float pos = DangerTime - remain_time;
	
		if (SpeedMult == 0)
			deathCountDown = -1;
		else 
			SpeedMult = 1;
	
		if(SpeedMult == 0)
		{
			AudioHelper::StopSample(deathBGMInstance);
			playing_danger_bgm = false;
		}
		else if(!playing_danger_bgm)
		{
			deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
			playing_danger_bgm = true;
		}
		float alpha = pos / DangerTime;
		alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
		dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
	}
	
}

#include <iostream>
using namespace std;
void ReversePlayScene::UpdatePlaceTurret(float deltaTime)
{
	placeTurretCountDown -= deltaTime;

	if(placeTurretCountDown > 0)
		return; 
	
	ChooseTurretPosition();
	ChooseTurretType();
	PlaceTurret(turret_pos.x, turret_pos.y);

	placeTurretCountDown = PlaceTurretDuration;

	cout << turret_pos.x << ' ' << turret_pos.y << '\n';
}

void ReversePlayScene::ChooseTurretPosition()
{
	int x, y;
	srand(time(NULL));

	int mostTry = 10;
	while(mostTry--)
	{
		x = rand();
		y = rand();
		x %= MapWidth;
		y %= MapHeight;
		if(CheckSpaceValid(x, y))
		{
			turret_pos.x = x;
			turret_pos.y = y;
			return;
		}
	}
	turret_pos.x = turret_pos.y = -1;
	LOG(INFO) << "miss one turret";
}

void ReversePlayScene::ChooseTurretType()
{
	srand(time(NULL));
	int x = rand();
	switch (x % 4)
	{
	case 0:
		cur_turret = new MachineGunTurret(0, 0);
		break;
	case 1:
		cur_turret = new LaserTurret(0, 0);
		break;
	case 2:
		cur_turret = new MissileTurret(0, 0);
		break;
	case 3:
		cur_turret = new AdvancedMissileTurret(0, 0);
		break;
	default:
		break;
	}
}

void ReversePlayScene::PlaceTurret(const int &x, const int &y)
{
	if (!cur_turret || x == -1 || y == -1)
		return;

	for(auto &it : this->TowerGroup->GetObjects())
	{
		Turret* turret = dynamic_cast<Turret*>(it);
		if(!it)
			return;
	}

	
	// Construct real turret.
	cur_turret->Position.x = x * BlockSize + BlockSize / 2;
	cur_turret->Position.y = y * BlockSize + BlockSize / 2;
	cur_turret->Enabled = true;
	cur_turret->Preview = false;
	cur_turret->Tint = al_map_rgba(255, 255, 255, 255);
	TowerGroup->AddNewObject(cur_turret);
	
	// To keep responding when paused.
	cur_turret->Update(0);
	
	cur_turret = nullptr;

	mapState[y][x] = TILE_OCCUPIED;
}


// void ReversePlayScene::Terminate()
// {
// 	AudioHelper::StopSample(deathBGMInstance);
// 	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
// 	PlayScene::Terminate();
// }