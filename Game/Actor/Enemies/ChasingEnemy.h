#pragma once

#include "Enemy.h"

class ChasingEnemy : public Enemy
{
    RTTI_DECLARATIONS(ChasingEnemy, Enemy)

protected:
    virtual void DecideDirection() override;

};