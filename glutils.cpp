#include "glutils.hpp"
#include "stdio_x.h"
#include <string.h>

int g_opengl_has_GL_EXT_bgra;

int opengl_isExtensionSupported(const char *extension) {
  const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;

  /* Extension names should not have spaces. */
  where = (GLubyte *) strchr(extension, ' ');
  if (where || *extension == '\0')
    return 0;
  extensions = glGetString(GL_EXTENSIONS);

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  start = extensions;

  for (;;) {
    where = (GLubyte *) strstr((const char *) start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if (where == start || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\0')
	return 1;
    start = terminator;
  }

  return 0;
}

void xsopengl_runtimecheckExtensions() {
  /* At context initialization. */
  g_opengl_has_GL_EXT_bgra = opengl_isExtensionSupported("GL_EXT_bgra");
}

int opengl_testuseExtensions() {
  int ret = 0;
  opengl_runtimecheckExtensions();

  /* When trying to use EXT_bgra extension. */
#ifdef GL_EXT_bgra
  if (g_opengl_has_GL_EXT_bgra) {
    int width = 4, height = 4;
    GLvoid * pixels[width * height];
    glDrawPixels(width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
  } else
#endif
    {
      /* No EXT_bgra so bail (or implement software workaround). */
      PWARN("Needs EXT_bgra extension!\n");
      ret = -1;
    }
  return ret;
}

void opengl_check_stuff(void) {
  GLint params[1];
  glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, params);
  lprintf("GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB: %d\n", params[0]);
}

int opengl_has_ARB_EXT_NV_texture_rectangle(void)
{
  return (GLEW_ARB_texture_rectangle ||
          GLEW_EXT_texture_rectangle ||
          GLEW_NV_texture_rectangle);
}

#include "extremes.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*!
 * Check for OpenGL Errors and Spew the error strings along with debug
 * info to \c stderr.
 */
#define glError()                                               \
  {                                                             \
    GLenum err = glGetError();                                  \
    while (err != GL_NO_ERROR) {					\
      fprintf(stderr, "glError: %s caught at %s:%u\n",          \
	      (char*)gluErrorString(err), __FILE__, __LINE__);  \
      err = glGetError();                                       \
    }                                                           \
  }

int glCheck_GLSL(void)
{
  int ret = 0;
#ifdef HAVE_GL_GLEW_H
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    PWARN("glewInit: %s\n", glewGetErrorString(err));
    return ret;
  }
  PNOTE("glewInit: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader) {
    ret = 1;
    printf("Ready for GLSL\n");
  } else {
    ret = 0;
    printf("Not ready for GLSL, =:(\n");
  }
#endif
  return ret;
}

void gluDrawTexSphere(GLfloat rad, GLint n) {
#ifdef HAVE_GL_GLU_H
  glPushMatrix();
  glRotatef(180, 0, 1, 0);
  glRotatef(90, 1, 0, 0);
  GLUquadric* sphere = gluNewQuadric();
  gluQuadricDrawStyle(sphere, GLU_FILL);
  gluQuadricNormals(sphere, GLU_SMOOTH);
  gluQuadricTexture(sphere, GL_TRUE);
  gluSphere(sphere, rad, n, n);
  gluDeleteQuadric(sphere);
  glPopMatrix();
#endif
}

#if 0
GLuint glLoadTexture(const char* pathT)
{
  /* clear error queues beforehand */
  glError ();

  /* read the image */
  ByteRaster* raster = read_image(pathT);
  if (raster == NULL) {
    fprintf (stderr, "Failed to read %s\n", pathT);
    return 0;
  }

  GLenum format;
  switch (raster->channels()) {
  case 1: format = GL_ALPHA; break;
  case 2: format = GL_LUMINANCE_ALPHA; break;
  case 3: format = GL_RGB; break;
  case 4: format = GL_RGBA; break;
  default:
    delete raster;
    fprintf (stderr, "Failed to find format for %s\n", pathT);
    return 0;
  }

#define NPOT(x) ( (x) & (x - 1) )
  if (NPOT (raster->width () ) || NPOT (raster->height () ) ) {
    fprintf (stderr, "Warning: %s has non-power-of-two size\n", pathT);
  }

  /* create a GL texture for the image */
  GLuint texture;
  glGenTextures (1, &texture);
  glBindTexture (GL_TEXTURE_2D, texture);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* magnification filtering */
  //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); /* OpenGL 3.0: Generate Mipmaps */
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (!GLEW_ARB_framebuffer_object) glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexImage2D(GL_TEXTURE_2D, 0, format, raster->width(), raster->height(),
	       0, format, GL_UNSIGNED_BYTE, raster->head () );
  if (GLEW_ARB_framebuffer_object) glGenerateMipmap(GL_TEXTURE_2D);  /* Generate mipmaps now!!! OpenGL 3.0 */
  glError();
  return texture;
}
#endif

/*!
 * Compile a Shader using the source file \p pathS of type
 * \p stype.
 * \return shader handle (non-zero) upon succeeded, -1 upon error.
 */
GLuint glhelpCompileShader(const char * pathS, GLenum stype) {
  char *sbuf = NULL;		/* shader source buffer */
  size_t slen = 0;
  if (!fload_alt(pathS, &sbuf, &slen) ) {
    fprintf(stderr, "glBuildProgram: Couldn't read source file %s\n", pathS);
    free(sbuf);
    return -1;
  }

  GLuint hS = glCreateShader(stype); /* allocate a hS */

  /* compile the hS */
  const GLint gl_slen = slen;
  glShaderSource(hS, 1, (const GLchar* *)&sbuf, &gl_slen);
  glCompileShader(hS);
  free(sbuf);

  /* see if we succeeded, if we didn't, spew debugging info and quit */
  GLint status;			/* compile status */
  glGetShaderiv(hS, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    const GLsizei logMaxSize = 4096;
    GLchar logText[logMaxSize];
    GLsizei logSize;
    glGetShaderInfoLog(hS, logMaxSize, &logSize, logText);
    glDeleteShader(hS);
    leprintf( "failed to compile %s. hint:%s", pathS, logText);
    return -1;
  }

  return hS;
}

GLuint glBuildProgram(const char * pathV,
		      const char * pathF) {
  GLuint hVS = 0, hFS = 0;
  GLuint hP = 0;

  if (pathV) {
    hVS = glhelpCompileShader(pathV, GL_VERTEX_SHADER);
    if (hVS == 0) { return -1; } glError();
  }

  if (pathF) {
    hFS = glhelpCompileShader(pathF, GL_FRAGMENT_SHADER);
    if (hFS == 0) { glDeleteShader (hVS); return -1; } glError();
  }

  hP = glCreateProgram(); /* handle to program */

  if (pathV) { glAttachShader(hP, hVS); }
  if (pathF) { glAttachShader(hP, hFS); }

  glLinkProgram(hP);             /* link it */

  /* regardless of how the link ended up, we're done with the shaders, so delete them */
  if (pathV) { glDeleteShader(hVS); }
  if (pathF) { glDeleteShader(hFS); }

  /* see if link succeeded, if we didn't, spew debugging info and quit */
  GLint status;			/* link status */
  glGetProgramiv (hP, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    const GLsizei logMaxSize = 4096;
    GLchar logText[logMaxSize];
    GLsizei logSize;
    glGetProgramInfoLog (hP, logMaxSize, &logSize, logText);
    glDeleteProgram (hP);
    leprintf("failed to link %s with %s. hint: %s\n", pathV, pathF, logText);
    return -1;
  }

  return hP;
}

/*! Very simple example of how to perform YUV->RGB (YCrCb->RGB)
 * conversion with an OpenGL fragmen shader. The data (not included)
 * is presumed to be three files with Y, U and V samples for a 720x576
 * pixels large image.
 *
 * Note! The example uses NVidia extensions for rectangular textures
 * to make it simpler to read (non-normalised coordinates).
 * Rewriting it without the extensions is quite simple, but left as an
 * exercise to the reader. (The trick is that the shader must know the
 * image dimensions instead)
 *
 * The program also does not check to see if the shader extensions are
 * available - this is after all just a simple example.
 */
#if 0
int test_YUV420P_GLSL_fragment_shader(void) {
  int i;

  int bw = 640;
  int bh = 480;

  /* fragment program */
  const char *fprog =
    "uniform sampler2DRect Ytex;\n"
    "uniform sampler2DRect Utex,Vtex;\n"
    "void main(void) {\n"
    "  float nx,ny,r,g,b,y,u,v;\n"
    "  vec4 txl,ux,vx;"
    "  nx=gl_TexCoord[0].x;\n"
    "  ny=576.0-gl_TexCoord[0].y;\n"
    "  y=texture2DRect(Ytex,vec2(nx,ny)).r;\n"
    "  u=texture2DRect(Utex,vec2(nx/2.0,ny/2.0)).r;\n"
    "  v=texture2DRect(Vtex,vec2(nx/2.0,ny/2.0)).r;\n"
    "  y=1.1643*(y-0.0625);\n"
    "  u=u-0.5;\n"
    "  v=v-0.5;\n"
    "  r=y+1.5958*v;\n"
    "  g=y-0.39173*u-0.81290*v;\n"
    "  b=y+2.017*u;\n" "  gl_FragColor=vec4(r,g,b,1.0);\n" "}\n";

  if (false) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, bw, 0, bh, -1, 1);
    glViewport(0, 0, bw, bh);
    glClearColor(0, 0, 0, 0);
    glColor3f(1.0, 0.84, 0.0);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  }

  GLhandleARB hP  = glCreateProgram(); /* handle to program */
  GLhandleARB hFS = glCreateShader(GL_FRAGMENT_SHADER); /* handle to shader */

  /* Compile the shader. */
  glShaderSource(hFS, 1, &fprog, NULL); /* set source memory buffer */
  glCompileShader(hFS);                 /* and compile it */

  /* Print the compilation log. */
  glGetObjectParameterivARB(hFS, GL_OBJECT_COMPILE_STATUS_ARB, &i);
  char *s = malloc(32768);
  glGetInfoLogARB(hFS, 32768, NULL, s);
  printf("Compile Log: %s\n", s);
  free(s);

  /* Create a complete program object. */
  glAttachObjectARB(hP, hFS);
  glLinkProgram(hP);

  /* And print the link log. */
  char *t = malloc(32768);
  glGetInfoLogARB(hP, 32768, NULL, t);
  printf("Link Log: %s\n", t);
  free(t);

  glUseProgram(hP);             /* use the fragment program. */

  /* texture image dimensions */
  int w = 376, h = 288;

  int szY = 4 * w * h;             /* 4 times as large as U and V */
  int szU = w * h;
  int szV = w * h;
  GLubyte *texY = malloc(szY);
  GLubyte *texU = malloc(szU);
  GLubyte *texV = malloc(szV);

  /* Load the textures. */
  if (false) {
    FILE * fp = NULL;
    fp = fopen("Image.Y", "rb"); fread(texY, szY, 1, fp); fclose(fp);
    fp = fopen("Image.U", "rb"); fread(texU, szU, 1, fp); fclose(fp);
    fp = fopen("Image.V", "rb"); fread(texV, szV, 1, fp); fclose(fp);
  }

  glEnable(GL_TEXTURE_2D);      /* enable texturing */

  /* Select texture unit 1 as the active unit and bind the U texture. */
  glActiveTexture(GL_TEXTURE1); i = glGetUniformLocation(hP, "texU"); glUniform1i(i, 1);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, 1);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texU);

  /* Select texture unit 2 as the active unit and bind the V texture. */
  glActiveTexture(GL_TEXTURE2); i = glGetUniformLocation(hP, "texV"); glUniform1i(i, 2);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, 2);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texV);

  /* Select texture unit 0 as the active unit and bind the Y texture. */
  glActiveTexture(GL_TEXTURE0); i = glGetUniformLocation(hP, "texY"); glUniform1i(i, 0);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, 3);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_LUMINANCE, 2*w, 2*h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texY);

  /* Draw image (again and again). */
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0);
  glVertex2i(0, 0);
  glTexCoord2i(720, 0);
  glVertex2i(bw, 0);
  glTexCoord2i(720, 576);
  glVertex2i(bw, bh);
  glTexCoord2i(0, 576);
  glVertex2i(0, bh);
  glEnd();

  glUseProgram(0);     /* use no fragment program */

  free(texY);
  free(texU);
  free(texV);

  return (0);
}
#endif

/*! See the GLX Extensions SGI_video_sync (\c GLX_SGI_video_sync)
 * - NVIDIA: fullscreen and windowed modes
 * - ATI: fullscreen mode only
 */
int glpTryWaitVideoSync(void) {
  int ret = 0;
#if (defined(GLX_SGI_video_sync))
  if (GLXEW_SGI_video_sync) {
    uint rc = 0;	/* retrace count */
    glXGetVideoSyncSGI(&rc);
    glXWaitVideoSyncSGI(2, (rc + 1) & 1, &rc);
    ret = 1;
  }
#endif
  return ret;
}
