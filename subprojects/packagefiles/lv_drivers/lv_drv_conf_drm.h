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

/*********************
 *  DISPLAY DRIVERS
 *********************/

/*-----------------------------------------
 *  DRM/KMS device (/dev/dri/cardX)
 *-----------------------------------------*/
#ifndef USE_DRM
#  define USE_DRM           1
#endif

#if USE_DRM
#  define DRM_CARD          "/dev/dri/card0"
#  define DRM_CONNECTOR_ID  -1	/* -1 for the first connected one */
#endif

#endif  /*LV_DRV_CONF_H*/
