#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <utility>
#include <vector>
#include "Engine/IScene.hpp"

struct ScoreboardData
{
    std::string player_name;
    int score;
    std::pair<int, int> date;
    ScoreboardData() {}
};

class ScoreboardScene final : public Engine::IScene 
{
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<ScoreboardData> record;
    void get_score_board_data();

public:
	explicit ScoreboardScene() = default;
	void Initialize() override;
	void Terminate() override;
	void BackOnClick();
    void NextOnClick();
    void PrevOnClick();
};

#endif // SCOREBOARDSCENE_HPP
