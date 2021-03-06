#define _POSIX_C_SOURCE 199309L
#include <time.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define ABS(X)   ((X) >= 0 ? (X) : -(X))
#define BETWEEN(A, LB, UB) ((A) >= (LB) && (A) <= (UB))

#define NELEM(X) (sizeof(X)/sizeof(X[0]))

typedef struct
{
    double * data;
    size_t len;
    size_t cap;
    int nr;
    int nc;
} double_2d_array_t;

typedef struct
{
    int x, y, w, h;
} rect_t;

typedef struct
{
    int r, g, b, a;
} color_t;

#define COLOR_TRANS         {.r =   0, .g =   0, .b =   0, .a =   0}
#define COLOR_BLACK         {.r =   0, .g =   0, .b =   0, .a = 255}
#define COLOR_RED           {.r = 255, .g =   0, .b =   0, .a = 255}
#define COLOR_GREEN         {.r =   0, .g = 255, .b =   0, .a = 255}
#define COLOR_BLUE          {.r =   0, .g =   0, .b = 255, .a = 255}
#define COLOR_YELLOW        {.r = 255, .g = 255, .b =   0, .a = 255}
#define COLOR_MAGENTA       {.r = 255, .g =   0, .b = 255, .a = 255}
#define COLOR_CYAN          {.r =   0, .g = 255, .b = 255, .a = 255}
#define COLOR_WHITE         {.r = 255, .g = 255, .b = 255, .a = 255}

#define COLOR_GRAY          {.r = 128, .g = 128, .b = 128, .a = 255}
#define COLOR_LIGHT_RED     {.r = 255, .g = 128, .b = 128, .a = 255}
#define COLOR_LIGHT_GREEN   {.r = 128, .g = 255, .b = 128, .a = 255}
#define COLOR_LIGHT_BLUE    {.r = 128, .g = 128, .b = 255, .a = 255}
#define COLOR_LIGHT_YELLOW  {.r = 255, .g = 255, .b = 128, .a = 255}
#define COLOR_LIGHT_MAGENTA {.r = 255, .g = 128, .b = 255, .a = 255}
#define COLOR_LIGHT_CYAN    {.r = 128, .g = 255, .b = 255, .a = 255}

#define COLOR_DARK_GRAY     {.r =  64, .g =  64, .b =  64, .a = 255}
#define COLOR_DARK_RED      {.r = 128, .g =  64, .b =  64, .a = 255}
#define COLOR_DARK_GREEN    {.r =  64, .g = 128, .b =  64, .a = 255}
#define COLOR_DARK_BLUE     {.r =  64, .g =  64, .b = 128, .a = 255}
#define COLOR_DARK_YELLOW   {.r = 128, .g = 128, .b =  64, .a = 255}
#define COLOR_DARK_MAGENTA  {.r = 128, .g =  64, .b = 128, .a = 255}
#define COLOR_DARK_CYAN     {.r =  64, .g = 128, .b = 128, .a = 255}

static const color_t color_trans            = COLOR_TRANS          ;
static const color_t color_black            = COLOR_BLACK          ;
static const color_t color_red              = COLOR_RED            ;
static const color_t color_green            = COLOR_GREEN          ;
static const color_t color_blue             = COLOR_BLUE           ;
static const color_t color_yellow           = COLOR_YELLOW         ;
static const color_t color_magenta          = COLOR_MAGENTA        ;
static const color_t color_cyan             = COLOR_CYAN           ;
static const color_t color_white            = COLOR_WHITE          ;

static const color_t color_gray             = COLOR_GRAY           ;
static const color_t color_light_red        = COLOR_LIGHT_RED      ;
static const color_t color_light_green      = COLOR_LIGHT_GREEN    ;
static const color_t color_light_blue       = COLOR_LIGHT_BLUE     ;
static const color_t color_light_yellow     = COLOR_LIGHT_YELLOW   ;
static const color_t color_light_magenta    = COLOR_LIGHT_MAGENTA  ;
static const color_t color_light_cyan       = COLOR_LIGHT_CYAN     ;

static const color_t color_dark_gray        = COLOR_DARK_GRAY      ;
static const color_t color_dark_red         = COLOR_DARK_RED       ;
static const color_t color_dark_green       = COLOR_DARK_GREEN     ;
static const color_t color_dark_blue        = COLOR_DARK_BLUE      ;
static const color_t color_dark_yellow      = COLOR_DARK_YELLOW    ;
static const color_t color_dark_magenta     = COLOR_DARK_MAGENTA   ;
static const color_t color_dark_cyan        = COLOR_DARK_CYAN      ;

static const color_t light_colors[] = {
    COLOR_LIGHT_RED     ,
    COLOR_LIGHT_GREEN   ,
    COLOR_LIGHT_BLUE    ,
    COLOR_LIGHT_MAGENTA ,
    COLOR_CYAN          ,
    COLOR_LIGHT_YELLOW  ,
};

static const color_t colors[] = {
    COLOR_RED           ,
    COLOR_GREEN         ,
    COLOR_BLUE          ,
    COLOR_MAGENTA       ,
    COLOR_DARK_CYAN     ,
    COLOR_YELLOW        ,
};

static const int num_colors = NELEM(colors);

typedef struct
{
    uint8_t * data;
    int nr;
    int nc;
    int comp;
} img_t;

static img_t
img_create(int nr, int nc)
{
    img_t img;
    img.comp = 4;
    img.data = calloc(nr * nc * img.comp, sizeof(*img.data));
    img.nr = nr;
    img.nc = nc;
    return img;
}

// TODO
#if 0
static void
img_fill()
{
}
#endif

static void
img_resize(img_t * img, int nr, int nc)
{
    int r, c;
    int i;
    int old_nr = img->nr;
    int old_nc = img->nc;
    if (nr == old_nr && nc == old_nc)
        return;
    img->nr = nr;
    img->nc = nc;
    uint8_t * new_data = calloc(nr * nc * img->comp, sizeof(*new_data));
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
    assert(BETWEEN(px_idx+3, 0, img.comp * img.nc * img.nr));
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

// TODO: allow for color customization
static void
bar_chart(img_t * img_p, double * data, int nr, int nc, rect_t chart_rect)
{
    int r, c;
    int bar_width = chart_rect.w / (nr*(nc+1)-1);

    double min_bar = 0;
    double max_bar = 0;
    for(r = 0; r < nr; r++) {
        for(c = 0; c < nc; c++) {
            max_bar = MAX(data[r*nc + c], max_bar);
            min_bar = MIN(data[r*nc + c], min_bar);
        }
    }
    double bar_range = max_bar - min_bar;

    int px_row_of_y_eq_zero = chart_rect.h * ABS(min_bar)/bar_range;

    int new_h = MAX(chart_rect.y + chart_rect.h, img_p->nr);
    int new_w = MAX(chart_rect.x + chart_rect.w, img_p->nc);
    img_resize(img_p, new_h, new_w);
    for(r = 0; r < nr; r++) {
        for(c = 0; c < nc; c++) {
            int ci = (nc+1)*r + c;
            color_t fill_color = light_colors[c % num_colors];
            color_t border_color = colors[c % num_colors];
            double datum = data[r*nc + c];
            int bar_height = chart_rect.h * ABS(datum)/bar_range;
            int bar_offset = datum >= 0 ? bar_height : 0;
            rect_t rect = {
                .x = chart_rect.x + bar_width*ci,
                .y = chart_rect.y + chart_rect.h - px_row_of_y_eq_zero - bar_offset,
                .w = bar_width,
                .h = bar_height
            };
            img_rect(*img_p, rect, border_color, fill_color);
        }
    }
}

// TODO: handle invalid files
static double_2d_array_t
read_data_file(const char * filename)
{
    char buffer[1024];
    double_2d_array_t d;
    d.cap = 100;
    d.len = 0;
    d.data = calloc(d.cap, sizeof(*d.data));
    d.nr = 0;
    d.nc = 0;

    FILE * fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("error");
        exit(EXIT_FAILURE);
    }
    double datum;
    int prev_nc = -1;
    int line_num = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        line_num++;
        if (strcmp(buffer, "\n") == 0)
            continue;
        size_t l = strlen(buffer);
        buffer[l-1] = ','; // replace newline with ,
        int nc = 0;
        int nread = 0;
        char * s = buffer;
        while (sscanf(s, "%lf,%n", &datum, &nread) == 1) {
            //printf("%lf %d \"%s\"\n", datum, nread, s);
            d.data[d.len++] = datum;
            if (d.len >= d.cap) {
                d.cap *= 2;
                d.data = realloc(d.data, sizeof(*d.data)*d.cap);
            }
            nc++;
            s += nread;
        }
        //printf("\n");
        d.nr++;
        d.nc = nc;
        if (prev_nc > 0 && prev_nc != nc) {
            fprintf(stderr, "error reading %s:%d: inconsistent number of columns (%d != %d)\n", filename, line_num, prev_nc, nc);
            exit(EXIT_FAILURE);
        }
        prev_nc = nc;
    }
    if (ferror(fp)) {
        perror("error");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    return d;
}

#if 0
static void
print_data_array(double_2d_array_t d)
{
    int r, c;
    for (r = 0; r < d.nr; r++) {
        for (c = 0; c < d.nr; c++)
            printf("%lf ", d.data[d.nc*r + c]);
        printf("\n");
    }
}
#endif

#ifdef TEST
int main_test(int argc, char * argv[])
#else
int main(int argc, char * argv[])
#endif
{
    int w = 50, h = 50;
    img_t img = img_create(h, w);

    if (argc < 4 || strcmp(argv[1], "-o") != 0) {
        fprintf(stderr, "usage: %s -o [OUT] [FILE]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    double_2d_array_t data = read_data_file(argv[3]);

    rect_t chart_rect = {
        .x = 0,
        .y = 0,
        .w = 900,
        .h = 200
    };
    bar_chart(&img, data.data, data.nr, data.nc, chart_rect);

    img_write(img, argv[2]);

    return 0;
}

#ifdef TEST
int main()
{
    struct timespec req = { .tv_sec = 0, .tv_nsec = 100000000 };

    char * argv[] = {"./pkplot", "-o", "out.png", "./data/example1.dat", NULL};
    int argc = NELEM(argv)-1;
    main_test(argc, argv);

    nanosleep(&req, NULL);

    char * argv2[] = {"./pkplot", "-o", "out.png", "./data/example2.dat", NULL};
    int argc2 = NELEM(argv2)-1;
    main_test(argc2, argv2);

    nanosleep(&req, NULL);

    char * argv3[] = {"./pkplot", "-o", "out.png", "./data/example3.dat", NULL};
    int argc3 = NELEM(argv3)-1;
    main_test(argc3, argv3);

    return 0;
}
#endif
