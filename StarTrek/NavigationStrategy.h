#pragma once
class NavigationStrategy {
public:
	virtual double ask_course(int min, int max) = 0;
	virtual double ask_warp(double min, double max) = 0;
};

