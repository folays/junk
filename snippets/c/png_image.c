#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <png.h>

int main(int argc, char **argv)
{
  unsigned width = 1024;
  unsigned height = 768;
  unsigned int y, x;

  // don't know why we need all these:
  png_byte bit_depth = '\b';
  png_byte color_type = '\006';
  png_byte** row_pointers;

  // initializing the row_pointers:
  row_pointers = malloc(sizeof(*row_pointers) * height);
  for (y = 0; y < height; ++y)
    row_pointers[y] = malloc(sizeof(**row_pointers) * width * 4);

  // fill all black
  png_byte* row;
  png_byte* ptr;
  for (y = 0; y < height; ++y)
    {
      row = row_pointers[y];
      for (x = 0; x < width; ++x)
	{
	  ptr = &(row[x * 4]);
	  ptr[0] = 128; // red
	  ptr[1] = 64;   // green
	  ptr[2] = 130;   // blue
	  ptr[3] = 127; // alpha
	}
    }
  // ---------------
  // writing the png
  // ---------------

  // these 2 are needed for writing
  png_structp png_ptr;
  png_infop info_ptr;

  FILE *fp = fopen("a.png", "w");
  if (!fp)
    err(1, "fopen");

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    err(1, "png_ptr");

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    err(1, "png_create_info_struct");

  if (setjmp(png_jmpbuf(png_ptr)))
    err(1, "setjmp #1");
  png_init_io(png_ptr, fp);
  // write header
  if (setjmp(png_jmpbuf(png_ptr)))
    err(1, "setjmp #2");

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  // write bytes
  if (setjmp(png_jmpbuf(png_ptr)))
    err(1, "setjmp #3");

  png_write_image(png_ptr, row_pointers);

  // write end
  if (setjmp(png_jmpbuf(png_ptr)))
    err(1, "setjmp #4");

  png_write_end(png_ptr, NULL);

  // -------
  // cleanup
  // -------
  for (y = 0; y < height; ++y)
    free(row_pointers[y]);
  free(row_pointers);

  fclose(fp);

  return 0;
}
