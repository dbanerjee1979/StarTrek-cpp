#pragma once

class Random {
public:
    Random();
    int next_int(int bound);
    double next_double(double bound);
    int next_pos();
};

