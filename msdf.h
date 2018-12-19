/* msdf
  Handles multi-channel signed distance field bitmap
  generation from given ttf (stb_truetype.h) font.

  Depends on stb_truetype.h to load the ttf file.

  This is in an unstable state, ymmv.

  Based on the C++ implementation by Viktor Chlumský.
  https://github.com/Chlumsky/msdfgen

  Current issues:

   *Glyph alignment seems off
   *Lack of multi-byte character support
   *Error correction appears to be wrong (pixel clash)
*/

#ifndef EX_MSDF_H
#define EX_MSDF_H

#include "stb_truetype.h"
#include "inttypes.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef float vec2[2];
typedef float vec3[3];

typedef struct {
  double dist;
  double d;
} signed_distance_t;

// the possible types:
// STBTT_vmove  = start of a contour
// STBTT_vline  = linear segment
// STBTT_vcurve = quadratic segment
// STBTT_vcubic = cubic segment
typedef struct {
  int color;
  vec2 p[4];
  int type;
} edge_segment_t;

// defines what color channel an edge belongs to
typedef enum {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7
} edge_color_t;

static inline double median(double a, double b, double c)
{
  return MAX(MIN(a, b), MIN(MAX(a, b), c));
}

static inline int nonzero_sign(double n)
{
  return 2*(n > 0)-1;
}

static inline double cross(vec2 a, vec2 b)
{
  return a[0]*b[1] - a[1]*b[0];
}

static inline void vec2_scale(vec2 r, vec2 const v, float const s)
{
  int i;
  for(i=0; i<2; ++i)
    r[i] = v[i] * s;
}

static inline float vec2_mul_inner(vec2 const a, vec2 const b)
{
  float p = 0.;
  int i;
  for(i=0; i<2; ++i)
    p += b[i]*a[i];
  return p;
}

static inline float vec2_len(vec2 const v)
{
  return sqrtf(vec2_mul_inner(v,v));
}

static inline void vec2_norm(vec2 r, vec2 const v)
{
  float k = 1.0 / vec2_len(v);
  vec2_scale(r, v, k);
}

static inline void vec2_sub(vec2 r, vec2 const a, vec2 const b)
{
  int i;
  for(i=0; i<2; ++i)
    r[i] = a[i] - b[i];
}

/*
  Generates a bitmap from the specified character (c)
  Bitmap is a 3-channel float array (3*w*h)
 */
float* ex_msdf_glyph(stbtt_fontinfo *font, uint32_t c, size_t w, size_t h);

static inline uint32_t ex_utf8(const char *c) {
  uint32_t val = 0;

  if ((c[0] & 0xF8) == 0xF0) {
    // 4 byte
    val |= (c[3] & 0x3F);
    val |= (c[2] & 0x3F) << 6;
    val |= (c[1] & 0x3F) << 12;
    val |= (c[0] & 0x07) << 18;
  }
  else if ((c[0] & 0xF0) == 0xE0) {
    // 3 byte
    val |= (c[2] & 0x3F);
    val |= (c[1] & 0x3F) << 6;
    val |= (c[0] & 0x0F) << 12;
  }
  else if ((c[0] & 0xE0) == 0xC0) {
    // 2 byte
    val |= (c[1] & 0x3F);
    val |= (c[0] & 0x1F) << 6;
  }
  else {
    // 1 byte
    val = c[0];
  }

  return val;
}

#endif // EX_MSDF_H