#pragma once

#include <functional>

class Empty;
class Enterprise;
class Klingon;
class StarBase;
class Star;

struct SectorVisitor {
	std::function<void()> visit_row_start;
	std::function<void()> visit_row_end;
	std::function<void(const Empty&)> visit_empty;
	std::function<void(const Enterprise&)> visit_enterprise;
	std::function<void(const Klingon&)> visit_klingon;
	std::function<void(const StarBase&)> visit_base;
	std::function<void(const Star&)> visit_star;
};

class Entity {
public:
	virtual bool is_empty() const;
	virtual void accept(SectorVisitor& visitor) = 0;
	virtual void torpedo_hit() = 0;
};

