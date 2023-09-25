/*
    Note: This source code is written by beginners and can be terrible.
    This program does not handle all exceptions, and there are many security vulnerabilities.
*/

#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "file.h"
#include "server.h"
#include <stdlib.h>

#pragma warning(disable:4996)

char dataPath[2048] = { 0, };
char userDataPath[2048] = { 0, };
char postDataPath[2048] = { 0, };

void createCyDirectories() {
    if (!exists(dataPath) || !isDirExists(dataPath)) {
        if (mkdir(dataPath) == 0) {
            puts("Created cynetwork_data directory");
        }
        else {
            perror("Could not create cynetwork_data directory");
            abort();
        }
    }

    if (!exists(userDataPath) || !isDirExists(userDataPath)) {
        if (mkdir(userDataPath) == 0) {
            puts("Created user_data directory");
        }
        else {
            perror("Could not create user_data directory");
            abort();
        }
    }

    if (!exists(postDataPath) || !isDirExists(postDataPath)) {
        if (mkdir(postDataPath) == 0) {
            puts("Created post_data directory");
        }
        else {
            perror("Could not create post_data directory");
            abort();
        }
    }
}

int main(int argc, char** argv) {
    strncpy(dataPath, argv[0], strlen(argv[0]) - strlen(strrchr(argv[0], '\\')) + 1);
    strcat(dataPath, "\\cynetwork_data");
    strcat(userDataPath, dataPath);
    strcat(userDataPath, "\\user_data");
    strcat(postDataPath, dataPath);
    strcat(postDataPath, "\\post_data");

    createCyDirectories();

    puts("Starting CyNetwork Server...");

    server();

    return 0;
}