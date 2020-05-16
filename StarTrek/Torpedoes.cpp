#include "Torpedoes.h"

Torpedoes::Torpedoes() :
    m_curr(10),
    m_init(10) {
}

void Torpedoes::replenish() {
    m_curr = m_init;
}

bool Torpedoes::expended() {
    return m_curr <= 0;
}

void Torpedoes::consume() {
    m_curr--;
}

int Torpedoes::operator*() const {
    return m_curr;
}
