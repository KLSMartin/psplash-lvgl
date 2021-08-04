#pragma once

#include <stdint.h>
#include "lvgl.h"

typedef struct {
    struct {
        struct {
            struct {
                int x, y;
            } offset;
            struct {
                int border_width;
                int padding;
            } background;
            struct {
                int border_width;
                int padding;
            } indicator;
            int width, height;
        } layout;
        struct {
            lv_color_t background;
            lv_color_t background_border;
            lv_color_t indicator;
            lv_color_t indicator_border;
        } colors;
    } progress_bar;
    struct {
        char image_path[512]; // PATH_MAX is runtime (i.e. fs) dependend. Deriving a runtime value is not worth the effort.
    } background;
} configuration_t;

extern configuration_t configuration;

void read_in_configuration(const char *configuration_file_path);