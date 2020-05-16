#pragma once
#include "Entity.h"
#include "Position.h"

class Quadrant;

class StarBase : public Entity {
private:
	Position m_sector;
	Quadrant& m_quadrant;
public:
	StarBase(const Position& sector, Quadrant& quadrant);
	const Position& sector() const;
	virtual void accept(SectorVisitor& visitor);
	virtual void torpedo_hit();
};

class StarbaseDestroyedException : std::runtime_error {
public:
	StarbaseDestroyedException();
};