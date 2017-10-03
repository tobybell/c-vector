# C Vector

A simple but capable vector implementation in C, and a thin command line interface to interact with it.

## Using It

To clone this repository, run

    $ git clone https://github.com/tobinbell/c-vector.git

To build the provided vector object and command line interface, run

    $ cd c-vector
    $ make

You can then interact with the C Vector data structure through the provided command line interface by running

    $ ./vector-cli

### Example Usage

#### CLI

    $ ./vector-cli
    Vector CLI; use `help` if you are totally lost.
    > init
        v = []
    > push 5
        v[0] = 5
    > push 7
        v[1] = 7
    > push 12
        v[2] = 12
    > push 19
        v[3] = 19
    > remove 0
        # v[0] = 5
    > print
        v = [7, 12, 19]
    > q

#### In C

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "vector.h"
    
    int main() {
    
      // Initialize a new vector.
      vector v = vector_create();
      
      // Read in values from the user.
      printf("Enter some strings. When finished, type \"done\".\n");
      char buf[4096];
      while (true) {
        scanf(" %4095s", buf);
        if (strcmp(buf, "done") == 0) break;
        vector_push(v, strdup(buf));
      }
      
      printf("You entered %d values:\n", vector_size(v));
      for (int i = 0; i < vector_size(v); i += 1) {
        printf("  %s\n", (char *) vector_get(v, i));
      }
      
      // Free vector memory when done, including client values.
      for (int i = 0; i < vector_size(v); i += 1) {
        free(vector_get(v, i));
      }
      vector_destroy(v);
      
      return 0;
    }

## Design and Performance

C Vector uses a dynamically allocated array to store its contents. Dynamic growth is achieved via doubling of this array when necessary. As more elements are added to the vector and extension becomes more expensive, the doubling operation ensures that extensions also become less frequent, producing *O*(1) ammortized append time. Inserting and removing elements from the interior of the vector is made possible by shifting the latter portion of the array up or down accordingly, producing *O*(*n*) runtime for non-posterior insertion and removal operations.

A complete description of this data structure's runtime is given below.

| Operation | Runtime  |
|-----------|----------|
| Size      | *O*(1)   |
| Get       | *O*(1)   |
| Set       | *O*(1)   |
| Push      | *O*(1)   |
| Pop       | *O*(1)   |
| Insert    | *O*(*n*) |
| Remove    | *O*(*n*) |

## Notes

As described in *Design* above, C Vector does not store data values internally, but rather by reference. Thus, operations on a C Vector accept and return `void *` pointers, and no client data is explicitly copied by the vector. This means that client code must take responsibility for storing values, either "dyanimcally" (no pun intended) using `malloc` or in some other data structure.

Clients are also solely responsible for managing the memory holding stored values. Even when a C Vector is destroyed (using `vector_destroy`), only its own memory will be freed, and not any memory referenced by the client pointers it stores. Therefore, clients should take care to explicitly free all value memory as appropriate before destroying a C Vector.
