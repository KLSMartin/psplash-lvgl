/**
 * @file lv_drv_conf.h
 * Configuration file for v7.10.1
 */

/*
 * COPY THIS FILE AS lv_drv_conf.h
 */

#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include "lv_conf.h"

/*-----------------------------------------
 *  Linux frame buffer device (/dev/fbx)
 *-----------------------------------------*/
#ifndef USE_FBDEV
#  define USE_FBDEV           1
#endif

#if USE_FBDEV
#  define FBDEV_PATH          "/dev/fb0"
#endif

#endif /*End of "Content enable"*/
