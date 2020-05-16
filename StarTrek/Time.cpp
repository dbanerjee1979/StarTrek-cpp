#include "Time.h"
#include "Time.h"
#include "Random.h"

Time::Time(Random& rnd, int total_klingons) :
	m_curr((double) rnd.next_int(20) + 20),
	m_init(m_curr),
	m_max_days((double) 25 + rnd.next_int(25)) {
	m_max_days = total_klingons > m_max_days ? (double) total_klingons + 1 : m_max_days;
}

int Time::max_days() const {
	return (int) m_max_days;
}

int Time::deadline() const {
	return (int) (m_init + m_max_days);
}

bool Time::is_beginning() const {
	return m_curr == m_init;
}

double Time::operator*() const {
	return m_curr;
}

void Time::maneuver(double w) {
	m_curr += w >= 1 ? 1 : 0.1 * (10 * w);
	if (m_curr > deadline()) {
		throw DeadlineReachedException();
	}
}

void Time::advance(double amt) {
	m_curr += amt;
}

bool Time::enough_time_remaining(int klingons_remaining) const {
	return klingons_remaining > m_curr - m_init - m_max_days;
}

DeadlineReachedException::DeadlineReachedException() :
	std::runtime_error("deadline reached") {
}
