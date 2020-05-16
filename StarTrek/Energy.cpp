#include "Energy.h"
#include "Events.h"
#include <algorithm>

Energy::Energy(Events& events) :
	m_curr(3000),
	m_init(m_curr),
	m_shields(0),
	m_events(events) {
}

bool Energy::shields_low() const {
	return m_shields <= 200;
}

void Energy::dock() {
	m_curr = m_init;
	m_shields = 0;
	m_events.shields_lowered_for_docking();
}

bool Energy::is_low() {
	return m_curr < m_init / 10;
}

int Energy::operator*() const {
	return m_curr + m_shields;
}

int Energy::available() const {
	return m_curr;
}

int Energy::shields() const {
	return m_shields;
}

bool Energy::enough(bool shields_damaged) const {
	return **this > 10 && (m_curr > 10 || !shields_damaged);
}

int Energy::distance_for_speed(double warp, bool shields_damaged) const {
	int n = int(warp * 8 + 0.5);
	if (m_curr - n < 0) {
		int shields_avail = 0;
		int shields_available = (m_shields >= (n - m_curr) && !shields_damaged) ? m_shields : 0;
		throw InsufficientNavigationEnergyException(warp, shields_avail);
	}
	return n;
}

void Energy::absorb_hit(int hit) {
	m_shields -= hit;
	if (m_shields <= 0) {
		throw EnterpriseDestroyedException();
	}
}

void Energy::maneuver(int n) {
	m_curr -= n + 10;
	if (m_curr < 0) {
		m_events.maneuver_energy_provided_by_shields();
		m_shields += m_curr;
		m_curr = 0;
		m_shields = std::max(m_curr, 0);
	}
}

void Energy::raise_shields(int amt) {
	m_curr = m_curr + m_shields + amt;
	m_shields = amt;
}

void Energy::consume(int amt) {
	m_curr -= amt;
}

InsufficientNavigationEnergyException::InsufficientNavigationEnergyException(double warp, int shields_avail) :
	std::runtime_error("insufficient energy"),
	m_warp(warp),
	m_shields_avail(shields_avail) {
}

double InsufficientNavigationEnergyException::warp() const {
	return m_warp;
}

int InsufficientNavigationEnergyException::shields_available() const {
	return m_shields_avail;
}

EnterpriseDestroyedException::EnterpriseDestroyedException() :
	std::runtime_error("enterprise destroyed") {
}
