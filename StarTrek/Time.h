#pragma once

#include <stdexcept>

class Random;

class Time {
private:
	double m_curr;
	double m_init;
	double m_max_days;
public:
	Time(Random& rnd, int total_klingons);
	int max_days() const;
	int deadline() const;
	bool is_beginning() const;
	double operator *() const;
	void maneuver(double w);
	void advance(double amt);
	bool enough_time_remaining(int klingons_remaining) const;
};

class DeadlineReachedException : public std::runtime_error {
public:
	DeadlineReachedException();
};

