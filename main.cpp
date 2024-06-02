// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/DifficultySelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "Scene/MapCreateScene.hpp"
#include "DebugMacro.hpp"
#include <filesystem>

using namespace std::filesystem;

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true, PRINT_LOG_VERBOSE);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    // TODO: [HACKATHON-1-SCENE] (3/4): Register Scenes here
	game.AddNewScene("start", new StartScene());
	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("difficulty-select", new DifficultySelectScene());
	game.AddNewScene("map-create", new MapCreateScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("scoreboard", new ScoreboardScene());

    // TODO: [HACKATHON-1-SCENE] (4/4): Change the start scene
	

	game.Start("start", 60, 1600, 832);
	return 0;
}

