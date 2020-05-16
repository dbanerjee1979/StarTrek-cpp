#include <stdexcept>
#include "Course.h"
#include "Position.h"

Course Course::s_courses[9] = {
    Course(0,  1),
    Course(-1,  1),
    Course(-1,  0),
    Course(-1, -1),
    Course(0, -1),
    Course(1, -1),
    Course(1,  0),
    Course(1,  1),
    Course(0,  1)
};

Course::Course(double dr, double dc) :
    m_dr(dr),
    m_dc(dc) {
}

Course::Course(double c) {
    c = c == 9 ? 1 : c;
    if (c < 1 || c >= 9) {
        throw InvalidCourseException();
    }
    int c1 = int(--c);
    *this = s_courses[c1] + (s_courses[c1 + 1] - s_courses[c1]) * (c - c1);
}

Course::Course(const Position& p) :
    m_dr(p.row()),
    m_dc(p.col()) {
}

const Course Course::operator+(const Course& other) const {
    return Course(m_dr + other.m_dr, m_dc + other.m_dc);
}

const Course Course::operator-(const Course& other) const {
    return Course(m_dr - other.m_dr, m_dc - other.m_dc);
}

const Course Course::operator*(double n) const {
    return Course(m_dr * n, m_dc * n);
}

const Course Course::operator/(double n) const {
    return *this * (1 / n);
}

bool Course::inside() const {
    return m_dr >= 0 && m_dr < 8 && m_dc >= 0 && m_dc < 8;
}

double Course::dr() const {
    return m_dr;
}

double Course::dc() const {
    return m_dc;
}

InvalidCourseException::InvalidCourseException() :
    std::runtime_error("INVALID COURSE") {
}