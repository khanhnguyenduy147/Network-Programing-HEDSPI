#ifndef CREATEARCHIVEDIR_H
#define CREATEARCHIVEDIR_H
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
void createArchiveDir(const char *dir_storage_name, char *root_path);
#endif