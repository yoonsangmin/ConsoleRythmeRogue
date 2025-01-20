#pragma once

#include "Enemy.h"

class StationaryEnemy : public Enemy
{
    RTTI_DECLARATIONS(StationaryEnemy, Enemy)


protected:
    virtual void DecideDirection() override;

};