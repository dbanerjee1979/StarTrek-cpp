#include <math.h>
#include <stdexcept>
#include "Position.h"
#include "Random.h"
#include "Course.h"

Position::Position(int row, int col) :
	m_row(row),
	m_col(col) {
}

Position::Position(Random& rnd) :
	m_row(rnd.next_pos()),
	m_col(rnd.next_pos()) {
}

Position::Position(const Course& c) :
	m_row(int(c.dr())),
	m_col(int(c.dc())) {
}

Position::Position(const std::string& s) :
	m_row(0),
	m_col(0) {
	auto p = s.find(",");
	if (p != std::string::npos) {
		m_row = std::stoi(s.substr(0, p));
		m_col = std::stoi(s.substr(p + 1));
	}
	else {
		throw std::invalid_argument("No separator");
	}
}

int Position::row() const {
	return m_row;
}

int Position::col() const {
	return m_col;
}

double Position::dist(const Position& other) const {
	return sqrt(pow((double) m_row - other.m_row, 2) + pow((double) m_col - other.m_col, 2));
}

double Position::direction(const Position& to) const {
	double r = double(m_row) - to.m_row;
	double c = double(to.m_col) - m_col;

	double dir = 0;
	if (c < 0) {
		if (r > 0) {
			return direction_from_y(r, c, 3.0);
		}
		else if (c != 0) {
			return direction_from_x(r, c, 5.0);
		}
		else {
			return direction_from_y(r, c, 7.0);
		}
	}
	else {
		if (r < 0) {
			return direction_from_y(r, c, 7.0);
		}
		else if (c > 0) {
			return direction_from_x(r, c, 1.0);
		}
		else if (r == 0) {
			return direction_from_x(r, c, 5.0);
		}
		else {
			return direction_from_x(r, c, 1.0);
		}
	}
}

bool Position::is_adjacent(const Position& other) const {
	for (int r = other.m_row - 1; r <= other.m_row + 1; r++) {
		for (int c = other.m_col - 1; c <= other.m_col + 1; c++) {
			if (m_row == r && m_col == c) {
				return true;
			}
		}
	}
	return false;
}

double Position::direction_from_y(double r, double c, double d) const {
	if (abs(r) >= abs(c)) {
		return d + (abs(c) / abs(r));
	}
	else {
		return d + (((abs(c) - abs(r)) + abs(c)) / abs(c));
	}
}

double Position::direction_from_x(double r, double c, double d) const {
	if (abs(r) <= abs(c)) {
		return d + (abs(r) / abs(c));
	}
	else {
		return d + (((abs(r) - abs(c)) + abs(r)) / abs(r));
	}
}

std::ostream& operator<<(std::ostream& os, const Position& p) {
	return os << "(" << (p.m_row + 1) << ", " << (p.m_col + 1) << ")";
}
