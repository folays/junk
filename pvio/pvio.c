#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static unsigned int hz;
static unsigned long long prev_uptime;
static unsigned long old_tot_ticks;

static unsigned long long read_uptime(void)
{
  unsigned long long uptime;
  unsigned char line[128];
  FILE *fp;
  unsigned long up_sec, up_cent;

  if ((fp = fopen("/proc/uptime", "r")) == NULL)
    err(1, "fopen");
  if (fgets(line, sizeof(line), fp) == NULL)
    err(1, "fgets");
  sscanf(line, "%lu.%lu", &up_sec, &up_cent);
  uptime = (unsigned long long) up_sec * hz + (unsigned long long) up_cent * hz / 100;
  fclose(fp);
  return uptime;
}

static void pvio_init()
{
  long ticks;

  if ((ticks = sysconf(_SC_CLK_TCK)) == -1)
    err(1, "sysconf");
  hz = (unsigned int)ticks;
}

static double pvio_get_util(const char *name)
{
  FILE *file;
  char line[256];

  unsigned int major, minor;
  char dev_name[256];
  unsigned long tot_ticks;
  double util = -1;

  if (!(file = fopen("/proc/diskstats", "r")))
    err(1, "fopen");
  while (fgets(line, sizeof(line), file))
    {
      int items;

      items = sscanf(line, "%u %u %s %*u %*u %*u %*u %*u %*u %*u %*u %*u %lu %*u",
      		     &major, &minor, dev_name, &tot_ticks);

      if (strcmp(dev_name, name))
	continue;

      unsigned long long curr_uptime = read_uptime();

      if (!prev_uptime)
	{
	  prev_uptime = curr_uptime - 1; /* that's nearly impossible to have a uptime of 0 jiffies ... */
	  old_tot_ticks = tot_ticks;
	}

      unsigned long itv = curr_uptime - prev_uptime;
      
      util = ((double)(tot_ticks - old_tot_ticks)) / itv * hz / 10.0;

      prev_uptime = curr_uptime;
      old_tot_ticks = tot_ticks;
    }
  fclose(file);
  return util;
}

static double aload;
static unsigned int bytes = 4096;

static void pvio_bli(const char *name)
{
  double util;

  util = pvio_get_util(name);
  //  printf("util : %f\n", util);
  if (util >= 0)
    {
      aload *= 1884;
      aload += util * (2048 - 1884);
      aload /= 2048;
      //printf("aload : %f\n", aload);
    }
  fprintf(stderr, "--> %7.3f %7.3f\t\t%d\n", util, aload, bytes);
}

int main(int argc, char **argv)
{
  pvio_init();
  while (1)
    {
      if (bytes > 0)
	splice(STDIN_FILENO, NULL, STDOUT_FILENO, NULL, bytes, SPLICE_F_MORE);
      pvio_bli("sda");
      usleep(100000);
    }
}
