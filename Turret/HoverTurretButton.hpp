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
            float img_x, float img_y,
            float inf_x, float inf_y, float inf_w, float inf_h,
            unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
            int cost, int range, int damage);
	void Update(float deltaTime) override;
	void Draw() const override;
};
#endif // TURRETBUTTON_HPP
