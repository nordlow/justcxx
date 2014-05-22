/*! \file opencv_x.hpp
 * \brief OpenCV Extension.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-07-13 22:35
 */

#pragma once

/*! Check for OpenCV Version.
 * \see http://stackoverflow.com/questions/11030640/how-to-determine-opencv-version
 * \see /usr/include/opencv2/core/version.h
 */
#ifndef CV_VERSION_AT_LEAST
#define CV_VERSION_AT_LEAST(major,minor,subminor) (CV_MAJOR_VERSION > major || \
                                                   (CV_MAJOR_VERSION == major && CV_MINOR_VERSION >= minor) || \
                                                   (CV_MAJOR_VERSION == major && CV_MINOR_VERSION == minor && CV_SUBMINOR_VERSION >= subminor))
#endif
