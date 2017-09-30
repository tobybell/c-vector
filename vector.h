#ifndef __VECTOR_H
#define __VECTOR_H

#include <stdbool.h>

/**
 * Type: Vector
 * 
 * Maintains an ordered, variable-length list of values that can be modified by
 * the client. Supports pushing and popping values to and from the end of the
 * list, inserting and removing values at arbitrary locations within the list,
 * and getting and setting the value at a particular index.
 */
typedef struct vector *vector;

/**
 * Create a new, empty vector.
 * 
 * The returned vector will have been dynamically allocated, and must be
 * destroyed after use using `vector_destroy`.
 */
vector vector_create();

/**
 * Clean up a vector after use.
 * 
 * This function must be called to avoid memory leaks. It frees the vector's
 * own storage, but it does not clean up the values that may exist inside of
 * it. If a vector is storing the only reference to any dynamically allocated
 * values, that memory must be freed by the client beforehand.
 */
void vector_destroy(vector v);

/**
 * Get the size (number of elements stored) of `v`.
 */
int vector_size(const vector v);

/**
 * Determine whether `i` is a valid index within `v`.
 */
bool vector_in_bounds(const vector v, int i);

/**
 * Write `value` at the existing index `i` in the vector `v`.
 */
void vector_set(vector v, int i, void *value);

/**
 * Get the value at index `i` in `v`.
 */
void *vector_get(const vector v, int i);

/**
 * Insert `value` at index `i` in the vector `v`.
 * 
 * This will shift all existing elements, starting at index `i`, one position
 * to the right, so that `value` can occupiy the space at index `i`.
 */
void vector_insert(vector v, int i, void *value);

/**
 * Remove and return the value at index `i` of the vector `v`.
 */
void *vector_remove(vector v, int i);

/**
 * Push `value` onto the end of the vector `v`.
 */
void vector_push(vector v, void *value);

/**
 * Remove and return the value at the end of the vector `v`.
 */
void *vector_pop(vector v);

#endif
