#pragma once
#include "Entity.h"
#include "Position.h"

class Star : public Entity {
    Position m_sector;
public:
    Star(const Position& sector);
    const Position& sector() const;
    virtual void accept(SectorVisitor& visitor);
    virtual void torpedo_hit();
};

