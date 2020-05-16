#include "Star.h"

Star::Star(const Position& sector) :
	m_sector(sector) {
}

const Position& Star::sector() const {
	return m_sector;
}

void Star::accept(SectorVisitor& visitor) {
	visitor.visit_star(*this);
}

void Star::torpedo_hit() {
}
