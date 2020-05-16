#include "Klingon.h"
#include "Random.h"
#include "Events.h"
#include "Quadrant.h"

Klingon::Klingon(const Position& sector, Random &rnd, Events& events, Quadrant& quadrant) :
	m_sector(sector),
	m_shields(200 * (0.5 + rnd.next_double(1))),
	m_rnd(rnd),
	m_events(events),
	m_quadrant(quadrant) {
}

const Position& Klingon::sector() const {
	return m_sector;
}

void Klingon::attack(std::shared_ptr<Enterprise> ent, const Position& from) {
	m_sector = from;
	if (m_shields > 0) {
		double r = m_rnd.next_double(1);
		int h = int((m_shields / m_sector.dist(ent->sector())) * (2 + r));
		bool damaged;
		ent->hit(h, m_sector, damaged);
		if (damaged) {
			m_shields /= 3 + r;
		}
	}
}

void Klingon::accept(SectorVisitor& visitor) {
	visitor.visit_klingon(*this);
}

bool Klingon::is_destroyed() const {
	return m_shields <= 0;
}

void Klingon::hit(int amt, const Position& from) {
	double dist = from.dist(m_sector);
	int hit = int((double(amt) / dist) * (m_rnd.next_double(1) + 2));
	if (hit > 0.15 * m_shields) {
		m_shields -= hit;
		m_events.klingon_took_damage(hit, m_sector);
		if (is_destroyed()) {
			m_events.klingon_destroyed();
		}
		else {
			m_events.klingon_shields_remaining(int(m_shields));
		}
	}
	else {
		m_events.klingon_took_no_damage(m_sector);
	}
}

void Klingon::torpedo_hit() {
	m_shields = 0;
	m_quadrant.klingon_destroyed(m_sector);
}
