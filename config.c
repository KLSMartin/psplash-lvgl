/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <libconfig.h>
#include "config.h"

configuration_t configuration = {0};

static uint8_t clip_color(int val)
{
    if (val < 0) {
        return 0;
    } else if (val > 255) {
        return 255;
    }
    return (uint8_t)val;
}

static void _read_color(config_t *libconfig_handle, const char *path_prefix, lv_color_t *color)
{
    char path_buf[256];
    int r, g, b;

    snprintf(path_buf, sizeof(path_buf)-1, "%s.r", path_prefix);
    if (config_lookup_int(libconfig_handle, path_buf, &r) != CONFIG_TRUE) {
        return;
    }
    snprintf(path_buf, sizeof(path_buf)-1, "%s.g", path_prefix);
    if (config_lookup_int(libconfig_handle, path_buf, &g) != CONFIG_TRUE) {
        return;
    }
    snprintf(path_buf, sizeof(path_buf)-1, "%s.b", path_prefix);
    if (config_lookup_int(libconfig_handle, path_buf, &b) != CONFIG_TRUE) {
        return;
    }

    *color = lv_color_make(clip_color(r), clip_color(g), clip_color(b));
}

#ifdef DEBUG
static void dump_config(void)
{
    printf("Color depth: %d\n", LV_COLOR_DEPTH);
    printf("image_path: %s\n", configuration.background.image_path);
    printf("progress.layout.width: %d\n", configuration.progress_bar.layout.width);
    printf("progress.layout.height: %d\n", configuration.progress_bar.layout.height);
    printf("progress.layout.offset.x: %d\n", configuration.progress_bar.layout.offset.x);
    printf("progress.layout.offset.y: %d\n", configuration.progress_bar.layout.offset.y);
    printf("progress.layout.background.padding: %d\n", configuration.progress_bar.layout.background.padding);
    printf("progress.layout.background.border_width: %d\n", configuration.progress_bar.layout.background.border_width);
    printf("progress.layout.background.radius: %d\n", configuration.progress_bar.layout.background.radius);
    printf("progress.layout.indicator.border_width: %d\n", configuration.progress_bar.layout.indicator.border_width);
    printf("progress.layout.indicator.radius: %d\n", configuration.progress_bar.layout.indicator.radius);
    #define _COLOR(_memb) configuration.progress_bar.colors. _memb . ch.red, configuration.progress_bar.colors. _memb . ch.green, configuration.progress_bar.colors. _memb . ch.blue
    printf("progress.layout.colors.background: %d %d %d\n", _COLOR(background));
    printf("progress.layout.colors.background_border: %d %d %d\n", _COLOR(background_border));
    printf("progress.layout.colors.indicator: %d %d %d\n", _COLOR(indicator));
    printf("progress.layout.colors.indicator_border: %d %d %d\n", _COLOR(indicator_border));
    #undef _COLOR
}
#else
#define dump_config() do {} while (0)
#endif

void read_in_configuration(const char *configuration_file_path)
{
    config_t libconfig_handle;
    const char *background_image_tmp;
    const size_t path_size_minus_one = sizeof(configuration.background.image_path) - 1;

    configuration.background.image_path[0] = '\0';
    strncpy(configuration.background.image_path, "/usr/share/logo.png", path_size_minus_one);
    configuration.progress_bar.layout.width = 300;
    configuration.progress_bar.layout.height = 20;
    configuration.progress_bar.layout.offset.x = 0;
    configuration.progress_bar.layout.offset.y = 0;
    configuration.progress_bar.layout.background.border_width = 1;
    configuration.progress_bar.layout.background.padding = 1;
    configuration.progress_bar.layout.background.radius = 0;
    configuration.progress_bar.layout.indicator.border_width = 0;
    configuration.progress_bar.layout.indicator.radius = 0;
    configuration.progress_bar.colors.background = lv_color_hex(0xffffffff);
    configuration.progress_bar.colors.background_border = lv_color_hex(0xffffffff);
    configuration.progress_bar.colors.indicator = lv_color_hex(0xffcccccc);
    configuration.progress_bar.colors.indicator_border = lv_color_hex(0xffcccccc);

    if (!configuration_file_path)
        goto _init_err;
    config_init(&libconfig_handle);
    if (config_read_file(&libconfig_handle, configuration_file_path) != CONFIG_TRUE)
        goto _init_err;
    if (config_lookup_string(&libconfig_handle, "background.image_path", &background_image_tmp) == CONFIG_TRUE) {
        if (background_image_tmp[0] == '/') {
            strncpy(configuration.background.image_path, background_image_tmp, path_size_minus_one);
        /* relative paths are relative to config file */
        } else if (background_image_tmp[0] != '\0') {
            char *configuration_file_path_copy, *configuration_file_dir;

            configuration_file_path_copy = strdup(configuration_file_path);

            if (configuration_file_path_copy) {
                configuration_file_dir = realpath(dirname(configuration_file_path_copy), NULL);

                if (configuration_file_dir && strlen(configuration_file_dir) + strlen(background_image_tmp) + 1 < path_size_minus_one) {
                    snprintf(configuration.background.image_path, path_size_minus_one, "%s/%s", configuration_file_dir, background_image_tmp);
                }
                free(configuration_file_path_copy);
                free(configuration_file_dir);
            }
        }
        configuration.background.image_path[path_size_minus_one] = '\0';
    }

    config_lookup_int(&libconfig_handle, "progress_bar.layout.width", &configuration.progress_bar.layout.width);
    config_lookup_int(&libconfig_handle, "progress_bar.layout.height", &configuration.progress_bar.layout.height);
    config_lookup_int(&libconfig_handle, "progress_bar.layout.offset.x", &configuration.progress_bar.layout.offset.x);
    config_lookup_int(&libconfig_handle, "progress_bar.layout.offset.y", &configuration.progress_bar.layout.offset.y);

    _read_color(&libconfig_handle, "progress_bar.colors.background", &configuration.progress_bar.colors.background);
    _read_color(&libconfig_handle, "progress_bar.colors.background_border", &configuration.progress_bar.colors.background_border);
    _read_color(&libconfig_handle, "progress_bar.colors.indicator", &configuration.progress_bar.colors.indicator);
    _read_color(&libconfig_handle, "progress_bar.colors.indicator_border", &configuration.progress_bar.colors.indicator_border);

    config_lookup_int(&libconfig_handle, "progress_bar.layout.indicator.border_width", &configuration.progress_bar.layout.indicator.border_width);
    config_lookup_int(&libconfig_handle, "progress_bar.layout.indicator.radius", &configuration.progress_bar.layout.indicator.radius);

    config_lookup_int(&libconfig_handle, "progress_bar.layout.background.border_width", &configuration.progress_bar.layout.background.border_width);
    config_lookup_int(&libconfig_handle, "progress_bar.layout.background.padding", &configuration.progress_bar.layout.background.padding);
    config_lookup_int(&libconfig_handle, "progress_bar.layout.background.radius", &configuration.progress_bar.layout.background.radius);
    config_destroy(&libconfig_handle);
    dump_config();

    return;

_init_err:

    fprintf(stderr, "Error reading configuration values from %s. Using defaults.\n", configuration_file_path);
}
