#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "Random.h"

Random::Random() {
    srand(time(NULL));
}

int Random::next_int(int bound) {
    return rand() % bound;
}

double Random::next_double(double bound) {
    double r = fabs(double(rand()));
    return r - bound * floor(r / bound);
}

int Random::next_pos() {
    return int(next_double(7.98) + 1.01) - 1;
}
