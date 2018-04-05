#include <zconf.h>
#include <stdlib.h>
#include <stdio.h>
#include "change_directory.h"

void change_directory(char** arguments) {
    if (arguments[1] == NULL) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(arguments[1]) != 0) {
            perror("cd: No such file or directory");
        }
    }
}
