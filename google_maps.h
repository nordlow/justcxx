/*!
 * \file google_maps.h
 * \brief Library for fetching Geographical Bitmaps from Google Maps.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://cfis.savagexi.com/articles/2006/06/30/mouse-coordinates-to-lat-long
 * \see http://www.codeproject.com/useritems/googlemap.asp
 *
 * Named file using Joakims standard naming by adding longitude and latitude
 * to the Google Map server file name.
 *
 * If image file not already exists in local cache
 * download it from a Google server using curl and save it to the cache
 * directory cache_dir.
 *
 * Google uses 4 servers to balance the load. these are mt0, mt1, mt2 and mt3.
 *
 * Each tile is a 256x256 PNG or JPEG picture.
 *
 * 1. Use image file IO functions to read pixels into an RGB-buffer.
 *    - jpgio.h as in vfile.h to read bitmap from JPEG-file.
 *
 * 2. Use this RGB-buffer as input argument for creating an OpenGL texture.
 */

/* ========================================================================= */

#pragma once

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef HAVE_CURL_CURL_H
#  include <curl/curl.h>
#else
typedef void CURLM;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------- Group Separator ---------------------------- */

typedef void GOOGLE_MAPS;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Initialize Google Maps Instance and return it.
 *
 * \param[in] cache_dir is path to local directory on file system where images
 *         have been and/or are to be cached.
 * \return the created instance
 */
GOOGLE_MAPS * google_maps_init(CURLM * CURLM_handle,
			       const char * cache_dir);

/*!
 * Clear Google Maps Instance.
 */
void google_maps_clear(GOOGLE_MAPS * maps);

void google_maps_reorg_levels(const char * cache_dir);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
