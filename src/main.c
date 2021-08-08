#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define NELEM(X) (sizeof(X)/sizeof(X[0]))

typedef struct
{
    int x, y, w, h;
} rect_t;

typedef struct
{
    int r, g, b, a;
} color_t;

static const color_t color_trans    = {.r =   0, .g =   0, .b =   0, .a =   0};
static const color_t color_black    = {.r =   0, .g =   0, .b =   0, .a = 255};
static const color_t color_red      = {.r = 255, .g =   0, .b =   0, .a = 255};
static const color_t color_green    = {.r =   0, .g = 255, .b =   0, .a = 255};
static const color_t color_blue     = {.r =   0, .g =   0, .b = 255, .a = 255};
static const color_t color_yellow   = {.r = 255, .g = 255, .b =   0, .a = 255};
static const color_t color_magenta  = {.r = 255, .g =   0, .b = 255, .a = 255};
static const color_t color_cyan     = {.r =   0, .g = 255, .b = 255, .a = 255};
static const color_t color_white    = {.r = 255, .g = 255, .b = 255, .a = 255};

static const color_t colors[] = {
    color_red,
    color_green,
    color_blue,
    color_cyan,
    color_magenta,
    color_yellow,
};

static const size_t num_colors = NELEM(colors);

typedef struct
{
    uint8_t * data;
    size_t nr;
    size_t nc;
    int comp;
} img_t;

static img_t
img_create(size_t nr, size_t nc)
{
    img_t img;
    img.comp = 4;
    img.data = calloc(nr * nc * img.comp, sizeof(*img.data));
    img.nr = nr;
    img.nc = nc;
    return img;
}

// TODO
static void
img_fill()
{
}

static void
img_resize(img_t * img, size_t nr, size_t nc)
{
    size_t r, c;
    int i;
    int old_size = img->nr * img->nc * img->comp;
    int new_size = nr * nc * img->comp;
    size_t old_nr = img->nr;
    size_t old_nc = img->nc;
    if (nr == old_nr && nc == old_nc)
        return;
    img->nr = nr;
    img->nc = nc;
    uint8_t * new_data = calloc(new_size, sizeof(*new_data));
    /* copy data from old img to new img (pixels have moved) */
    for (r = 0; r < MIN(old_nr, nr); r++) {
        for (c = 0; c < MIN(old_nc, nc); c++) {
            int new_px_idx = img->comp * (r*nc + c);
            int old_px_idx = img->comp * (r*old_nc + c);
            for (i = 0; i < img->comp; i++)
                new_data[new_px_idx+i] = img->data[old_px_idx+i];
        }
    }
    free(img->data);
    img->data = new_data;
}

static void
img_set_px(img_t img, int row, int col, color_t color)
{
    int px_idx = img.comp * (row*img.nc + col);
    // TODO: handle case where color has fewer than 4 components
    img.data[px_idx+0] = color.r;
    img.data[px_idx+1] = color.g;
    img.data[px_idx+2] = color.b;
    img.data[px_idx+3] = color.a;
}

static int
img_write(img_t img, const char * filename)
{
    return stbi_write_png(filename, img.nc, img.nr, img.comp, img.data, img.nc*img.comp);
}

static void
img_rect(img_t img, rect_t rect, color_t border, color_t fill)
{
    int r, c;
    // fill
    for (r = rect.y; r < rect.y + rect.h; r++)
        for (c = rect.x; c < rect.x + rect.w; c++)
            img_set_px(img, r, c, fill);
    // vertical borders
    for (r = rect.y; r < rect.y + rect.h; r++) {
        img_set_px(img, r, rect.x, border);
        img_set_px(img, r, rect.x + rect.w-1, border);
    }
    // horizontal borders
    for (c = rect.x; c < rect.x + rect.w; c++) {
        img_set_px(img, rect.y, c, border);
        img_set_px(img, rect.y + rect.h-1, c, border);
    }
}

static void
bar(img_t img, int x, int y, int bar_idx, int bar_width, int bar_height, color_t color)
{
}

// TODO: allow for color customization
static void
bar_chart(img_t * img_p, int * data, size_t nr, size_t nc, rect_t chart_rect)
{
    size_t r, c;
    int bar_width = chart_rect.w / (nr*(nc+1)-1);

    int max_bar = 0;
    for(r = 0; r < nr; r++)
        for(c = 0; c < nc; c++)
            max_bar = MAX(data[r*nc + c], max_bar);

    int new_h = MAX(chart_rect.y + chart_rect.h, img_p->nr);
    int new_w = MAX(chart_rect.x + chart_rect.w, img_p->nc);
    img_resize(img_p, new_h, new_w);
    for(r = 0; r < nr; r++) {
        for(c = 0; c < nc; c++) {
            int ci = (nc+1)*r + c;
            color_t color = colors[c % num_colors];
            int bar_height = chart_rect.h * data[r*nc + c]/max_bar;
            rect_t rect = {.x = chart_rect.x + bar_width*ci, .y = chart_rect.y + chart_rect.h - bar_height, .w = bar_width, .h = bar_height};
            img_rect(*img_p, rect, color, color);
            bar(*img_p, 10, 200, ci, 50, data[r*nc + c], color);
        }
        if (r == nr-1)
            break;
        rect_t rect = {.x = 10 + bar_width*((nc+1)*(r+1)-1), .y = 200, .w = bar_width, .h = 0};
        img_rect(*img_p, rect, color_trans, color_trans);
    }
}

int main()
{
    int w = 50, h = 50;
    img_t img = img_create(h, w);

    int data[][4] = {
        { 10, 10,  7,  15 },
        { 20, 15, 12,  10 },
        { 30,  5, 30,   8 }
    };

    size_t nr = NELEM(data);
    size_t nc = NELEM(data[0]);
    rect_t chart_rect = {.x = 10, .y = 5, .w = 900, .h = 200};
    bar_chart(&img, (int *) data, nr, nc, chart_rect);

    img_write(img, "foo.png");

    return 0;
}
