// octects a envoyer par seconde
#define BITRATE 1000
/* octects a bufferiser (octects a avoir envoye "en trop" par rapport au bitrate en permancence)
 * ne pas envoyer trop sinon le client risque de penser qu'on envoie trop vite et il risque de
 *  sauter en avant ou accelererer temporairement
 */
#define BUFFER_MIN 64 * 1024
#define BUFFER_MAX 128 * 1024

static struct tv last_tv;
int total_nbytes = 0;

int main()
{
  {
    int flag;
    fcntl(s, F_GETFL);
    fcntl(s, F_SETFL, flag | O_NONBLOCK);
  }
  gettimeofday(&last_tv);
  schedule_timeout(0); // 0 for immediate timeout
}

on_timer()
{
  struct tv tv;
  int nbytes;

  gettimeofday(&tv);
  // on calcule la difference de temps
  int time_diff_in_ms = (tv.tv_sec - last_tv.tv_sec) * 1000 - (tv.tv_usec - last_tv.tv_usec);
  // on calcule le nombre d'octects a send() pour atteindre un buffer de BUFFER_MAX
  nbytes = BUFFER_MAX - (total_nbytes - time_diff_in_ms * BITRATE * 1000);
  if (nbytes > 0)
    {
      nbytes = send(nbytes);
      if (nbytes < 0 && errno != EAGAIN)
	err(1, "bloh ? :(");
      else if (nbytes == 0)
	errx(1, "c'est fini!");
      if (nbytes > 0)
	total_nbytes += nbytes;
    }
  while (total_nbytes >= BITRATE && last_tv.tv_sec >= 1)
    { /* ca c'est pour eviter d'overflow */
      total_nbytes -= BITRATE;
      last_tv.tv_sec += 1;
    }
  /*
   * quelques maths:
   *  buffer(seconds) = buffer en fonction du temps
   *  on veut que : buffer(seconds) >= BUFFER_MIN
   *  d'autre part, on sait que buffer(seconds) = total_nbytes - BITRATE * seconds
   * quelques maths:
   *  buffer(seconds) >= BUFFER_MIN
   *  total_nbytes - BITRATE * seconds >= BUFFER_MIN
   *  -seconds >= (BUFFER_MIN - total_nbytes) / BITRATE
   *  seconds <= -(BUFFER_MIN - total_nbytes) / BITRATE ## inversion de signes
   *  seconds <= (total_nbytes - BUFFER_MIN) / BITRATE
   * --> donc il faut que le timeout en secondes soit <= (total_nbytes - BUFFER_MIN) / BITRATE
   */
  int timeout_in_ms = (total_nbytes - BUFFER_MIN) * 1000 / BITRATE;
  if (timeout_in_ms < 0) /* si le timeout est negatif parce qu'on a monis de BUFFER_MIN d'avance */
    timeout_in_ms = 0; /* reschedule immediatelly */
  schedule_timeout(timeout_in_ms); // in ms
}