/*
 *  psplash-lvgl
 *
 *  Copyright (c) 2021 Leif Middelschulte <leif.middelschulte@klsmartin.com>
 *
 *  Parts of this file ( fifo handling ) based on 'usplash' copyright
 *  Matthew Garret.
 *  Other parts are based on psplash-fb.c by Matthew Allum
 *  Copyright (c) 2006 Matthew Allum <mallum@o-hand.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

/* === Includes ============================================================= */
/* standard includes */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>

/* lvgl includes */
#include "lvgl.h"
#include "lv_png.h"

#include "drm.h"
#include "monitor.h"
#include "fbdev.h"

/* Project Includes */
#include "config.h"

/* === Defines ======================================================= */
#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define DBG(x, ...) \
   { printf ( __FILE__ ":%d,%s() " x "\n", __LINE__, __func__, ##__VA_ARGS__); }
#else
#define DBG(x, ...) do {} while (0)
#endif

/* === Typedefs ======================================================= */
typedef struct
{
  int progress;
  struct
  {
    lv_task_t *update_task;
    struct {
      lv_style_t bg;
      lv_style_t indicator;
    } styles;
    lv_obj_t *bar;
  } ui;
} progress_indicator_data_t;

/* === Static members ======================================================= */
static const char * const PSPLASH_FIFO = "psplash_fifo";

/* work around lv_drivers/disp inconsistencies between drm and fb backends */
static struct {
#if USE_FBDEV
  uint32_t
#elif USE_DRM
  lv_coord_t
#else
  int
#endif
  width, height;
} display_size;

static progress_indicator_data_t progress_indicator_data;

/* === Private function prototypes ========================================== */
static void update_ui(void)
{
  int16_t current_value = lv_bar_get_value(progress_indicator_data.ui.bar);
  if (current_value == progress_indicator_data.progress)
  {
    return;
  }
  lv_bar_set_value(progress_indicator_data.ui.bar, progress_indicator_data.progress, LV_ANIM_ON);
}

static lv_obj_t *background_create(lv_obj_t *parent)
{
  lv_png_init();
  lv_obj_t *image = lv_img_create(parent, NULL);
  lv_img_set_src(image, configuration.background.image_path);
  lv_obj_align(image, NULL, LV_ALIGN_CENTER, 0, 0);
  return image;
}

/*
  Desired layout:
  +-----------------------------------------------+
  |#########                                      |
  +-----------------------------------------------+
*/
static lv_obj_t *interactive_progress_bar_create(lv_obj_t *parent, progress_indicator_data_t *data)
{
  background_create(parent);
  lv_obj_t *bar = lv_bar_create(parent, NULL);
  data->ui.bar = bar;

  // use custom style
  lv_style_init(&progress_indicator_data.ui.styles.bg);
  lv_style_set_bg_color(&progress_indicator_data.ui.styles.bg, LV_STATE_DEFAULT, configuration.progress_bar.colors.background);
  lv_style_set_bg_opa(&progress_indicator_data.ui.styles.bg, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_style_set_pad_all(&progress_indicator_data.ui.styles.bg, LV_STATE_DEFAULT, configuration.progress_bar.layout.background.padding);
  lv_style_set_border_width(&progress_indicator_data.ui.styles.bg, LV_STATE_DEFAULT, configuration.progress_bar.layout.background.border_width);
  lv_style_set_border_color(&progress_indicator_data.ui.styles.bg, LV_STATE_DEFAULT, configuration.progress_bar.colors.background_border);
  lv_style_set_radius(&progress_indicator_data.ui.styles.bg, LV_STATE_DEFAULT, configuration.progress_bar.layout.background.radius);
  lv_obj_add_style(bar, LV_BAR_PART_BG, &progress_indicator_data.ui.styles.bg);
  lv_style_init(&progress_indicator_data.ui.styles.indicator);
  lv_style_set_bg_color(&progress_indicator_data.ui.styles.indicator, LV_STATE_DEFAULT, configuration.progress_bar.colors.indicator);
  lv_style_set_bg_opa(&progress_indicator_data.ui.styles.indicator, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_style_set_border_width(&progress_indicator_data.ui.styles.indicator, LV_STATE_DEFAULT, configuration.progress_bar.layout.indicator.border_width);
  lv_style_set_border_color(&progress_indicator_data.ui.styles.indicator, LV_STATE_DEFAULT, configuration.progress_bar.colors.indicator_border);
  lv_style_set_radius(&progress_indicator_data.ui.styles.indicator, LV_STATE_DEFAULT, configuration.progress_bar.layout.indicator.radius);
  lv_obj_add_style(bar, LV_BAR_PART_INDIC, &progress_indicator_data.ui.styles.indicator);

  lv_obj_set_size(bar, configuration.progress_bar.layout.width, configuration.progress_bar.layout.height);
  lv_obj_align(bar, NULL, LV_ALIGN_IN_BOTTOM_MID, configuration.progress_bar.layout.offset.x, configuration.progress_bar.layout.offset.y);
  // normalize scale [0 to 100]
  lv_bar_set_range(bar, 0, 100);
  lv_bar_set_start_value(bar, 0, LV_ANIM_OFF);
  return bar;
}

static void *ui_update_thread_cb(void *data)
{
  (void)data;

  while (1) {
    lv_tick_inc(1);
    lv_task_handler();
    update_ui();
    usleep(1000);
  }
  return NULL;
}



/***************************************************************************/ /**
 * @brief  Initialises LittlevGL and registers drivers
 ******************************************************************************/
static void init_lvgl()
{
  static lv_disp_drv_t disp_drv;
  static lv_disp_buf_t disp_buf;
  static lv_disp_t *disp;
  static lv_color_t *buf_1;
  char buf[128] = "unspecified";

  /* LittlevGL init */
  lv_init();
  /* Linux frame buffer device init */
#if USE_MONITOR
  sprintf(buf, "SDL2");
  monitor_init();
  display_size.width = LV_HOR_RES_MAX;
  display_size.height = LV_VER_RES_MAX;
#endif
#if USE_DRM
  sprintf(buf, "DRM");
  drm_init();
  drm_get_sizes(&display_size.width, &display_size.height, NULL);
#endif
#if USE_FBDEV
  sprintf(buf, "FB");
  fbdev_init();
  fbdev_get_sizes(&display_size.width, &display_size.height);
#endif
  printf("Initialized backend: %s\n", buf);

  buf_1 = malloc(display_size.width * display_size.height * sizeof(*buf_1));
  if (!buf_1) {
    fprintf(stderr, "%s\n", strerror(ENOMEM));
    exit(1);
  }

  /*Initialize `disp_buf` with the buffer(s) */
  lv_disp_buf_init(&disp_buf, buf_1, NULL, display_size.width * display_size.height);

  /* Add a display the LittlevGL using the frame buffer driver */
  lv_disp_drv_init(&disp_drv);
#if USE_MONITOR
  disp_drv.flush_cb = monitor_flush; /*It flushes the internal graphical buffer to the frame buffer*/
#endif
#if USE_DRM
  disp_drv.flush_cb = drm_flush; /*It flushes the internal graphical buffer to the drm device*/
#endif
#if USE_FBDEV
  disp_drv.flush_cb = fbdev_flush; /*It flushes the internal graphical buffer to the drm device*/
#endif
  disp_drv.buffer = &disp_buf;

  /* propaget the runtime determined screen size */
  disp_drv.hor_res = display_size.width;
  disp_drv.ver_res = display_size.height;
  printf("Display size %dx%d\n", disp_drv.hor_res, disp_drv.ver_res);
  disp = lv_disp_drv_register(&disp_drv);
  lv_disp_set_default(disp);
}

/***************************************************************************/ /**
* @brief Entry function of the program
* @return does not return
*
* Takes no arguments
******************************************************************************/
static void ui_create()
{
  interactive_progress_bar_create(lv_scr_act(), &progress_indicator_data);
}

void psplash_draw_msg(const char *msg)
{
#if !DEBUG
  (void)msg;
#endif
  DBG("displaying '%s'\n", msg);
}

void psplash_draw_progress(int value)
{
  progress_indicator_data.progress = value;
  DBG("value: %i\n", value);
}

static int
parse_command(char *string)
{
  char *command;

  DBG("got cmd %s", string);

  if (strcmp(string, "QUIT") == 0)
    return 1;

  command = strtok(string, " ");

  if (!strcmp(command, "PROGRESS"))
  {
    char *arg = strtok(NULL, "\0");
    if (arg)
    {
      char *endptr = NULL;
      long int val;
      errno = 0;    /* To distinguish success/failure after call */
      val = strtol(arg, &endptr, 10);
      if ((errno == 0) && endptr && (*endptr == '\0') && (val >= 0) && (val <= 100)) {
        psplash_draw_progress(val);
      }
    }
  }
  else if (!strcmp(command, "MSG"))
  {
    char *arg = strtok(NULL, "\0");

    if (arg)
      psplash_draw_msg(arg);
  }
  else if (!strcmp(command, "QUIT"))
  {
    return 1;
  }

  return 0;
}

void psplash_main(int pipe_fd)
{
  int err;
  ssize_t length = 0;
  fd_set descriptors;
  char *end;
  char *cmd;
  char command[2048];

  FD_ZERO(&descriptors);
  FD_SET(pipe_fd, &descriptors);

  end = command;

  while (1)
  {
    err = select(pipe_fd + 1, &descriptors, NULL, NULL, NULL);

    if (err <= 0)
    {
      /*
	  if (errno == EINTR)
	    continue;
	  */
      return;
    }

    length += read(pipe_fd, end, sizeof(command) - (end - command));

    if (length == 0)
    {
      /* Reopen to see if there's anything more for us */
      close(pipe_fd);
      pipe_fd = open(PSPLASH_FIFO, O_RDONLY | O_NONBLOCK);
      goto out;
    }

    cmd = command;
    do
    {
      int cmdlen;
      char *cmdend = memchr(cmd, '\n', length);

      /* Replace newlines with string termination */
      if (cmdend)
        *cmdend = '\0';

      cmdlen = strnlen(cmd, length);

      /* Skip string terminations */
      if (!cmdlen && length)
      {
        length--;
        cmd++;
        continue;
      }

      if (parse_command(cmd))
        return;

      length -= cmdlen;
      cmd += cmdlen;
    } while (length);

  out:
    end = &command[length];

    FD_ZERO(&descriptors);
    FD_SET(pipe_fd, &descriptors);
  }

  return;
}

/**
 * @brief Notify systemd deamon, in case service is started with type=notify
 */
static int sdnotify_ready(void)
{
  int ret;
  const char *sock_path;
  size_t plen;
  struct sockaddr_un sockaddr = {
      .sun_family = AF_UNIX,
      .sun_path = { 0 },
  };
  size_t sockaddr_sz = offsetof(struct sockaddr_un, sun_path);

  static int notify_socket = -EBADF;

  /* if this does not exist, not started via system or not type=notify */
  sock_path = getenv("NOTIFY_SOCKET");
  if (!sock_path)
    return 0;

  if (((*sock_path != '/') && (*sock_path != '@')))
    return -EPROTO;

  plen = strlen(sock_path);
  if (plen < 2)
    return -EINVAL;

  if ((plen + 1) > sizeof(sockaddr.sun_path))
    return -ENAMETOOLONG;

  if (*sock_path == '/') {
    /* filesystem socket */

    /* copy *with* trailing NUL byte */
    memcpy(sockaddr.sun_path, sock_path, plen + 1);

    /* including trailing NUL byte */
    sockaddr_sz += (plen + 1);

  } else if (*sock_path == '@') {
    /* abstract namespace socket */

    /* First byte stays/replaced by \0 byte for abstract socket. Copy with trailing NUL byte */
    memcpy(sockaddr.sun_path + 1, sock_path + 1, plen);

    /* trailing NUL byte excluded in size */
    sockaddr_sz += plen;

  } else {
    return -EINVAL;
  }

  if (notify_socket == -EBADF) {
    notify_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (notify_socket == -1) {
      ret = -errno;
      fprintf(stderr, "Failed to open NOTIFY_SOCKET=%s: %s\n", sock_path, strerror(-ret));
      notify_socket = -EBADF;
      return ret;
    }
  }

  ret = sendto(notify_socket, "READY=1", sizeof("READY=1"), 0, (const struct sockaddr *) &sockaddr, sockaddr_sz);
  if (ret == -1) {
    ret = -errno;
    fprintf(stderr, "Failed to notify ready to NOTIFY_SOCKET=%s: %s\n", sock_path, strerror(-ret));
    close(notify_socket);
    notify_socket = -EBADF;
    return ret;
  }

  /* keep notify_socket open */
  return 0;
}


int main(int argc, char **argv)
{
  char *rundir;
  int pipe_fd, ret = 0;
  pthread_t ui_update_thread;

  if (argc == 1)
    read_in_configuration("config.ini");
  else if (argc == 2)
    read_in_configuration(argv[1]);
  else {
    fprintf(stderr, "usage: psplash-lvgl [config.ini]\n");
    exit(1);
  }

  rundir = getenv("PSPLASH_FIFO_DIR");

  if (!rundir)
    rundir = "/run";

  if (chdir(rundir) != 0) {
    perror(NULL);
  }

  if (mkfifo(PSPLASH_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP))
  {
    if (errno != EEXIST)
    {
      perror("mkfifo");
      exit(1);
    }
  }

  pipe_fd = open(PSPLASH_FIFO, O_RDONLY | O_NONBLOCK);

  if (pipe_fd == -1)
  {
    perror("pipe open");
    goto unlink_fifo_exit;
  }

  sdnotify_ready();

  init_lvgl();
  ui_create();

  /* call task handler once to show ui, to prevent race of update thread against psplash_main exiting directly */
  lv_task_handler();

  pthread_create(&ui_update_thread, NULL, ui_update_thread_cb, NULL);

  psplash_main(pipe_fd);

unlink_fifo_exit:
  unlink(PSPLASH_FIFO);

  return ret;
}
