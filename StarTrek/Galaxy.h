#pragma once
#include <vector>
#include <memory>
#include "Quadrant.h"
#include "Time.h"
#include "Enterprise.h"

class Random;
class Position;
class Events;
class Quadrant;

struct QuadrantVisitor {
    std::function<void()> visit_row_start;
    std::function<void()> visit_row_end;
    std::function<void()> visit_outside;
    std::function<void(Quadrant & quad)> visit;
};

class Galaxy {
private:
    int m_total_klingons;
    int m_total_bases;
    int m_init_klingons;
    std::vector<std::vector<std::shared_ptr<Quadrant>>> m_quadrants;
    std::shared_ptr<Enterprise> m_enterprise;
public:
    Galaxy(Random& rnd, Events& events);
    int total_klingons() const;
    int total_bases() const;
    std::shared_ptr<Quadrant> quadrant(const Position& pos);
    std::shared_ptr<Enterprise> enterprise();
    void long_range_scan(std::shared_ptr<Quadrant> from, QuadrantVisitor& visitor);
    void klingon_destroyed();
    void starbase_destroyed();
    void visit_quadrants(QuadrantVisitor& visitor) const;
};

class NoBasesAndNoTimeException : std::runtime_error {
public:
    NoBasesAndNoTimeException();
};

