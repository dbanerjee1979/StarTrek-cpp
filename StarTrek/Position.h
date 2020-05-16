#pragma once

#include <iostream>
#include <string>

class Random;
class Course;

class Position {
private:
    int m_row;
    int m_col;
public:
    Position(int row, int col);
    Position(Random& rnd);
    Position(const Course& c);
    Position(const std::string& s);
    int row() const;
    int col() const;
    double dist(const Position& other) const;
    double direction(const Position& to) const;
    bool is_adjacent(const Position& other) const;
private:
    double direction_from_y(double r, double c, double d) const;
    double direction_from_x(double r, double c, double d) const;

    friend std::ostream& operator<<(std::ostream& os, const Position& p);
};

