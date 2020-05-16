#pragma once

class Empty;
class Enterprise;
class Klingon;
class StarBase;
class Star;

class SectorVisitor {
public:
	virtual void visitor_row_start() = 0;
	virtual void visitor_row_end() = 0;
	virtual void visit(Empty&) = 0;
	virtual void visit(Enterprise&) = 0;
	virtual void visit(Klingon&) = 0;
	virtual void visit(StarBase&) = 0;
	virtual void visit(Star&) = 0;
};

