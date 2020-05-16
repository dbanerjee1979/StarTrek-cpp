#pragma once

class Position;

class Course {
private:
    double m_dr;
    double m_dc;

    static Course s_courses[9];
public:
    Course(double dr, double dc);
    Course(double c);
    Course(const Position& p);
    const Course operator+(const Course& other) const;
    const Course operator-(const Course& other) const;
    const Course operator*(double n) const;
    const Course operator/(double n) const;
    bool inside() const;
    double dr() const;
    double dc() const;
};

class InvalidCourseException : std::runtime_error {
public:
    InvalidCourseException();
};
