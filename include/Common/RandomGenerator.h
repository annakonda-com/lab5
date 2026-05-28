#pragma once

class RandomGen {
private:
    unsigned int state;
public:
    RandomGen(unsigned int seed) : state(seed) {}

    double Next() {
        state = (1103515245 * state + 12345) % 2147483648;
        return (static_cast<double>(state) / 1073741824.0) - 1.0;
    }
};
