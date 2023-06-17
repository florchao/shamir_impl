#include "utils.h"

uint8_t  sum(uint64_t x, uint64_t y) {
    return mod(x + y);
}
uint8_t  sub(uint64_t x, uint64_t y) {
    return mod(x - y);
}
uint8_t  mul(uint64_t x, uint64_t y) {
    return mod(x * y);
}
uint8_t  modDiv(uint64_t x, uint64_t y) {
    return mul(x, inverses[mod(y)]);
}
uint8_t mod(int64_t x) {
    while (x < 0) {
        x += P;
    }
    return x % P;
}