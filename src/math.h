/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef TREEBIRD_MATH_H
#define TREEBIRD_MATH_H

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define CLAMP(v, lower, upper) ((v) > (upper) ? (upper) : \
                                ((v) > (lower) ? (v) : (lower)))

#endif /* TREEBIRD_MATH_H */
