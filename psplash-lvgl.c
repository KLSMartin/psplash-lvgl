/* 
 *  pslash - a lightweight framebuffer splashscreen for embedded devices. 
 *
 *  Copyright (c) 2006 Matthew Allum <mallum@o-hand.com>
 *
 *  Parts of this file ( fifo handling ) based on 'usplash' copyright 
 *  Matthew Garret.
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifdef HAVE_SYSTEMD
#include <systemd/sd-daemon.h>
#endif

/***************************************************************************/ /**
 * @file
 * @brief   manages various disk space related operations
 * @ingroup
 * @author  ML1
 * \n       KLS Martin GmbH + Co. KG
 * \n       Germany
 * @date    Created 23.02.2121
 ******************************************************************************/

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

/* lvgl includes */
#include "lvgl.h"
// #include "themes/apply_theme.h"
#include "drm.h"
#include "monitor.h"

/* Project Includes */

/* === Defines ======================================================= */
#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define DBG(x, a...) \
   { printf ( __FILE__ ":%d,%s() " x "\n", __LINE__, __func__, ##a); }
#else
#define DBG(x, a...) do {} while (0)
#endif

/* === Typedefs ======================================================= */
typedef struct
{
  float progress;
  struct
  {
    lv_task_t *update_task;
    lv_obj_t *bar;
  } ui;
} progress_indicator_data_t;

typedef struct
{
  lv_coord_t width;
  lv_coord_t height;
} size;

/* === Static members ======================================================= */
static const char * const PSPLASH_FIFO = "psplash_fifo";
static size display_size;
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

/*
  Desired layout:
  +-----------------------------------------------+
  |#########                                      |
  +-----------------------------------------------+
*/
static lv_obj_t *interactive_progress_bar_create(lv_obj_t *parent, progress_indicator_data_t *data)
{
  const lv_coord_t parent_obj_width = lv_obj_get_width_fit(parent);
  lv_obj_t *bar = lv_bar_create(parent, NULL);
  data->ui.bar = bar;
  lv_obj_set_auto_realign(bar, true);
  lv_obj_align_origo(bar, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_cont_set_fit2(bar, LV_FIT_MAX, LV_FIT_TIGHT);
  lv_cont_set_layout(bar, LV_LAYOUT_COLUMN_MID);
  lv_obj_set_width(bar, parent_obj_width * 0.8);
  // normalize scale [0 to 100]
  lv_bar_set_range(bar, 0, 100);
  return bar;
}

static void *ui_update_thread_cb(void *data) {
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
  static lv_color_t *buf_1;

  /* LittlevGL init */
  lv_init();
  /* Linux frame buffer device init */
#if USE_MONITOR
  monitor_init();
  display_size.width = LV_HOR_RES_MAX;
  display_size.height = LV_VER_RES_MAX;
#endif
#if USE_DRM
  drm_init();
  drm_get_sizes(&display_size.width, &display_size.height, NULL);
#endif
  // Ignore failing allocation on purpose. Let the application eventually crash (SEGV).
  buf_1 = malloc(display_size.width * display_size.height * sizeof(*buf_1));

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
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /* apply theme */
  // apply_theme();
}

/***************************************************************************/ /**
* @brief Entry function of the program
* @return does not return
*
* Takes no arguments
******************************************************************************/
static void ui_create(__attribute__((unused)) const int angle)
{
  /*-------------- create page ---------------------*/
  lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);
  lv_obj_set_size(page, display_size.width, display_size.height);
  /* fix size, no scrolling */
  lv_page_set_scrollable_fit(page, LV_FIT_MAX);

  interactive_progress_bar_create(page, &progress_indicator_data);
}

void psplash_draw_msg(const char *msg)
{
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
      psplash_draw_progress(atoi(arg));
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

void psplash_main(int pipe_fd, int timeout)
{
  int err;
  ssize_t length = 0;
  fd_set descriptors;
  struct timeval tv;
  char *end;
  char *cmd;
  char command[2048];

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  FD_ZERO(&descriptors);
  FD_SET(pipe_fd, &descriptors);

  end = command;

  while (1)
  {
    if (timeout != 0)
      err = select(pipe_fd + 1, &descriptors, NULL, NULL, &tv);
    else
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

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    FD_ZERO(&descriptors);
    FD_SET(pipe_fd, &descriptors);
  }

  return;
}

int main(int argc, char **argv)
{
  char *rundir;
  int pipe_fd, i = 0, angle = 0, ret = 0;
  pthread_t ui_update_thread;

  while (++i < argc)
  {
    if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--angle"))
    {
      if (++i >= argc)
        goto fail;
      angle = atoi(argv[i]);
      continue;
    }

  fail:
    fprintf(stderr,
            "Usage: %s [-a|--angle <0|90|180|270>]\n",
            argv[0]);
    exit(-1);
  }

  rundir = getenv("PSPLASH_FIFO_DIR");

  if (!rundir)
    rundir = "/run";

  chdir(rundir);

  if (mkfifo(PSPLASH_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP))
  {
    if (errno != EEXIST)
    {
      perror("mkfifo");
      exit(-1);
    }
  }

  pipe_fd = open(PSPLASH_FIFO, O_RDONLY | O_NONBLOCK);

  if (pipe_fd == -1)
  {
    perror("pipe open");
    exit(-2);
  }

  //   if ((fb = psplash_fb_new(angle,fbdev_id)) == NULL)
  if (0)
  {
    ret = -1;
    goto unlink_fifo_exit;
  }

#ifdef HAVE_SYSTEMD
  sd_notify(0, "READY=1");
#endif

  init_lvgl();
  ui_create(angle);
  pthread_create(&ui_update_thread, NULL, ui_update_thread_cb, NULL);
  psplash_draw_progress(0);

#ifdef PSPLASH_STARTUP_MSG
  psplash_draw_msg(fb, PSPLASH_STARTUP_MSG);
#endif

  /* Scene set so let's flip the buffers. */
  /* The first time we also synchronize the buffers so we can build on an
   * existing scene. After the first scene is set in both buffers, only the
   * text and progress bar change which overwrite the specific areas with every
   * update.
   */
  //   psplash_fb_flip(fb, 1);

  psplash_main(pipe_fd, 0);

  //   psplash_fb_destroy (fb);

unlink_fifo_exit:
  unlink(PSPLASH_FIFO);

  return ret;
}
