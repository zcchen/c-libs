#ifndef CTOOLINGS_ENDIAN_H
#define CTOOLINGS_ENDIAN_H

// In this union: least addr first, then, biggest addr at the end.
// When getting the ul type, it will return its system-endian unsigned long data.
// At little-endian system, the first char of the `ul` data is `c[0]`;
// At big-endian system, the first char of the `ul` data is `c[3]`.
// Base on that method, it is easy to detect the endian of system.
union endian_tester_t {
    char c[4];
    unsigned long ul;
};

const union endian_tester_t endian_tester = {
    .c = {'l', '?', '?', 'b'},
};

#define get_sys_endian() ((char)endian_tester.ul)

#endif /* ifndef CTOOLINGS_ENDIAN_H */
