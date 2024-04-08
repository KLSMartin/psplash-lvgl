#include <stdio.h>
#include <string.h>
#include <libconfig.h>
#include "config.h"

configuration_t configuration;

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
    const int default_color_val = 0x00;
    int r, g, b;

    snprintf(path_buf, sizeof(path_buf)-1, "%s.r", path_prefix);
    if (config_lookup_int(libconfig_handle, path_buf, &r) != CONFIG_TRUE) {
        fprintf(stderr, "configuration reader: Could not find a valid value at \"%s\". Defaulting to 0x%x.\n", path_buf, default_color_val);
        r = default_color_val;
    }
    snprintf(path_buf, sizeof(path_buf)-1, "%s.g", path_prefix);
    if (config_lookup_int(libconfig_handle, path_buf, &g) != CONFIG_TRUE) {
        fprintf(stderr, "configuration reader: Could not find a valid value at \"%s\". Defaulting to 0x%x.\n", path_buf, default_color_val);
        g = default_color_val;
    }
    snprintf(path_buf, sizeof(path_buf)-1, "%s.b", path_prefix);
    if (config_lookup_int(libconfig_handle, path_buf, &b) != CONFIG_TRUE) {
        fprintf(stderr, "configuration reader: Could not find a valid value at \"%s\". Defaulting to 0x%x.\n", path_buf, default_color_val);
        b = default_color_val;
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
    if (!configuration_file_path)
        goto _init_defaults_return;
    config_init(&libconfig_handle);
    if (config_read_file(&libconfig_handle, configuration_file_path) != CONFIG_TRUE)
        goto _init_defaults_return;
    if (config_lookup_string(&libconfig_handle, "background.image_path", &background_image_tmp) == CONFIG_TRUE)
        strncpy(configuration.background.image_path, background_image_tmp, path_size_minus_one);
    else
        strncpy(configuration.background.image_path, "/usr/share/logo.png", path_size_minus_one);
    configuration.background.image_path[path_size_minus_one] = '\0';

    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.width", &configuration.progress_bar.layout.width) != CONFIG_TRUE)
        configuration.progress_bar.layout.width = 300;
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.height", &configuration.progress_bar.layout.height) != CONFIG_TRUE)
        configuration.progress_bar.layout.height = 20;
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.offset.x", &configuration.progress_bar.layout.offset.x) != CONFIG_TRUE)
        configuration.progress_bar.layout.offset.x = 0;
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.offset.y", &configuration.progress_bar.layout.offset.y) != CONFIG_TRUE)
        configuration.progress_bar.layout.offset.y = 0;
    _read_color(&libconfig_handle, "progress_bar.colors.background", &configuration.progress_bar.colors.background);
    _read_color(&libconfig_handle, "progress_bar.colors.background_border", &configuration.progress_bar.colors.background_border);
    _read_color(&libconfig_handle, "progress_bar.colors.indicator", &configuration.progress_bar.colors.indicator);
    _read_color(&libconfig_handle, "progress_bar.colors.indicator_border", &configuration.progress_bar.colors.indicator_border);
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.indicator.border_width", &configuration.progress_bar.layout.indicator.border_width) != CONFIG_TRUE)
        configuration.progress_bar.layout.indicator.border_width = 0;
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.indicator.radius", &configuration.progress_bar.layout.indicator.radius) != CONFIG_TRUE)
        configuration.progress_bar.layout.indicator.radius = 0;

    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.background.border_width", &configuration.progress_bar.layout.background.border_width) != CONFIG_TRUE)
        configuration.progress_bar.layout.background.border_width = 0;
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.background.padding", &configuration.progress_bar.layout.background.padding) != CONFIG_TRUE)
        configuration.progress_bar.layout.background.padding = 0;
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.background.radius", &configuration.progress_bar.layout.background.radius) != CONFIG_TRUE)
        configuration.progress_bar.layout.background.radius = 0;
    config_destroy(&libconfig_handle);
    dump_config();
    return;
_init_defaults_return:
    fprintf(stderr, "Error reading configuration values from %s. Falling back do defaults.\n", configuration_file_path);
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
    strncpy(configuration.background.image_path, "/usr/share/logo.png", path_size_minus_one);
    configuration.background.image_path[path_size_minus_one] = '\0';
}
