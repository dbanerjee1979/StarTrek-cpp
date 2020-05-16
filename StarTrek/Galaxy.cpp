#include "Galaxy.h"
#include "Position.h"

Galaxy::Galaxy(Random& rnd, Events& events) :
    m_total_klingons(0),
    m_total_bases(0) {
    for (int r = 0; r < 8; r++) {
        m_quadrants.push_back(std::vector<std::shared_ptr<Quadrant>>());
        for (int c = 0; c < 8; c++) {
            m_quadrants[r].push_back(std::make_shared<Quadrant>(*this, r, c, rnd, events, m_total_klingons, m_total_bases));
        }
    }
    m_init_klingons = m_total_klingons;
    if (m_total_bases == 0) {
        Position p = rnd;
        m_quadrants[p.row()][p.col()]->add_starbase(m_total_klingons, m_total_bases);
    }
    m_enterprise = std::make_shared<Enterprise>(rnd, *this, events);
}

int Galaxy::total_klingons() const {
    return m_total_klingons;
}

int Galaxy::total_bases() const {
    return m_total_bases;
}

std::shared_ptr<Quadrant> Galaxy::quadrant(const Position& pos) {
    return m_quadrants[pos.row()][pos.col()];
}

std::shared_ptr<Enterprise> Galaxy::enterprise() {
    return m_enterprise;
}

void Galaxy::long_range_scan(std::shared_ptr<Quadrant> from, QuadrantVisitor& visitor) {
    const Position& pos = from->pos();
    int cr = pos.row();
    int cc = pos.col();
    for (int r = cr - 1; r <= cr + 1; r++) {
        visitor.visit_row_start();
        for (int c = cc - 1; c <= cc + 1; c++) {
            if (r < 0 || r > 7 || c < 0 || c > 7) {
                visitor.visit_outside();
            }
            else {
                visitor.visit(*m_quadrants[r][c]);
            }
        }
        visitor.visit_row_end();
    }
}

void Galaxy::klingon_destroyed() {
    m_total_klingons--;
}

void Galaxy::starbase_destroyed() {
    m_total_bases--;
    if (m_total_bases == 0 && !m_enterprise->enough_time_remaining(m_total_klingons)) {
        throw NoBasesAndNoTimeException();
    }
    m_enterprise->undock();

}

void Galaxy::visit_quadrants(QuadrantVisitor& visitor) const {
    for (int r = 0; r < 8; r++) {
        visitor.visit_row_start();
        for (int c = 0; c < 8; c++) {
            visitor.visit(*m_quadrants[r][c]);
        }
        visitor.visit_row_end();
    }
}

NoBasesAndNoTimeException::NoBasesAndNoTimeException() :
    std::runtime_error("not enough time left and no starbases") {
}
