#pragma once

class Quadrant;

class QuadrantVisitor {
public:
	virtual void visitRowStart() = 0;
	virtual void visitRowEnd() = 0;
	virtual void visitOutside() = 0;
	virtual void visit(Quadrant& quad) = 0;
};

