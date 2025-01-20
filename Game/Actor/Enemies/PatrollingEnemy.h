#pragma once

#include "Enemy.h"

class PatrollingEnemy : public Enemy
{
    RTTI_DECLARATIONS(PatrollingEnemy, Enemy)

protected:
    virtual void DecideDirection() override;

};