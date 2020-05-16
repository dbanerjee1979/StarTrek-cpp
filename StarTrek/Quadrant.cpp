#include "Quadrant.h"
#include "Random.h"
#include "Galaxy.h"
#include "Position.h"
#include "Klingon.h"
#include "StarBase.h"
#include "Star.h"
#include "Course.h"
#include "Events.h"

std::string Quadrant::s_regions[2][8] = {
	{ "ANTARES", "RIGEL", "PROCYON", "VEGA", "CANOPUS", "ALTAIR", "SAGITTARIUS", "POLLUX" },
	{ "SIRIUS", "DENEB", "CAPELLA", "BETELGEUSE", "ALDEBARAN", "REGULUS", "ARCTURUS", "SPICA" }
};

std::string Quadrant::s_numbers[4] = { "I", "II", "III", "IV" };

Quadrant::Quadrant(Galaxy& galaxy, int row, int col, Random& rnd, Events& events, int& total_klingons, int& total_bases) :
	m_galaxy(galaxy),
	m_row(row),
	m_col(col),
	m_name(s_regions[m_col / 4][m_row] + " " + s_numbers[m_col % 4]),
	m_visited(false),
	m_rnd(rnd),
	m_events(events),
	m_num_klingons(init_klingon_count(rnd)),
	m_num_bases(rnd.next_int(100) > 96 ? 1 : 0),
	m_num_stars(rnd.next_pos() + 1),
	m_empty(new Empty()) {
	total_klingons += m_num_klingons;
	total_bases += m_num_bases;
	for (int r = 0; r < 8; r++) {
		m_entities.push_back(std::vector<std::shared_ptr<Entity>>());
		for (int c = 0; c < 8; c++) {
			m_entities[r].push_back(std::shared_ptr<Entity>());
		}
	}
}

void Quadrant::add_starbase(int& total_klingons, int& total_bases) {
	int k_incr = m_num_klingons < 2 ? m_num_klingons + 1 : m_num_klingons;
	m_num_klingons += k_incr;
	total_klingons += k_incr;
	m_num_bases++;
	total_bases++;
}

void Quadrant::enter() {
	m_visited = true;

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			m_entities[r][c] = m_empty;
		}
	}
	auto ent = m_galaxy.enterprise();
	set(ent->sector(), ent);

	m_klingons.clear();
	for (int i = 0; i < m_num_klingons; i++) {
		Position p = avail_pos();
		std::shared_ptr<Klingon> klingon = std::make_shared<Klingon>(p, m_rnd, m_events, *this);
		set(p, klingon);
		m_klingons.push_back(klingon);
	}

	m_bases.clear();
	for (int i = 0; i < m_num_bases; i++) {
		Position p = avail_pos();
		std::shared_ptr<StarBase> base = std::make_shared<StarBase>(p, *this);
		set(p, base);
		m_bases.push_back(base);
	}

	for (int i = 0; i < m_num_stars; i++) {
		Position p = avail_pos();
		std::shared_ptr<Star> star = std::make_shared<Star>(p);
		set(p, star);
	}
}

const std::string& Quadrant::region() const {
	return s_regions[m_col / 4][m_row];
}

const std::string &Quadrant::name() const {
	return m_name;
}

bool Quadrant::visited() const {
	return m_visited;
}

int Quadrant::num_klingons() const {
	return m_num_klingons;
}

int Quadrant::num_bases() const {
	return m_num_bases;
}

int Quadrant::num_stars() const {
	return m_num_stars;
}

bool Quadrant::is_adjacent_to_base(Position& p) const {
	for (auto it = m_bases.begin(); it != m_bases.end(); it++) {
		if (p.is_adjacent((*it)->sector())) {
			return true;
		}
	}
	return false;
}

void Quadrant::visit(SectorVisitor& visitor) {
	for (int r = 0; r < 8; r++) {
		visitor.visit_row_start();
		for (int c = 0; c < 8; c++) {
			m_entities[r][c]->accept(visitor);
		}
		visitor.visit_row_end();
	}
}

const Position Quadrant::pos() const {
	return Position(m_row, m_col);
}

void Quadrant::klingon_attack() {
	for (auto it = m_klingons.begin(); it != m_klingons.end(); it++) {
		auto k = *it;
		set(k->sector(), m_empty);
		Position from = avail_pos();
		k->attack(m_galaxy.enterprise(), from);
		set(from, k);
	}
}

void Quadrant::traverse(const Course& c, int dist, std::shared_ptr<Quadrant>& quadrant, Position& sector) {
	Course p = sector;
	for (int i = 0; i < dist; i++) {
		p = p + c;
		if (p.inside()) {
			if (!get(p)->is_empty()) {
				p = p - c;
				m_events.engines_shut_down_for_bad_navigation(p);
				break;
			}
		}
		else {
			p = static_cast<Course>(quadrant->pos()) * 8 + sector + c * dist;
			Position q = p / 8;
			Position s = p - static_cast<Course>(q) * 8;
			if (s.row() < 0) {
				q = Position(q.row() - 1, q.col());
				s = Position(7, s.col());
			}
			if (s.col() < 0) {
				q = Position(q.row(), q.col() - 1);
				s = Position(s.row(), 7);
			}
			bool stopped = false;
			if (q.row() < 0) {
				q = Position(0, q.col());
				s = Position(0, s.col());
				stopped = true;
			}
			else if (q.row() > 7) {
				q = Position(7, q.col());
				s = Position(7, s.col());
				stopped = true;
			}
			if (q.col() < 0) {
				q = Position(q.row(), 0);
				s = Position(s.row(), 0);
				stopped = true;
			}
			else if (q.col() > 7) {
				q = Position(q.row(), 7);
				s = Position(s.row(), 7);
				stopped = true;
			}
			quadrant = m_galaxy.quadrant(q);
			p = s;
			if (stopped) {
				m_events.engines_shut_down_for_perimeter_breach(quadrant->pos(), sector);
			}
			break;
		}
	}
	
	set(sector, m_empty);
	sector = p;
	set(sector, m_galaxy.enterprise());
}

void Quadrant::enterprise_attack(double amt) {
	const Position& from = m_galaxy.enterprise()->sector();
	amt = int(amt / m_num_klingons);
	for (auto it = m_klingons.begin(); it != m_klingons.end(); it++) {
		auto klg = *it;
		if (!klg->is_destroyed()) {
			klg->hit(amt, from);
			if (klg->is_destroyed()) {
				klingon_destroyed(klg->sector());
			}
		}
	}
}

void Quadrant::torpedo_data(std::vector<std::pair<double, double>>& dir_dist) {
	if (m_num_klingons == 0) {
		throw NoKlingonsException();
	}
	dir_dist = std::vector<std::pair<double, double>>();
	const Position &from = m_galaxy.enterprise()->sector();
	for (auto it = m_klingons.begin(); it != m_klingons.end(); it++) {
		if (!(*it)->is_destroyed()) {
			const Position& to = (*it)->sector();
			dir_dist.push_back(std::make_pair(from.direction(to), from.dist(to)));
		}
	}
}

void Quadrant::starbase_data(double& dir, double& dist) {
	if (m_num_bases == 0) {
		throw NoStarbaseException();
	}
	auto from = m_galaxy.enterprise()->sector();
	auto to = m_bases[0]->sector();
	dir = from.direction(to);
	dist = from.dist(to);
}

void Quadrant::fire_torpedo(const Course& c, std::function<void(const Position&)> track, SectorVisitor& visitor, bool& missed) {
	Course p = c + m_galaxy.enterprise()->sector();
	while (p.inside()) {
		track(p);
		auto e = get(p);
		if (!e->is_empty()) {
			e->torpedo_hit();
			e->accept(visitor);
			missed = false;
			return;
		}
		p = p + c;
	}
	missed = true;
}

void Quadrant::klingon_destroyed(const Position& p) {
	set(p, m_empty);
	m_num_klingons--;
	m_galaxy.klingon_destroyed();
}

void Quadrant::starbase_destroyed(const Position& p) {
	set(p, m_empty);
	m_num_bases--;
	m_galaxy.starbase_destroyed();
}

int Quadrant::init_klingon_count(Random& rnd) {
	int r = rnd.next_int(100);
	return r > 98 ? 3 : r > 95 ? 2 : r > 80 ? 1 : 0;
}

void Quadrant::set(const Position& p, std::shared_ptr<Entity> e) {
	m_entities[p.row()][p.col()] = e;
}

std::shared_ptr<Entity> Quadrant::get(const Position& p) const {
	return m_entities[p.row()][p.col()];
}

Position Quadrant::avail_pos() const {
	while (true) {
		Position p = m_rnd;
		if (get(p)->is_empty()) {
			return p;
		}
	}
}
