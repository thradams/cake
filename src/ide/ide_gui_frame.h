/*
 * The frame recorder (ide_gui_frame.c): the core's drawing calls go here
 * while it paints; frame_end hands the backend only what changed since the
 * last paint and says which rect to copy to the screen.
 */
#ifndef IDE_GUI_FRAME_H
#define IDE_GUI_FRAME_H

#include "ide_gui_backend.h"

void frame_begin(void);
/* What changed, drawn and in *painted; 0 when the picture is the same. */
int frame_end(struct gui_canvas* c, int width, int height, struct gui_rect* painted);
/* The next paint draws everything (a new back buffer, a new size). */
void frame_invalidate(void);

void frame_fill_rect(struct gui_canvas* c, int x, int y, int w, int h, uint32_t rgb);
void frame_shade_rect(struct gui_canvas* c, int x, int y, int w, int h, int alpha);
void frame_set_clip(struct gui_canvas* c, int x, int y, int w, int h);
void frame_draw_text(struct gui_canvas* c, int x, int y, const uint32_t* cps, int count,
                     uint32_t fg, uint32_t bg, enum gui_font font);

#endif
