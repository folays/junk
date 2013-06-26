/* I can be compiled with the command "gcc -o dentls dentls.c" */

#define _GNU_SOURCE
#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string.h>


struct linux_dirent {
        long           d_ino;
        off_t          d_off;
        unsigned short d_reclen;
        char           d_name[256];
        char           d_type;
};

int main(const int argc, const char** argv) {
    int totalfiles = 0;
    int dirfd = -1;
    int offset = 0;
    int bufcount = 0;
    void *buffer = NULL;
    char *d_type;
    struct linux_dirent *dent = NULL;
    struct stat dstat;

    /* Test we have a directory path */
    if (argc < 2) {
        fprintf(stderr, "You must supply a valid directory path.\n");
        exit(1);
    }

    const char *path = argv[1];

    /* Standard sanity checking stuff */
    if (access(path, R_OK) < 0) {
        perror("Could not access directory");
        exit(1);
    }

    if (lstat(path, &dstat) < 0) {
        perror("Unable to lstat path");
        exit(1);
    }

    if (!S_ISDIR(dstat.st_mode)) {
        fprintf(stderr, "The path %s is not a directory.\n", path);
        exit(1);
    }

    /* Allocate a buffer of equal size to the directory to store dents */
    if ((buffer = malloc(dstat.st_size+10240)) == NULL) {
        perror("malloc failed");
        exit(1);
    }

    /* Open the directory */
    if ((dirfd = open(path, O_RDONLY)) < 0) {
        perror("Open error");
        exit(1);
    }

    /* Switch directories */
    fchdir(dirfd);

    while (bufcount = syscall(SYS_getdents, dirfd, buffer, dstat.st_size+10240)) {
        offset = 0;
        dent = buffer;
        while (offset < bufcount) {
            /* Dont print thisdir and parent dir */
            if (!((strcmp(".",dent->d_name) == 0) || (strcmp("..",dent->d_name) == 0))) {
                d_type = (char *)dent + dent->d_reclen-1;
                /* Only print files */
                if (*d_type == DT_REG) {
                    printf("%s\n", dent->d_name);
                    /* For deleting files, uncomment me */
                    //if (unlink(dent->d_name) < 0) {
                    //  perror("unlink");
                    //}
                }
            }
            totalfiles++;
            offset += dent->d_reclen;
            dent = buffer + offset;
        }
    }
    fprintf(stderr, "Total files: %d\n", totalfiles);

    close(dirfd);

}

