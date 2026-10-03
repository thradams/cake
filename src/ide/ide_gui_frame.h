/*
 * The frame recorder (ide_gui_frame.c): the core's drawing calls go here
 * while it paints; frame_end hands the backend only what changed since the
 * last paint and says which rect to copy to the screen.
 */
#ifndef IDE_GUI_FRAME_H
#define IDE_GUI_FRAME_H

#include "ide_gui_backend.h"

/* One per OS window: what it drew last time, to compare with. */
struct frame_recorder;

struct frame_recorder* frame_create(void);
void frame_free(struct frame_recorder* r);

/* Recording starts; text is measured with `c`'s fonts. */
void frame_begin(struct frame_recorder* r, struct gui_canvas* c);
/* What changed, drawn into the canvas of frame_begin and in *painted; 0
 * when the picture is the same. */
int frame_end(struct frame_recorder* r, int width, int height, struct gui_rect* painted);
/* The next paint draws everything (a new back buffer, a new size). */
void frame_invalidate(struct frame_recorder* r);

void frame_fill_rect(struct frame_recorder* r, int x, int y, int w, int h, uint32_t rgb);
void frame_shade_rect(struct frame_recorder* r, int x, int y, int w, int h, int alpha);
void frame_set_clip(struct frame_recorder* r, int x, int y, int w, int h);
void frame_draw_text(struct frame_recorder* r, int x, int y, const uint32_t* cps, int count,
                     uint32_t fg, uint32_t bg, enum gui_font font);
/* The width frame_draw_text gives the run, px. */
int frame_text_width(struct frame_recorder* r, const uint32_t* cps, int count, enum gui_font font);

#endif
