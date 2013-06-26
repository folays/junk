#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct pt
{
  char x, y, z;
};

static char cube[125];
#define POS(x, y, z) (&cube[x + y * 5 + z * 25])

struct rot
{
  char x, y, z;
};

struct piece_pos
{
  struct pt pt[5];
};

static struct piece_pos one_piece = {{{0, 0, 0},
				      {0, 1, 0},
				      {0, 2, 0},
				      {0, 3, 0},
				      {1, 1, 0}}};
static struct piece_pos *all_pieces;
static int nb_all_pieces;

static struct piece_pos inserted[25];
static int nb_inserted;

static void try(void);

static int myflag;

static void display_cube()
{
  int y, z;

  printf("=== CUBE BEGIN\n");
  for (y = 0; y < 5; ++y)
    {
      printf("--- plane x %d\n", y);
      for (z = 4; z >= 0; --z)
	{
	  printf("%d %d %d %d %d\n",
		 *POS(0, y, z),
		 *POS(1, y, z),
		 *POS(2, y, z),
		 *POS(3, y, z),
		 *POS(4, y, z));
	}
    }
  printf("=== CUBE END\n");
}

static void display_inserted()
{
  int i, j;

  printf("solution!\n");
  for (i = 0; i < nb_inserted; ++i)

    {
      printf("--- piece %d\n", i + 1);

      struct piece_pos *piece = &inserted[i];
      for (j = 1; j < sizeof(piece->pt) / sizeof(*piece->pt); ++j)
	{
	  struct pt *pt = &piece->pt[j];
	  printf("bloc %d : %d %d %d\n", j + 1, pt->x, pt->y, pt->z);
	}
    }
  exit(0);
}

static void gen_piece_insert(struct piece_pos *piece)
{
  int i;

  for (i = 0; i < nb_all_pieces; ++i)
    {
      struct piece_pos *old_piece = &all_pieces[i];
      int j;

      int diff = 0;
      for (j = 0; j < 15; ++j)
	{
	  if (((char *)old_piece->pt)[j] != ((char *)piece->pt)[j])
	    {
	      ++diff;
	    }
	}
      if (!diff)
	{
	  /* printf("duplicate piece\n"); */
	  return;
	}
    }
  all_pieces = realloc(all_pieces, (nb_all_pieces + 1) * sizeof(*all_pieces));
  all_pieces[nb_all_pieces] = *piece;
  ++nb_all_pieces;
}

static void gen_piece_pos_rot(struct rot *rot)
{
  struct piece_pos piece = one_piece;
  int i, j;

  for (i = 0; i < sizeof(piece.pt) / sizeof(*piece.pt); ++i)
    {
      struct pt *pt = &piece.pt[i];
      for (j = 0; j < rot->x; ++j)
	{
	  char old_x = pt->x;
	  pt->x = pt->y;
	  pt->y = -old_x;
	}
      for (j = 0; j < rot->y; ++j)
	{
	  char old_x = pt->x;
	  pt->x = pt->z;
	  pt->z = -old_x;
	}
      for (j = 0; j < rot->z; ++j)
	{
	  char old_y = pt->y;
	  pt->y = -pt->z;
	  pt->z = old_y;
	}
    }
  gen_piece_insert(&piece);
}

static void gen_piece_pos()
{
  struct rot rot;

  for (rot.x = 0; rot.x <= 4; ++rot.x)
    {
      for (rot.y = 0; rot.y <= 4; ++rot.y)
	{
	  for (rot.z = 0; rot.z <= 4; ++rot.z)
	    {
	      gen_piece_pos_rot(&rot);
	    }
	}
    }
}

static int flag_pt(struct pt *pt, char flag)
{
  /* printf ("flag_pt %d %d %d : %d -> %d\n", */
  /* 	  pt->x, pt->y, pt->z, */
  /* 	  *POS(pt->x, pt->y, pt->z), */
  /* 	  flag); */
  if (pt->x < 0 || pt->x > 4 ||
      pt->y < 0 || pt->y > 4 ||
      pt->z < 0 || pt->z > 4)
    return 0;
  char *p = POS(pt->x, pt->y, pt->z);
  if (*p == flag)
    {
      /* printf("flag already setted %d\n", *p); */
      return 0;
    }
  *p = flag;
  return 1;
}

static void try_pos_piece(struct pt *pt, const struct piece_pos *ro_piece)
{
  struct piece_pos piece = *ro_piece;
  int i;

  /* printf("try\n"); */
  for (i = 1; i < sizeof(piece.pt) / sizeof(*piece.pt); ++i)
    {
      /* printf("i %d\n", i); */
      struct pt *new_pt = &piece.pt[i];

      new_pt->x += pt->x;
      new_pt->y += pt->y;
      new_pt->z += pt->z;
      if (!flag_pt(new_pt, 1))
	break;
    }
  if (i == sizeof(piece.pt) / sizeof(*piece.pt))
    {
      inserted[nb_inserted] = piece;
      ++nb_inserted;
      if (nb_inserted >= 24)
	printf("enter cube %d\n", nb_inserted);
      if (nb_inserted == 25)
	display_inserted();
      try();
      /* printf("leave cube %d\n", nb_inserted); */
      --nb_inserted;
    }
  /* printf ("cancel\n"); */
  while (i-- >= 1)
    {
      struct pt *new_pt = &piece.pt[i];

      flag_pt(new_pt, 0);
    }
}

static void try_pos(struct pt *pt)
{
  int i;

  for (i = 0; i < nb_all_pieces; ++i)
    {
      struct piece_pos *piece = &all_pieces[i];

      /* printf("try_pos_piece at %d %d %d piece %d\n", */
      /* 	     pt->x, pt->y, pt->z, i); */
      /* if (pt->x == 4 && pt->y == 4 && pt->z == 4 && i == 23) */
      /* 	{ */
      /* 	  printf("fail\n"); */
      /* 	  display_cube(); */
      /* 	  myflag = 1; */
      /* 	  exit(1); */
      /* 	} */
      try_pos_piece(pt, piece);
    }
}

static void try()
{
  struct pt pt;

  for (pt.x = 0; pt.x <= 4; ++pt.x)
    {
      for (pt.y = 0; pt.y <= 4; ++pt.y)
	{
	  for (pt.z = 0; pt.z <= 4; ++pt.z)
	    {
	      if (!flag_pt(&pt, 1))
	      	continue;
	      try_pos(&pt);
	      flag_pt(&pt, 0);
	    }
	}
    }
}

int main(int argc, char **argv)
{
  gen_piece_pos();
  printf("nb_all_pieces : %d\n", nb_all_pieces);
  try();
  exit(1);
}
