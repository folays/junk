#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char **argv)
{
  DIR *dir;
  struct dirent *dirent;

  char *dirname = strdup(argv[1]);
  strrchr(dirname, '/')[1] = '\0';

  char *filename = strrchr(argv[1], '/');
  if (!filename)
    filename = argv[1];
  else
    filename = filename + 1;

  printf("Compare INODES\n");
  printf("Dirname..: %s\n", dirname);
  printf("Filename.: %s\n", filename);

  struct stat dir_buf;
  stat(dirname, &dir_buf);

  dir = opendir(dirname);
  while (dirent = readdir(dir))
    {
      if (!strcmp(dirent->d_name, filename))
	printf("dirent->d_ino -> %s ino/%lu (dev/%lu)\n", dirent->d_name, dirent->d_ino, dir_buf.st_dev);
    }
  struct stat file_buf;
  stat(argv[1], &file_buf);
  printf("stat->st_dev  -> %s ino/%lu (dev/%lu)\n", filename, file_buf.st_ino, file_buf.st_dev);
}
