#ifndef HOVERTURRETBUTTON_HPP
#define HOVERTURRETBUTTON_HPP
#include <string>

#include "UI/Component/HoverImageButton.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class HoverTurretButton : public Engine::HoverImageButton
{
protected:
    PlayScene* getPlayScene();
public:
    int money;
    Engine::Sprite Base;
    Engine::Sprite Turret;

    HoverTurretButton(std::string img, std::string imgIn, Engine::Sprite Base, Engine::Sprite Turret, 
            int img_x, int img_y,
            int inf_x, int inf_y,
            unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
            int cost, int range, int damage, float reload);
	void Update(float deltaTime) override;
	void Draw() const override;
};
#endif // TURRETBUTTON_HPP
