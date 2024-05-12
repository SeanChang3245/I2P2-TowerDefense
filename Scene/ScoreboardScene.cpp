#include <functional>
#include <string>
#include <fstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "ScoreboardScene.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"

void ScoreboardScene::Initialize() {
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;

	AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 48, halfW, halfH / 5, 0, 255, 0, 255, 0.5, 0.5));
	
	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 700, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::PrevOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, halfW - 500, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 300, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::NextOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Next Page", "pirulen.ttf", 48, halfW + 500, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ScoreboardScene::Terminate() {
    record.clear();
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}

void ScoreboardScene::BackOnClick() {
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void ScoreboardScene::PrevOnClick() {
    
}

void ScoreboardScene::NextOnClick() {

}

std::ifstream& operator>>(std::ifstream& fin, ScoreboardData& data)
{
    fin >> data.player_name >> data.score;
    
}

void ScoreboardScene::get_score_board_data()
{
    std::string filename = std::string("Resource/scoreboard.txt");
    std::ifstream fin(filename);

    ScoreboardData tmp;
    while(fin >> tmp)
        record.emplace_back(tmp);
}