#include <cairo/cairo.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
  double width = 320, height = 240;
  double f, delta = 0.125;
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *crc = cairo_create(surface);
  cairo_text_extents_t extents;
  cairo_font_options_t* font;

  // Set up font  (face is second field in 'xlsfonts'; first field is foundry)
  font = cairo_font_options_create();
  cairo_font_options_set_antialias(font, CAIRO_ANTIALIAS_SUBPIXEL);
  cairo_set_font_options(crc, font);
  cairo_select_font_face(crc, "nimbus sans l", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(crc, 16);
  cairo_font_options_destroy(font);

  // Background
  cairo_rectangle(crc, 0, 0, width, height);
  cairo_set_source_rgb(crc, 1, 1, 1);
  cairo_fill(crc);

  // Fill
  cairo_move_to(crc, 0, 220);
  for (f = -4; f < 4; f += delta) {
    double v = exp(-f * f / 2) / sqrt(2 * M_PI);
    cairo_line_to(crc, width * (f + 4) / 8, 220 - v * height);
  }
  cairo_close_path(crc);
  cairo_set_source_rgb(crc, 122 / 255.0, 143 / 255.0, 248 / 255.0);
  cairo_fill(crc);

  // Graph
  cairo_move_to(crc, 0, 220);
  for (f = -4; f < 4; f += delta) {
    double v = exp(-f * f / 2) / sqrt(2 * M_PI);
    cairo_line_to(crc, width * (f + 4) / 8, 220 - v * height);
  }
  cairo_set_line_width(crc, 2);
  cairo_set_source_rgb(crc, 7 / 255.0, 16 / 255.0, 141 / 255.0);
  cairo_stroke(crc);

  // Vertical Axis
  cairo_set_source_rgb(crc, 0, 0, 0);
  cairo_move_to(crc, width / 2, 0);
  cairo_line_to(crc, width / 2, height);
  cairo_stroke(crc);

  // Horizontal Axis
  cairo_move_to(crc, 0, 220);
  cairo_line_to(crc, width, 220);
  cairo_stroke(crc);

  // Horizontal Ticks
  cairo_set_line_width(crc, 1);
  for (f = -4; f < 4; f++) {
    cairo_move_to(crc, width * (f + 4) / 8, 220 - 5);
    cairo_line_to(crc, width * (f + 4) / 8, 220 + 5);
    cairo_stroke(crc);

    if (f > -4 && f != 0 && f < +4) {
      char str[] = "+3";
      if (f > 0)
	sprintf(str, "%d", (int) f);
      else
	sprintf(str, "%d", (int) f);
      cairo_text_extents(crc, str, &extents);
      cairo_move_to(crc, width * (f + 4) / 8 - extents.width / 2, 220 + 7 + extents.height);
      cairo_show_text(crc, str);
    }
  }

  // Vertical Ticks
  for (f = +1; f >= -1; f -= 0.1) {
    cairo_move_to(crc, width / 2 - 5, 220 + f * height);
    cairo_line_to(crc, width / 2 + 5, 220 + f * height);
    cairo_stroke(crc);
  }

  cairo_text_extents(crc, "0.5", &extents);
  cairo_move_to(crc, width / 2 + 7, 220 - 0.5 * height + extents.height / 2);
  cairo_show_text(crc, "0.5");

  cairo_surface_write_to_png(surface, "gauss.png");
  return 0;
}
