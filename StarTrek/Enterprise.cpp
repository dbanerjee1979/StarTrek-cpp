#include <algorithm>
#include <functional>
#include "Enterprise.h"
#include "Galaxy.h"
#include "Events.h"
#include "Course.h"
#include "Random.h"
#include "ShieldsStrategy.h"

Enterprise::Enterprise(Random& rnd, Galaxy& galaxy, Events& events) :
	m_rnd(rnd),
	m_galaxy(galaxy),
	m_quadrant(galaxy.quadrant(rnd)),
	m_sector(rnd),
	m_docked(false),
	m_energy(events),
	m_time(rnd, galaxy.total_klingons()),
	m_events(events),
	m_cond(EnterpriseCondition::GREEN) {
	for (int dt = int(DamageType::First); dt <= int(DamageType::Last); dt++) {
		m_damage[DamageType(dt)] = 0;
	}
}

std::shared_ptr<Quadrant> Enterprise::quadrant() const {
	return m_quadrant;
}

const Position& Enterprise::sector() const {
	return m_sector;
}

void Enterprise::enter_quadrant() {
	if (m_time.is_beginning()) {
		m_events.entering_quadrant_mission_start(*m_quadrant);
	}
	else {
		m_events.entering_quadrant(*m_quadrant);
	}
	m_quadrant->enter();
}

bool Enterprise::shields_low() const {
	return m_energy.shields_low();
}

Time& Enterprise::time() {
	return m_time;
}

void Enterprise::short_range_scan(SectorVisitor& visitor) {
	m_docked = m_quadrant->is_adjacent_to_base(m_sector);
	if (m_docked) {
		m_energy.dock();
		m_torpedos.replenish();
	}
	m_cond = m_docked ? EnterpriseCondition::DOCKED :
		m_quadrant->num_klingons() > 0 ? EnterpriseCondition::RED :
		m_energy.is_low() ? EnterpriseCondition::YELLOW :
		EnterpriseCondition::GREEN;
	
	if (is_damaged(DamageType::SHORT_RANGE_SENSORS)) {
		throw ShortRangeSensorsInoperableException();
	}
	m_quadrant->visit(visitor);
}

void Enterprise::accept(SectorVisitor& visitor) {
	visitor.visit_enterprise(*this);
}

void Enterprise::torpedo_hit() {
}

void Enterprise::long_range_scan(QuadrantVisitor& visitor) {
	if (is_damaged(DamageType::LONG_RANGE_SENSORS)) {
		throw LongRangeSensorsInoperableException();
	}
	m_galaxy.long_range_scan(m_quadrant, visitor);
}

EnterpriseCondition Enterprise::condition() const {
	return m_cond;
}

int Enterprise::torpedoes_remaining() const {
	return *m_torpedos;
}

int Enterprise::total_energy() const {
	return *m_energy;
}

int Enterprise::shields() const {
	return m_energy.shields();
}

bool Enterprise::has_enough_energy() const {
	return m_energy.enough(is_damaged(DamageType::SHIELD_CONTROL));
}

void Enterprise::navigate(std::function<double(int, int)> ask_course, std::function<double(double, double)> ask_warp) {
	Course c = ask_course(1, 9);

	bool damaged = is_damaged(DamageType::WARP_ENGINES);
	double max = damaged ? 0.2 : 8;
	double w = ask_warp(0, max);
	if (w < 0 || w > max) {
		throw InvalidWarpFactorException(w, max, damaged);
	}
	if (w == 0) {
		return;
	}

	int n = m_energy.distance_for_speed(w, is_damaged(DamageType::SHIELD_CONTROL));
	m_quadrant->klingon_attack();
	damage_control(w);
	auto q = m_quadrant;
	m_quadrant->traverse(c, n, m_quadrant, m_sector);
	m_energy.maneuver(n);
	m_time.maneuver(q == m_quadrant ? w : 1);
	if (q != m_quadrant) {
		enter_quadrant();
	}
}

void Enterprise::hit(int hit, const Position& from, bool& damaged) {
	if (m_docked) {
		m_events.starbase_shields_absorbed_hit();
		damaged = false;
		return;
	}
	m_events.enterprise_hit_from_sector(hit, from);
	m_energy.absorb_hit(hit);
	int s = m_energy.shields();
	m_events.enterprise_shields_down_to(s);
	if (hit >= 20 && m_rnd.next_int(100) <= 60 && double(hit) / s > 0.02) {
		DamageType dt = DamageType(int(DamageType::First) + m_rnd.next_int(int(DamageType::Last) - int(DamageType::First)));
		m_damage[dt] -= double(hit) / s + 0.5 * m_rnd.next_double(1);
		m_events.enterprise_damaged_by_hit(dt);
	}
	damaged = true;
}

void Enterprise::damage_control(double warp) {
	std::vector<std::pair<DamageType, RepairStatus>> m_status;
	double d = 0;
	double amt = std::min(warp, 1.0);
	for (int i = int(DamageType::First); i <= int(DamageType::Last); i++) {
		DamageType dt = DamageType(i);
		if (is_damaged(dt)) {
			double d = (m_damage[dt] += amt);
			if (d < 0) {
				m_damage[dt] = std::min(d, -0.1);
			}
			else {
				m_status.push_back(std::make_pair(dt, RepairStatus::COMPLETED));
			}
		}
	}

	if (m_rnd.next_int(100) <= 20) {
		DamageType dt = DamageType(int(DamageType::First) +  m_rnd.next_int(int(DamageType::Last) - int(DamageType::First)));
		if (m_rnd.next_int(100) >= 60) {
			m_damage[dt] += m_rnd.next_double(3) + 1;
			m_status.push_back(std::make_pair(dt, RepairStatus::IMPROVED));
		}
		else {
			m_damage[dt] -= m_rnd.next_double(5) + 1;
			m_status.push_back(std::make_pair(dt, RepairStatus::DAMAGED));
		}
	}

	if (!m_status.empty()) {
		m_events.enterprise_damage_repaired(m_status);
	}
}

void Enterprise::raise_shields(std::function<int(int)> ask_shields, bool& changed) {
	if (is_damaged(DamageType::SHIELD_CONTROL)) {
		throw ShieldControlInoperableException();
	}

	int x = ask_shields(*m_energy);

	if (x < 0 || m_energy.shields() == x) {
		changed = false;
	}
	else {
		if (x > * m_energy) {
			throw InvalidShieldChangeException();
		}

		m_energy.raise_shields(x);
		changed = true;
	}
}

void Enterprise::fire_phasers(std::function<int(int)> ask_phasers) {
	if (is_damaged(DamageType::PHASER_CONTROL)) {
		throw PhasersInoperableException();
	}

	if (m_quadrant->num_klingons() == 0) {
		throw NoKlingonsException();
	}

	if (is_damaged(DamageType::LIBRARY_COMPUTER)) {
		m_events.firing_phasers_damaged_computer();
	}

	int e = m_energy.available();
	double amt;
	do {
		amt = ask_phasers(e);
		if (amt <= 0) {
			return;
		}
	} while (e - amt < 0);
	
	m_energy.consume(int(amt));
	if (is_damaged(DamageType::LIBRARY_COMPUTER)) {
		amt *= m_rnd.next_double(1);
	}

	m_quadrant->enterprise_attack(amt);
}

void Enterprise::fire_torpedo(std::function<double(int, int)> ask_course, std::function<void(const Position&)> track, SectorVisitor& visitor, bool& missed) {
	if (m_torpedos.expended()) {
		throw TorpedoesExpendedException();
	}
	if (is_damaged(DamageType::PHOTON_TUBES)) {
		throw TorpedoesInoperableException();
	}
	Course c = ask_course(1, 9);
	m_energy.consume(2);
	m_torpedos.consume();
	m_quadrant->fire_torpedo(c, track, visitor, missed);
}

bool Enterprise::computer_damaged() const {
	return is_damaged(DamageType::LIBRARY_COMPUTER);
}

void Enterprise::repair_damage(std::function<void()> damage_control_unavailable, std::function<bool(double time)> ask_do_repairs, std::function<void(std::vector<std::pair<DamageType, double>>&)> damage_report) {
	if (is_damaged(DamageType::DAMAGE_CONTROL)) {
		damage_control_unavailable();
	}
	else if (m_docked) {
		return;
	}
	else {
		double t = 0;
		for (int dt = int(DamageType::First); dt <= int(DamageType::Last); dt++) {
			if (is_damaged(DamageType(dt))) {
				t += 0.1;
			}
		}
		if (t == 0) {
			return;
		}
		t += 0.5 * m_rnd.next_double(1);
		t = std::min(0.9, t);
		if (ask_do_repairs(t)) {
			for (int dt = int(DamageType::First); dt <= int(DamageType::Last); dt++) {
				if (is_damaged(DamageType(dt))) {
					m_damage[DamageType(dt)] = 0;
				}
			}
			m_time.advance(t);
		}
	}
	std::vector<std::pair<DamageType, double>> damage;
	for (int dt = int(DamageType::First); dt <= int(DamageType::Last); dt++) {
		damage.push_back(std::make_pair(DamageType(dt), m_damage[DamageType(dt)]));
	}
	damage_report(damage);
}

void Enterprise::undock() {
	m_docked = false;
}

bool Enterprise::enough_time_remaining(int klingons_remaining) const {
	return m_time.enough_time_remaining(klingons_remaining);
}


bool Enterprise::is_damaged(DamageType dt) const {
	return m_damage.at(dt) < 0;
}

ShortRangeSensorsInoperableException::ShortRangeSensorsInoperableException() :
	std::runtime_error("SHORT_RANGE_SENSORS") {
}

std::ostream& operator<<(std::ostream& os, const EnterpriseCondition& cond) {
	switch (cond) {
	case EnterpriseCondition::DOCKED: return os << "DOCKED";
	case EnterpriseCondition::GREEN: return os << "GREEN";
	case EnterpriseCondition::YELLOW: return os << "YELLOW";
	case EnterpriseCondition::RED: return os << "*RED*";
	default:
		return os;
	}
}

std::ostream& operator<<(std::ostream& os, const DamageType& dt) {
	switch (dt) {
	case DamageType::WARP_ENGINES: return os << "WARP ENGINES";
	case DamageType::SHORT_RANGE_SENSORS: return os << "SHORT RANGE SENSORS";
	case DamageType::LONG_RANGE_SENSORS: return os << "LONG RANGE SENSORS";
	case DamageType::PHASER_CONTROL: return os << "PHASER CONTROL";
	case DamageType::PHOTON_TUBES: return os << "PHOTON TUBES";
	case DamageType::DAMAGE_CONTROL: return os << "DAMAGE CONTROL";
	case DamageType::SHIELD_CONTROL: return os << "SHIELD CONTROL";
	case DamageType::LIBRARY_COMPUTER: return os << "LIBRARY-COMPUTER";
	default:
		return os;
	}
}

InvalidWarpFactorException::InvalidWarpFactorException(double warp, double max, bool damaged) :
	std::runtime_error("INVALID WARP FACTOR"),
	m_warp(warp),
	m_max(max),
	m_damaged(damaged) {
}

double InvalidWarpFactorException::warp() const {
	return m_warp;
}

double InvalidWarpFactorException::max() const {
	return m_max;
}

bool InvalidWarpFactorException::damaged() const {
	return m_damaged;
}

LongRangeSensorsInoperableException::LongRangeSensorsInoperableException() :
	std::runtime_error("long range sensors inoperable") {
}

ShieldControlInoperableException::ShieldControlInoperableException() : 
	std::runtime_error("shield control inoperable") {
}

InvalidShieldChangeException::InvalidShieldChangeException() :
	std::runtime_error("invalid shield change") {
}

PhasersInoperableException::PhasersInoperableException() :
	std::runtime_error("phasers inoperable") {
}

NoKlingonsException::NoKlingonsException() :
	std::runtime_error("no klingons") {
}

DamageControlInoperableException::DamageControlInoperableException() :
	std::runtime_error("damage control inoperable") {
}

NoStarbaseException::NoStarbaseException() :
	std::runtime_error("No starbase") {
}

TorpedoesExpendedException::TorpedoesExpendedException() :
	std::runtime_error("Nor torpedoes") {
}

TorpedoesInoperableException::TorpedoesInoperableException() :
	std::runtime_error("Torpedoes inoperable") {
}
