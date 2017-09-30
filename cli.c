#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

#define MAX_LINE 80

/**
 * Read at most `MAX_LINE` characters (plus terminal newline) from `stdin` into
 * the buffer `dest`. If the line is too long, will reprompt automatically.
 * Returns a bool indicating whether `stdin` is still open or not.
 */
bool read_cmd(char *dest) {

  while (true) {

    // Return false when input has been closed.
    if (feof(stdin)) return false;

    printf("> ");

    // Process at most MAX_LINE characters from stdin.
    for (int i = 0; i <= MAX_LINE; i += 1) {
      char ch = getchar();
      if (ch == '\n' || ch == '\0') {
        dest[i] = '\0';
        return true;
      }
      dest[i] = ch;
    }

    // We overran the max line length. Consume the rest of the line and
    // reprompt.
    for (char ch = getchar(); ch != '\n' && ch != '\0'; ch = getchar());
    printf("    error; line too long (> 80)\n");
  }
}

/**
 * Clean up an existing vector's memory, freeing both the vector itself and its
 * contents.
 */
void do_cleanup(vector v) {
  if (v != NULL) {
    for (int i = 0; i < vector_size(v); i += 1) {
      free(vector_get(v, i));
    }
    vector_destroy(v);
  }
}

/**
 * Make sure a vector has been initialized before trying to operate on it. If
 * `v` is NULL, will print out an error message suggesting the `init` command
 * and then return false. Otherwise, returns true.
 */
bool ensure_exists(vector v) {
  if (v != NULL) return true;
  printf("    error; use `init` first to initialize a new empty vector\n");
  return false;
}

/**
 * Parse a line that should not contain any other arguments. Returns
 * `true`/`false` to indicate success.
 */
bool parse(char *line, char *cmd) {
  if (strtok(NULL, " ") != NULL) {
    printf("    error; use format `%s`\n", cmd);
    return false;
  }
  return true;
}

/**
 * Parse a line with an index argument. The found index will be placed into `i`.
 * Returns `true`/`false` to indicate success.
 */
bool parse_i(char *line, char *cmd, int *i) {
  char *index = strtok(NULL, " ");
  if (index != NULL) {
    *i = strtol(index, &index, 0);
  }
  if (index == NULL || *index != '\0' || strtok(NULL, " ") != NULL) {
    printf("    error; use format `%s %%d`\n", cmd);
    return false;
  }
  return true;
}

/**
 * Parse a line with a value argument. The found value (dynamically allocated)
 * will be placed into `value`. Returns `true`/`false` to indicate success.
 */
bool parse_v(char *line, char *cmd, char **value) {
  char *token = strtok(NULL, " ");
  if (token == NULL || strtok(NULL, " ") != NULL) {
    printf("    error; use format `%s %%[^ ]`\n", cmd);
    return false;
  }
  *value = strdup(token);
  return true;
}

/**
 * Parse a line with an index and value argument. The found index will be
 * placed into `i` and the found value (dynamically allocated) will be placed
 * in `value`. Returns `true`/`false` to indicate success.
 */
bool parse_iv(char *line, char *cmd, int *i, char **value) {
  char *index = strtok(NULL, " ");
  char *token = strtok(NULL, " ");
  if (index != NULL) {
    *i = strtol(index, &index, 0);
  }
  if (token == NULL || *index != '\0' || strtok(NULL, " ") != NULL) {
    printf("    error; use format `%s %%d %%[^ ]`\n", cmd);
    return false;
  }
  *value = strdup(token);
  return true;
}

/**
 * Accepts a command string `command` and runs the correct routine.
 */
void run_cmd(char *line) {

  // Stores the vector manipulated by the shell.
  static vector v = NULL;

  // Extract the command from the string using `strtok`.
  char *cmd = strtok(line, " ");
  if (cmd == NULL) return;

  // Command: `help`. List commands.
  if (strcmp(cmd, "help") == 0) {
    if (!parse(line, cmd)) return;
    printf("    help                List available commands\n");
    printf("    exit/quit/q         Exit vector shell\n");
    printf("    init                Initialize new empty vector\n");
    printf("    size                Get current vector size\n");
    printf("    ls/print/dump       Get all vector contents\n");
    printf("    set <i> <value>     Set <value> at index <i>\n");
    printf("    get <i>             Get the value at index <i>\n");
    printf("    insert <i> <value>  Insert <value> into index <i>\n");
    printf("    remove <i>          Remove the value at index <i>\n");
    printf("    push <value>        Push <value> to end of vector\n");
    printf("    pop                 Remove the value at end of vector\n");
  }

  // Command: `exit`, `quit`. Closes the shell.
  else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0 ||
      strcmp(cmd, "q") == 0) {
    if (!parse(line, cmd)) return;
    exit(0);
  }

  // Command: `init`. Creates a new, empty vector.
  else if (strcmp(cmd, "init") == 0) {
    if (!parse(line, cmd)) return;
    do_cleanup(v);
    v = vector_create();
    printf("    v = []\n");
  }

  // Command: `size`. Gets the current size of the vector.
  else if (strcmp(cmd, "size") == 0) {
    if (!parse(line, cmd)) return;
    if (!ensure_exists(v)) return;
    printf("    |v| = %d\n", vector_size(v));
  }

  // Command: `ls`, `dump`, `print`. Prints the full vector contents.
  else if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "dump") == 0 || 
      strcmp(cmd, "print") == 0) {
    if (!parse(line, cmd)) return;
    if (!ensure_exists(v)) return;
    printf("    v = [");
    for (int i = 0; i < vector_size(v); i += 1) {
      printf(i == 0 ? "%s" : ", %s", vector_get(v, i));
    }
    printf("]\n");
  }

  // Command: `set %d %[^ ]`. Set a new value at an existing index.
  else if (strcmp(cmd, "set") == 0) {
    int i;
    char *value;
    if (!parse_iv(line, cmd, &i, &value)) return;
    if (!ensure_exists(v)) return;

    // Cannot set out of bounds.
    if (!vector_in_bounds(v, i)) {
      printf("    error; out of bounds\n");
    } else {
      vector_set(v, i, value);
      printf("    v[%d] = %s\n", i, value);
    }
  }

  // Command: `get %d`. Prints the value at a given index.
  else if (strcmp(cmd, "get") == 0) {
    int i;
    if (!parse_i(line, cmd, &i)) return;
    if (!ensure_exists(v)) return;

    // Cannot read from out of bounds.
    if (!vector_in_bounds(v, i)) {
      printf("    error; out of bounds\n");
    } else {
      char *value = vector_get(v, i);
      printf("    v[%d] = %s\n", i, value);
    }
  }

  // Command: `insert %d %[^ ]`. Insert a new value at a given index.
  else if (strcmp(cmd, "insert") == 0) {
    int i;
    char *value;
    if (!parse_iv(line, cmd, &i, &value)) return;
    if (!ensure_exists(v)) return;

    // Cannot insert if out of bounds.
    if (i < 0 || i > vector_size(v)) {
      printf("    error; out of bounds\n");
    } else {
      vector_insert(v, i, value);
      printf("    v[%d] = %s\n", i, value);
    }
  }

  // Command: `remove %d`. Remove the value at a given index.
  else if (strcmp(cmd, "remove") == 0) {
    int i;
    if (!parse_i(line, cmd, &i)) return;
    if (!ensure_exists(v)) return;

    // Cannot remove from out of bounds.
    if (!vector_in_bounds(v, i)) {
      printf("    error; out of bounds\n");
    } else {
      char *value = vector_remove(v, i);
      printf("    # v[%d] = %s\n", i, value);
      free(value);
    }
  }

  // Command: `push %[^ ]`. Pushes a new string value onto the end of the
  // vector.
  else if (strcmp(cmd, "push") == 0) {
    char *value;
    if (!parse_v(line, cmd, &value)) return;
    if (!ensure_exists(v)) return;
    vector_push(v, value);
    printf("    v[%d] = %s\n", vector_size(v) - 1, value);
  }

  // Command: `pop`. Removes and prints the value at the end of the vector.
  else if (strcmp(cmd, "pop") == 0) {
    if (!parse(line, cmd)) return;
    if (!ensure_exists(v)) return;

    // Cannot pop from an empty vector.
    if (vector_size(v) == 0) {
      printf("    error; empty\n");
    } else {
      char *value = vector_pop(v);
      printf("    # v[%d] = %s\n", vector_size(v), value);
      free(value);
    }
  }

  else {
    printf("    error; unknown command\n");
  }
}

int main() {

  printf("Vector CLI; use `help` if you are totally lost.\n");

  char command[MAX_LINE + 1];
  while (read_cmd(command)) {
    run_cmd(command);
  }

  return 0;
}
