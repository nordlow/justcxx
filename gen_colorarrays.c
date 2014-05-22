#include "gen_colorarrays.h"

bool g_color_linterpol_flag = TRUE;

void
cGLarray_genSplat(uint32_t * cGL,
                  color_t col,
                  size_t glcolors_stride, size_t glcolorsN)
{
  for (size_t i = 0; i < glcolorsN; i++) {
    cGL[glcolors_stride * i] = color_to_GL_RGBA(col);
  }
}

void
cGLarray_genSplatIO(uint32_t * cGL,
                    color_t colorI, color_t colorO,
                    size_t glcolorsN)
{
  for (size_t i = 0; i < glcolorsN; i++) {
    cGL[2*i + 0] = color_to_GL_RGBA(colorI);
    cGL[2*i + 1] = color_to_GL_RGBA(colorO);
  }
}

void
cGLarray_genCirGrad(const color_t * colorsG, size_t gradN,
                    uint32_t * cGL, size_t glcolors_stride,
                    size_t glcolorsN)
{
  const float ratio = (float)gradN / glcolorsN;
  if (g_color_linterpol_flag) {
    for (size_t i = 0; i < glcolorsN; i++) {
      size_t grad_i = i * gradN / glcolorsN;
      size_t grad_j = (grad_i + 1) % gradN;
      color_t src = colorsG[grad_i];
      color_t dst = colorsG[grad_j];
      float prg = (float)i * ratio - (float)grad_i; /* \TODO: ldexp? */
      color_t curr; color_linterpol(src, prg, dst, &curr);
      cGL[i * glcolors_stride] = color_to_GL_RGBA(curr);
    }
  } else {
    for (size_t i = 0; i < glcolorsN; i++) {
      color_t curr = colorsG[i * gradN / glcolorsN];
      cGL[i * glcolors_stride] = color_to_GL_RGBA(curr);
    }
  }
}

void
cGLarray_genCirGradIO(color_t colorI, color_t colorO,
                      const color_t * colorsGI, size_t numGI,
                      const color_t * colorsGO, size_t numGO,
                      uint32_t * cGL, size_t glcolors_stride,
                      size_t glcolorsN)
{
  if (colorsGI && numGI >= 1) {
    cGLarray_genCirGrad(colorsGI, numGI,
                        &cGL[0], 2, glcolorsN);
  } else {
    cGLarray_genSplat(&cGL[0], colorI, 2, glcolorsN);
  }
  if (colorsGO && numGO >= 1) {
    cGLarray_genCirGrad(colorsGO, numGO,
                        &cGL[1], 2, glcolorsN);
  } else {
    cGLarray_genSplat(&cGL[1], colorO, 2, glcolorsN);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
colorarrayGL_convert_colorarray(uint32_t * cGL,
                                const color_t * colors, size_t glcolorsN)
{
  for (size_t i = 0; i < glcolorsN; i++) {
    cGL[i] = color_to_GL_RGBA(colors[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */
