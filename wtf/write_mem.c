#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>

char *searched = "blablubli";
char   *little = "erf\0\0\0\0\0\0\0\0\0\0\0";

#define Z_A 0x02006000
#define Z_B 0x0207c000


/* void *addr; */

int main()
{
  int pid = 15650;

  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1)
    err(1, "ptrace");

  if (waitpid(pid, NULL, 0) == -1)
    {
      ptrace(PTRACE_DETACH, pid, NULL, NULL);
      err(1, "ptrace");
    }

  int fd = open("/proc/15650/mem", O_RDWR);
  if (fd < 0)
    err(1, "could not open mem");

  printf("fd : %d\n", fd);

  /* void *addr = mmap(NULL, Z_B - Z_A, PROT_WRITE | PROT_READ,  MAP_SHARED, fd, Z_A); */
  /* printf("addr : %p\n", addr); */
  /* if (addr == MAP_FAILED) */
  /*   err(1, "mmap"); */
  if (lseek(fd, Z_A, SEEK_SET) < 0)
    err(1, "lseek");

  /* addr = malloc(Z_B - Z_A + 1); */
  int len = strlen(searched);
  char *blu = malloc(len + 1);


  int i;
  for (i = 0; i < Z_B - Z_A - len; ++i)
    {
      if (lseek(fd, Z_A + i, SEEK_SET) < 0)
	err(1, "lseek in loop to off %d", i);

      int nb = read(fd, blu, len);
      if (nb < 0)
	err(1, "read %d", nb);
      if (!strncmp(searched, blu, len))
	{
	  printf("fonud at off 0x%x\n", i);
	  /* if (lseek(fd, Z_A + i, SEEK_SET) < 0) */
	  /*   warn("lseek"); */
	  /* if (write(fd, (void *)little, (size_t)strlen(little)) < 0) */
	  /*   warn("write"); */
	  if (ptrace(PTRACE_POKEDATA, pid, Z_A + i, *(void **)little) < 0)
	    err(1, "ptrace");
	}
      /* write(STDOUT_FILENO, &c, 1); */
    }
}
