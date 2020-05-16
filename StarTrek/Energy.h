#pragma once
#include <stdexcept>

class Events;

class Energy {
private:
    int m_curr;
    int m_init;
    int m_shields;
    Events& m_events;
public:
    Energy(Events& events);
    bool shields_low() const;
    void dock();
    bool is_low();
    int operator *() const;
    int available() const;
    int shields() const;
    bool enough(bool shields_damaged) const;
    int distance_for_speed(double warp, bool shields_damaged) const;
    void absorb_hit(int hit);
    void maneuver(int n);
    void raise_shields(int amt);
    void consume(int amt);
};

class InsufficientNavigationEnergyException : public std::runtime_error {
private:
    double m_warp;
    int m_shields_avail;
public:
    InsufficientNavigationEnergyException(double warp, int shields_avail);
    double warp() const;
    int shields_available() const;
};

class EnterpriseDestroyedException : public std::runtime_error {
public:
    EnterpriseDestroyedException();
};
