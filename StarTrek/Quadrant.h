#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "Entity.h"
#include "Empty.h"
#include "Klingon.h"
#include "StarBase.h"
#include "Star.h"

class Random;
class Galaxy;
class Position;
class Course;
class Events;
class Time;
class Enterprise;

class Quadrant {
private:
	Galaxy& m_galaxy;
	int m_row;
	int m_col;
	std::string m_name;
	bool m_visited;
	Random& m_rnd;
	Events& m_events;
	int m_num_klingons;
	int m_num_bases;
	int m_num_stars;
	std::vector<std::vector<std::shared_ptr<Entity>>> m_entities;
	std::shared_ptr<Empty> m_empty;
	std::vector<std::shared_ptr<Klingon>> m_klingons;
	std::vector<std::shared_ptr<StarBase>> m_bases;
public:
	Quadrant(Galaxy& galaxy, int row, int col, Random& rnd, Events& events, int& total_klingons, int& total_bases);
	void add_starbase(int& total_klingons, int& total_bases);
	void enter();
	const std::string& region() const;
	const std::string& name() const;
	bool visited() const;
	int num_klingons() const;
	int num_bases() const;
	int num_stars() const;
	bool is_adjacent_to_base(Position& p) const;
	void visit(SectorVisitor& visitor);
	const Position pos() const;
	void klingon_attack();
	void traverse(const Course& c, int dist, std::shared_ptr<Quadrant>& quadrant, Position& sector);
	void enterprise_attack(double amt);
	void torpedo_data(std::vector<std::pair<double, double>>& dir_dist);
	void starbase_data(double& dir, double& dist);
	void fire_torpedo(const Course& c, std::function<void(const Position&)> track, SectorVisitor& visitor, bool& missed);
	void klingon_destroyed(const Position& p);
	void starbase_destroyed(const Position& p);
private:
	static int init_klingon_count(Random& rnd);
	void set(const Position& p, std::shared_ptr<Entity> e);
	std::shared_ptr<Entity> get(const Position& p) const;
	Position avail_pos() const;

	static std::string s_regions[2][8];
	static std::string s_numbers[4];
};

