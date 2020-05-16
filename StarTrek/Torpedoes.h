#pragma once

class Torpedoes {
private:
	int m_curr;
	int m_init;
public:
	Torpedoes();
	void replenish();
	bool expended();
	void consume();
	int operator *() const;
};

