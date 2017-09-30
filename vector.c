#include "vector.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/**
 * Struct: Vector
 * 
 * Implements the storage for the vector type defined in `vector.h`. The vector
 * struct uses three members:
 *  `elems`    Array of pointers; stores the vector's contents.
 *  `capacity` Max number of elements the vector can store without
 *             reallocating.
 *  `size`     Number of elements currently stored.
 */
struct vector {
  void **elems;
  int capacity;
  int size;
};

// Two internal helper functions. Implemented at the bottom of this file.
static void **get_element(const vector v, int i);
static void extend_if_necessary(vector v);

/**
 * Create a new, empty vector.
 * 
 * The returned vector will have been dynamically allocated, and must be
 * destroyed after use using `vector_destroy`.
 */
vector vector_create() {

  // Allocate space for the vector itself, as well as its internal element 
  // storage (capacity 1 to start).
  vector v = malloc(sizeof (vector));
  assert(v != NULL);
  v->elems = malloc(sizeof (void *));
  assert(v->elems != NULL);

  // Vector metadata. Capacity starts at one, since we have allocated space for 
  // one element already.
  v->capacity = 1;
  v->size = 0;

  return v;
}

/**
 * Clean up a vector after use.
 * 
 * This function must be called to avoid memory leaks. It frees the vector's
 * own storage, but it does not clean up the values that may exist inside of
 * it. If a vector is storing the only reference to any dynamically allocated
 * values, that memory must be freed by the client beforehand.
 */
void vector_destroy(vector v) {
  free(v->elems);
  free(v);
}

/**
 * Get the size (number of elements stored) of `v`.
 */
int vector_size(const vector v) {
  return v->size;
}

/**
 * Determine whether `i` is a valid index within `v`.
 */
bool vector_in_bounds(const vector v, int i) {
  return i < (size_t) v->size;
}

/**
 * Write `value` at the existing index `i` in the vector `v`.
 */
void vector_set(vector v, int i, void *value) {

  // We use the `get_element` helper routine to safely get a pointer to the
  // given index's location in the vector's own internal storage.
  *get_element(v, i) = value;
}

/**
 * Get the value at index `i` in `v`.
 */
void *vector_get(const vector v, int i) {

  // Hand off the work to the `get_element` helper, which gives us a pointer to
  // the matching element within the vector's internal storage. Then, just
  // dereference.
  return *get_element(v, i);
}

/**
 * Insert `value` at index `i` in the vector `v`.
 * 
 * This will shift all existing elements, starting at index `i`, one position
 * to the right, so that `value` can occupiy the space at index `i`.
 */
void vector_insert(vector v,int i, void *value) {
  v->size += 1;
  extend_if_necessary(v);

  // Get a reference to the desired element position within the vector's own 
  // internal storage.
  void **target = get_element(v, i);

  // We compute the number of elements *including and after* the element to
  // remove, and then use `memmove` to shift those elements to the right so as 
  // to make room for the new value.
  int remaining = v->size - i - 1;
  memmove(target + 1, target, remaining * sizeof (void *));

  *target = value;
}

/**
 * Remove and return the value at index `i` of the vector `v`.
 */
void *vector_remove(vector v, int i) {

  // Get a reference to the desired element position within the vector's own 
  // internal storage, and save the found value to return.
  void **target = get_element(v, i);
  void *result = *target;

  // We compute the number of elements *after* the element to remove, and then
  // use `memmove` to shift all subsequent elements down to cover the removed 
  // element.
  int remaining = v->size - i - 1;
  memmove(target, target + 1, remaining * sizeof (void *));
  v->size -= 1;

  return result;
}

/**
 * Push `value` onto the end of the vector `v`.
 */
void vector_push(vector v, void *value) {

  // Offload to the existing insertion routine.
  vector_insert(v, v->size, value);
}

/**
 * Remove and return the value at the end of the vector `v`.
 */
void *vector_pop(vector v) {

  // Offload to `vector_remove`.
  return vector_remove(v, v->size - 1);
}

/**
 * Internal helper; computes a pointer to the memory location for a given index
 * `i` within `v`.
 */
static void **get_element(const vector v, int i) {
  assert(i < (size_t) v->size);
  return &v->elems[i];
}

/**
 * Internal helper; doubles the vector's internal storage capacity when 
 * necessary (*i.e.*, the vector's `size` becomes greater than its `capacity`).
 */
static void extend_if_necessary(vector v) {
  if (v->size > v->capacity) {

    // Doubling the capacity when necessary allows for an amortized constant 
    // runtime for extensions. Using `realloc` will conveniently copy the 
    // vector's existing contents to any newly allocated memory.
    v->capacity *= 2;
    v->elems = realloc(v->elems, v->capacity * sizeof (void *));
  }
}
