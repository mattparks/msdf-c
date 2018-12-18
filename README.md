# msdf-c
A pure C99 multi-channel signed distance field generator.  Handles multi-channel signed distance field bitmap
generation from given ttf/otf font.

---

**This is in an unstable state, ymmv.**

Based on the C++ implementation by Viktor Chlumský.

https://github.com/Chlumsky/msdfgen

Depends:

* stb_truetype.h

Current issues:

* Glyph alignment seems off

* Lack of multi-byte character support

* Error correction appears to be wrong (pixel clash)

* Lacking a sensible API, and code is a bit messy