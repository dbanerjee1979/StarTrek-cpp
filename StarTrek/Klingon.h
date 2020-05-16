#pragma once
#include <memory>
#include "Entity.h"
#include "Position.h"
#include "Enterprise.h"

class Random;

class Klingon : public Entity {
	Position m_sector;
	double m_shields;
	Random& m_rnd;
	Events& m_events;
	Quadrant& m_quadrant;
public:
	Klingon(const Position& sector, Random& rnd, Events& events, Quadrant& quadrant);
	const Position& sector() const;
	void attack(std::shared_ptr<Enterprise> ent, const Position& from);
	virtual void accept(SectorVisitor& visitor);
	bool is_destroyed() const;
	void hit(int amt, const Position& from);
	virtual void torpedo_hit();
};

