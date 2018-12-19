# msdf-c
A pure C99 multi-channel signed distance field generator.  Handles MSDF bitmap
generation from given ttf/otf font.

---

**This is in an unstable state, ymmv.**

[Based on the C++ implementation by Viktor Chlumský.](https://github.com/Chlumsky/msdfgen)

Current issues:

* ~~Glyph alignment seems off~~ [X]

* Lack of multi-byte character support

* Error correction appears to be wrong (pixel clash)

* Lacking a sensible API, and code is a bit messy