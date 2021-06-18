#include <stdio.h>
#include <string.h>
#include <libconfig.h>
#include "config.h"

configuration_t configuration;

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

    *color = lv_color_make(r, g, b);
}

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
    if (config_lookup_int(&libconfig_handle, "progress_bar.layout.offset.y", &configuration.progress_bar.layout.offset.y) != CONFIG_TRUE)
        configuration.progress_bar.layout.offset.y = 0;
    _read_color(&libconfig_handle, "progress_bar.colors.background", &configuration.progress_bar.colors.background);
    _read_color(&libconfig_handle, "progress_bar.colors.indicator", &configuration.progress_bar.colors.indicator);
    config_destroy(&libconfig_handle);
    return;
_init_defaults_return:
    fprintf(stderr, "Error reading configuration values from %s. Falling back do defaults.\n", configuration_file_path);
    configuration.progress_bar.layout.width = 300;
    configuration.progress_bar.layout.height = 20;
    configuration.progress_bar.layout.offset.x = 0;
    configuration.progress_bar.layout.offset.y = 0;
    configuration.progress_bar.colors.background = lv_color_hex(0xffffffff);
    configuration.progress_bar.colors.indicator = lv_color_hex(0xffcccccc);
    strncpy(configuration.background.image_path, "/usr/share/logo.png", path_size_minus_one);
    configuration.background.image_path[path_size_minus_one] = '\0';
}