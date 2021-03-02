#include <stdio.h>
#include <string.h>
#include <libconfig.h>
#include "config.h"

configuration_t configuration;

static void _read_color(config_t *libconfig_handle, const char *path_prefix, int *color)
{
    char path_buf[256];
    char keys[] = "bgra";
    int i;
    int color_tmp;
    const int default_color_val = 0x00;
    *color = 0;
    for (i = 0; i < sizeof(keys)-1; i++) {
        snprintf(path_buf, sizeof(path_buf)-1, "%s.%c", path_prefix, keys[i]);
        if (config_lookup_int(libconfig_handle, path_buf, &color_tmp) != CONFIG_TRUE) {
            fprintf(stderr, "configuration reader: Could not find a valid value at \"%s\". Defaulting to 0x%x.\n", path_buf, default_color_val);
            color_tmp = default_color_val;
        }
        *color |= (color_tmp & 0xff) << i * 8;
    }
}

void read_in_configuration(const char *configuration_file_path)
{
    int ret;
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
        strncpy(&configuration.background.image_path, "/usr/share/logo.png", path_size_minus_one);
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
    configuration.progress_bar.layout.width = 300;
    configuration.progress_bar.layout.height = 20;
    configuration.progress_bar.layout.offset.x = 0;
    configuration.progress_bar.layout.offset.y = 0;
    configuration.progress_bar.colors.background = 0xffffffff;
    configuration.progress_bar.colors.indicator = 0xffcccccc;
    strncpy(configuration.background.image_path, "/usr/share/logo.png", path_size_minus_one);
    configuration.background.image_path[path_size_minus_one] = '\0';
}