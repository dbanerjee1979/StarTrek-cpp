#include "StarBase.h"
#include "Quadrant.h"

StarBase::StarBase(const Position& sector, Quadrant& quadrant) :
    m_sector(sector),
    m_quadrant(quadrant) {
}

const Position& StarBase::sector() const {
    return m_sector;
}

void StarBase::accept(SectorVisitor& visitor) {
    visitor.visit_base(*this);
}

void StarBase::torpedo_hit() {
    m_quadrant.starbase_destroyed(m_sector);
    throw StarbaseDestroyedException();
}

StarbaseDestroyedException::StarbaseDestroyedException() :
    std::runtime_error("starbase destroyed") {
}
