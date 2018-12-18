/*
 This is just a simple example usage of the
 msdf generator. It generates a bitmap from character
 you specify.

 Not intended for real-world use, this is purely an example.
 Embed msdf.c/h into your code directly and use however
 you need to.

 Usage:
 ./msdf_gen A  

 The resulting bitmap will be stored in msdf.png
 */

#include "msdf.h"
#include <string.h>
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define INF   -1e24
#define RANGE 1.0

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
  char c = argv[1][0];

  // load the ttf data
  uint8_t *data = io_read_file("font/OpenSans-Regular.ttf", "rb");
  if (!data) {
    printf("Failed loading font.\n");
    return;
  }

  // process the ttf data
  stbtt_fontinfo font;
  stbtt_InitFont(&font, (const uint8_t*)data, stbtt_GetFontOffsetForIndex(data,0));

  // generate a msdf bitmap
  int size = 128;
  float *msdf = ex_msdf_glyph(&font, c, size, size);
  uint8_t *bitmap = malloc(3*size*size);
  memset(bitmap, 0, 3*size*size);
  for (int y=0; y<size; y++) {
    for (int x=0; x<size; x++) {
      size_t index = 3*((y*size)+x);

      float v = median(msdf[index], msdf[index+1], msdf[index+2]) - 0.5;
      v *= vec2_mul_inner((vec2){2.0f/size, 2.0f/size}, (vec2){x/0.5, y/0.5});
      float a = MAX(0.0, MIN(v + 0.5, 1.0));
      a = sqrt(1.0 * 1.0 * (1.0 - a) + 0.0 * 0.0 * a);

      bitmap[index]   = 255*a;
      bitmap[index+1] = 255*a;
      bitmap[index+2] = 255*a;

      // uncomment for MSDF msdf
      // bitmap[index]   = (uint8_t)(255*(msdf[index]+size)/size);
      // bitmap[index+1] = (uint8_t)(255*(msdf[index+1]+size)/size);
      // bitmap[index+2] = (uint8_t)(255*(msdf[index+2]+size)/size);
    }
  }

  // debug output
  stbi_write_png("msdf.png", size, size, 3, bitmap, size*3);

  free(data);
  free(msdf);
  free(bitmap);
  printf("Done generating msdf glyph.\n");
}