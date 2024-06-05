#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "ModeSelectScene.hpp"
#include "Engine/LOG.hpp"

void ModeSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, NORMAL));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("easy", "pirulen.ttf", 48, halfW, halfH / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH /2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, SURVIVAL));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("normal", "pirulen.ttf", 48, halfW, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH /2 + 250, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, REVERSE));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("hard", "pirulen.ttf", 48, halfW, halfH / 2 + 300, 0, 0, 0, 255, 0.5, 0.5));
}

void ModeSelectScene::Terminate() {
    Engine::LOG(Engine::INFO) << "terminate mode select scene";
	IScene::Terminate();
}

void ModeSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void ModeSelectScene::PlayOnClick(PlayMode mode) {
    // PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    // Engine::GameEngine::GetInstance().ChangeScene("play");


}