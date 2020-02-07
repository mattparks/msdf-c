/*
 This is just a simple example usage of the
 msdf generator. It generates a bitmap from the
 character you specify.

 This c file is not intended for real-world use.
 this is purely an example. Embed msdf.c/h into your
 code directly and use however you need to.

 Usage:
 ./msdf_gen 'A' glyph.png

 The resulting bitmap will be stored in glyph.png
 the MSDF bitmap will be stored in msdf_glyph.png.

 Supports multi-byte characters (utf8)
 */

#include "msdf.h"
#include <string.h>
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PX_RANGE 2.0

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint8_t* io_read_file(const char *path, const char *mode)
{
  // load the file
  FILE *file;
  file = fopen(path, mode);
  if (file == NULL) {
    printf("could not load file %s\n", path);
    return NULL;
  }

  // get file length
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  // allocate space for file data
  uint8_t *buff = malloc(size+1);

  // read file contents into buffer
  size_t l = fread(buff, size, 1, file);

  // null-terminate the buffer
  buff[size] = '\0';

  fclose(file);

  return buff;
}

void main(int argc, char **argv)
{
  char fontdefault[] = "font/OpenSans-Regular.ttf";
  int size = 128;

  char *c     = argv[1];
  char *out   = argv[2];
  char *fon   = argv[3];
  char *fontf = !fon ? fontdefault : fon;

  // print help
  if (!c || !out || strcmp(c, "-h") == 0) {
    printf("msdf-c, a C99 multi-channel signed-distance-field generator\n");
    printf("by exezin https://github.com/exezin/msdf-c\n");
    printf("based on https://github.com/Chlumsky/msdfgen\n");
    printf("usage:\n  ./msdf_gen 'A' out.png \"path/to/font.ttf\"\n");

    return;
  }

  // load the ttf (or otf) data
  uint8_t *data = io_read_file(fontf, "rb");
  if (!data) {
    printf("Failed loading font.\n");
    return;
  }

  // process the ttf data
  stbtt_fontinfo font;
  stbtt_InitFont(&font, (const uint8_t*)data, stbtt_GetFontOffsetForIndex(data,0));

  // get baseline metrics
  int ascent, descent;
  stbtt_GetFontVMetrics(&font, &ascent, &descent, 0);

  // Funit to pixel scale
  float scale = stbtt_ScaleForPixelHeight(&font, size);
  int baseline = (int)(ascent*scale);

  // pixel range needs inverting for otf fonts
  // not entirely sure why
  int otf = (strstr(fontf, ".otf")) ? -1 : 1;

  // generate a msdf bitmap
  // ideally you would do this in your shader
  ex_metrics_t metrics;
  float *msdf = ex_msdf_glyph(&font, ex_utf8(c), size, size, &metrics, 0);
  uint8_t *bitmap = malloc(3*size*size);
  uint8_t *bitmap_sdf = malloc(3*size*size);
  memset(bitmap, 0, 3*size*size);
  memset(bitmap_sdf, 0, 3*size*size);
  for (int y=0; y<size; y++) {
    for (int x=0; x<size; x++) {
      size_t index = 3*((y*size)+x);

      float v = MAX(MIN(msdf[index], msdf[index+1]), MIN(MAX(msdf[index], msdf[index+1]), msdf[index+2])) - 0.5;

      float p = 0.;
      for(int i=0; i<4; ++i)
        p += ((PX_RANGE*otf)/size)*size;
      v *= p;
      float a = MAX(0.0, MIN(v + 0.5, 1.0));
      a = sqrt(1.0 * 1.0 * (1.0 - a) + 0.0 * 0.0 * a);

      bitmap[index+0] = 255*a;
      bitmap[index+1] = 255*a;
      bitmap[index+2] = 255*a;

      bitmap_sdf[index+0] = (uint8_t)(255*(msdf[index+0]+size)/size);
      bitmap_sdf[index+1] = (uint8_t)(255*(msdf[index+1]+size)/size);
      bitmap_sdf[index+2] = (uint8_t)(255*(msdf[index+2]+size)/size);
    }
  }

  printf("Glyph metrics for '%s':\n", c);
  printf("left_bearing: %i\n",  metrics.left_bearing);
  printf("advance:      %i\n",  metrics.advance);
  printf("glyph width:  %i\n",  metrics.ix1 - metrics.ix0);
  printf("glyph height: %i\n",  metrics.iy1 - metrics.iy0);
  printf("glyph x:   %i %i\n",  metrics.ix0,  metrics.ix1);
  printf("glyph y:   %i %i\n",  metrics.iy0,  metrics.iy1);
  printf("baseline:     %i\n",  baseline);

  // uncomment to draw a cross over the image
  // for debugging alignment issues
  /*for (int y=0; y<size; y++) {
    int index = 3*((y*size)+size/2);
    bitmap[index+0] = 0;
    bitmap[index+1] = 0;
    bitmap[index+2] = 0;
    index = 3*((size/2*size)+y);
    bitmap[index+0] = 0;
    bitmap[index+1] = 0;
    bitmap[index+2] = 0;
  }*/

  // debug output
  char buff[256];
  sprintf(buff, "msdf_%s", out);
  stbi_write_png(out, size, size, 3, bitmap, size*3);
  stbi_write_png(buff, size, size, 3, bitmap_sdf, size*3);

  free(data);
  free(msdf);
  free(bitmap);
  free(bitmap_sdf);
  printf("Done generating msdf glyph.\n");
}