#ifndef ADVANCEDMISSILETURRET_HPP
#define ADVANCEDMISSILETURRET_HPP
#include "Turret.hpp"

class AdvancedMissileTurret: public Turret {
public:
	static const int Price;
    AdvancedMissileTurret(float x, float y);
    void CreateBullet() override;
};
#endif 