#pragma once
#include "Entity.h"

class Empty : public Entity {
public:
    virtual bool is_empty() const;
    virtual void accept(SectorVisitor& visitor);
    virtual void torpedo_hit();
};

