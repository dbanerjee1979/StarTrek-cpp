#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include "Energy.h"
#include "Torpedoes.h"
#include "Position.h"
#include "Time.h"
#include "Entity.h"
#include "NavigationStrategy.h"

class Random;
class Galaxy;
class Quadrant;
class Events;
class ShieldsStrategy;
struct QuadrantVisitor;
struct SectorVisitor;

enum class EnterpriseCondition { YELLOW, RED, GREEN, DOCKED };

enum class DamageType { WARP_ENGINES, SHORT_RANGE_SENSORS, LONG_RANGE_SENSORS, PHASER_CONTROL, PHOTON_TUBES, DAMAGE_CONTROL, SHIELD_CONTROL, LIBRARY_COMPUTER, First = WARP_ENGINES, Last = LIBRARY_COMPUTER };

enum class RepairStatus { COMPLETED, DAMAGED, IMPROVED };

class Enterprise : public Entity {
private:
	Random& m_rnd;
	Galaxy& m_galaxy;
	std::shared_ptr<Quadrant> m_quadrant;
	Position m_sector;
	bool m_docked;
	Energy m_energy;
	Torpedoes m_torpedos;
	Time m_time;
	Events& m_events;
	EnterpriseCondition m_cond;
	std::unordered_map<DamageType, double> m_damage;
public:
	Enterprise(Random& rnd,  Galaxy& galaxy, Events& m_events);
	std::shared_ptr<Quadrant> quadrant() const;
	const Position& sector() const;
	void enter_quadrant();
	bool shields_low() const;
	Time& time();
	void short_range_scan(SectorVisitor& visitor);
	virtual void accept(SectorVisitor& visitor);
	virtual void torpedo_hit();
	void long_range_scan(QuadrantVisitor& visitor);
	EnterpriseCondition condition() const;
	int torpedoes_remaining() const;
	int total_energy() const;
	int shields() const;
	bool has_enough_energy() const;
	void navigate(std::function<double(int, int)> ask_course, std::function<double(double, double)> ask_warp);
	void hit(int amount, const Position& from, bool& damaged);
	void damage_control(double warp);
	void raise_shields(std::function<int(int)> ask_shields, bool& changed);
	void fire_phasers(std::function<int(int)> ask_phasers);
	void fire_torpedo(std::function<double(int, int)> ask_course, std::function<void(const Position&)> track, SectorVisitor& visitor, bool& missed);
	bool computer_damaged() const;
	void repair_damage(std::function<void()> damage_report_unavailable, std::function<bool(double time)> ask_do_repairs, std::function<void(std::vector<std::pair<DamageType, double>>&)> damage_report);
	void undock();
	bool enough_time_remaining(int klingons_remaining) const;
private:
	bool is_damaged(DamageType dt) const;
};

class ShortRangeSensorsInoperableException : std::runtime_error {
public:
	ShortRangeSensorsInoperableException();
};

class LongRangeSensorsInoperableException : std::runtime_error {
public:
	LongRangeSensorsInoperableException();
};

class ShieldControlInoperableException : std::runtime_error {
public:
	ShieldControlInoperableException();
};

class DamageControlInoperableException : std::runtime_error {
public:
	DamageControlInoperableException();
};

class PhasersInoperableException : std::runtime_error {
public:
	PhasersInoperableException();
};

class TorpedoesInoperableException : std::runtime_error {
public:
	TorpedoesInoperableException();
};

class InvalidWarpFactorException : std::runtime_error {
private:
	double m_warp;
	double m_max;
	bool m_damaged;
public:
	InvalidWarpFactorException(double warp, double max, bool damaged);
	double warp() const;
	double max() const;
	bool damaged() const;
};

class InvalidShieldChangeException : std::runtime_error {
public:
	InvalidShieldChangeException();
};

class NoKlingonsException : std::runtime_error {
public:
	NoKlingonsException();
};

class NoStarbaseException : std::runtime_error {
public:
	NoStarbaseException();
};

class TorpedoesExpendedException : std::runtime_error {
public:
	TorpedoesExpendedException();
};

std::ostream& operator <<(std::ostream& os,const EnterpriseCondition&);

std::ostream& operator <<(std::ostream& os, const DamageType&);