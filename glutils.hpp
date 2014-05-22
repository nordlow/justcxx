/*! \file glutils.hpp
 * \brief Various utility functions useful when programming in OpenGL.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://www.opengl.org/wiki/Common_Mistakes
 * \see https://en.wikibooks.org/wiki/OpenGL_Programming
 * \see http://www.geeks3d.com/20130705/shader-library-circle-disc-fake-sphere-in-glsl-opengl-glslhacker/
 */

#pragma once

/*! To enable OpenGL Extension Function Prototypes (in glext.h). */
#define GL_GLEXT_PROTOTYPES

#ifdef HAVE_GL_GLEW_H              /* http://glew.sourceforge.net/ */
#  include <GL/glew.h> /* NOTE: GLEW must override traditional GL Headers */
#else
#  if HAVE_GL_GL_H
#    include <GL/gl.h>
#  endif
#  if HAVE_GL_GLEXT_H
#    include <GL/glext.h>
#  endif
#endif

#ifdef HAVE_GL_GLXEW_H             /* http://glew.sourceforge.net/ */
#  include <GL/glxew.h> /* NOTE: GLEW must override traditional GL Headers */
#else
#  if HAVE_GL_GLX_H
#    include <GL/glx.h>
#  endif
#  if HAVE_GL_GLXEXT_H
#    include <GL/glxext.h>
#  endif
#endif

#ifdef HAVE_GL_GLU_H
#  include <GL/glu.h>
#endif

#if defined(HAVE_X11_XLIB_H)
#  include <X11/Xlib.h>
#endif

#include "geometry/vec.hpp"

/*! Assuming that your application thread is made current to an OpenGL rendering
 * context, the following routine can be used to determine at run-time if the
 * OpenGL implementation really supports a particular extension.
 */
int opengl_isExtensionSupported(const char *extension);

extern int g_opengl_has_GL_EXT_bgra;

/*! Perform run-time check of OpenGL extensions.
 * Affects all the global variables beginning with the prefix g_opengl_has.
 */
void opengl_runtimecheckExtensions(void);

/*! With the isExtensionSupported routine, you can check if the current OpenGL
 * rendering context supports a given OpenGL extension. To make sure that the
 * EXT_bgra extension is supported before using it, you can do the following.
 *
 * Notice that if the EXT_bgra extension is lacking at either run-time or
 * compile-time, the code above will detect the lack of EXT_bgra support. Sure
 * the code is a bit messy, but the code above works. You can skip the
 * compile-time check if you know what development environment you are using
 * and you do not expect to ever compile with a <GL/gl.h> that does not
 * support the extensions that your application uses. But the run-time check
 * really should be performed since who knows on what system your program ends
 * up getting run on.
 */
int opengl_testuseExtensions(void);

void opengl_check_stuff(void);

/*! Check if current hardware supports any of the OpenGL Extensions
 * - \c GL_ARB_texture_rectangle,
 * - \c GL_EXT_texture_rectangle,
 * - \c GL_NV_texture_rectangle.
 *
 * NOTE: ARB_texture_rectangle is functionally @b identical to
 * EXT_texture_rectangle and NV_texture_rectangle extensions currently
 * shipping, @b except for the additions to the OpenGL @b Shading
 * @b Language.
 */
int opengl_has_ARB_EXT_NV_texture_rectangle(void);

/*! Check for presence of OpenGL Shading Language (GLSL).
 * \return 1 upon presence, 0 otherwise.
 */
int glCheck_GLSL(void);

/*! Replace the Current OpenGL Matrix with an x-y-2D-Affine Matrix whose
 * translation is \p t and scale is \p s.
 */
inline void glAffineXY_CR_vec2f_vec2f(const vec2f& t,
                                      const vec2f& s)
{
  glLoadIdentity();
  glTranslatef(t(0), t(1), 0.0f);
  glScalef(s(0), s(1), 1.0f);
}

/*! Draw a Textured Sphere of radius \p rad with \p n subdivisions.
 */
void gluDrawTexSphere(GLfloat rad, GLint n);

/*! Load a texture from a file and create an OpenGL texture object for
 * it, and set appropriate min/mag texture filters
 */
GLuint glLoadTexture(const char* path);

/*! Compile and Link the Vertex Program \p pathV and the
 * Fragment Program \p pathF.
 *
 * \return a program handle (positive integer) upon succees, or
 *         otherwise on failure -1 and print the GLSL error string to
 *         \c stderr.
 */
GLuint glBuildProgram(const char * pathV,
		      const char * pathF);

/*! Try to Wait for Vertical Sync (VSync) if possible.
 * \return 1 if support for Video Vertical Synchronization (VSync) was present
 *         and wait was performed, 0 otherwise.
 */
int glpTryWaitVideoSync(void);
