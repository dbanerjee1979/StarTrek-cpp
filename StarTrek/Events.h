#pragma once

#include <vector>

class Quadrant;
class Position;
enum class DamageType;
enum class RepairStatus;

class Events {
public:
    virtual void entering_quadrant_mission_start(Quadrant&) = 0;
    virtual void entering_quadrant(Quadrant&) = 0;
    virtual void shields_lowered_for_docking() = 0;
    virtual void starbase_shields_absorbed_hit() = 0;
    virtual void enterprise_hit_from_sector(int hit, const Position& sector) = 0;
    virtual void enterprise_shields_down_to(int shields) = 0;
    virtual void enterprise_damaged_by_hit(const DamageType& dt) = 0;
    virtual void enterprise_damage_repaired(const std::vector<std::pair<DamageType, RepairStatus>>& status) = 0;
    virtual void engines_shut_down_for_bad_navigation(const Position& sector) = 0;
    virtual void maneuver_energy_provided_by_shields() = 0;
    virtual void engines_shut_down_for_perimeter_breach(const Position& quadrant, const Position& sector) = 0;
    virtual void firing_phasers_damaged_computer() = 0;
    virtual void klingon_took_no_damage(const Position& sector) = 0;
    virtual void klingon_took_damage(int hit, const Position& sector) = 0;
    virtual void klingon_destroyed() = 0;
    virtual void klingon_shields_remaining(int shields) = 0;
};