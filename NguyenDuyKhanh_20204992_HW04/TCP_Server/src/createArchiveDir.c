#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>

/**
 * createArchiveDir - Create an archive directory for storage.
 *
 * This function creates an archive directory for storage and sets the `root_path` to
 * the absolute path of the current directory. If the directory already exists, it
 * doesn't create a new one.
 *
 * @param dir_storage_name The name of the archive directory to create.
 * @param root_path        The buffer to store the absolute path of the current directory.
 */

void createArchiveDir(const char *dir_storage_name, char *root_path){
    //  Get the absolute path of the current directory.
    char current_directory[PATH_MAX];
    if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
        perror("Error getting the current directory");
        return;
    }

    // Copy the current directory to root_path.
    strcpy(root_path, current_directory);
    if (root_path == NULL) {
        perror("Error copying the directory path");
        return;
    }


    // Create the storage directory if it doesn't exist.
    if (mkdir(dir_storage_name, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Error creating the storage directory");
            return;
        }
    }
}