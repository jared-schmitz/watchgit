/*
 * gitfuncs.h
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "gitfuncs.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * print_repo_path()
 *
 * Simply prints out the path to the
 * give repository (a puts() call).
 */
int print_repo_path(const char *col_name, const char *thing) {
  (!strcmp(col_name, "aliases"))
    ? printf("%s: ", thing)
    : printf("%s\n", thing);

  return 0;
}

/*
 * print_repo_status()
 *
 * Invokes git to print the status of
 * the repository.
 *
 * TODO: Replace ugly system() call
 * with a proper fork() and exec().
 */
int print_repo_status(const char *col_name, const char *thing) {
  char command[PATH_MAX + 128];

  if (strcmp(col_name, "paths")) {
    printf("repo: %s\n", thing);
    return 0;
  }

  sprintf(command, "cd %s && git status -s && echo ''", thing);
  return system(command);
}

