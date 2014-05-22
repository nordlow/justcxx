///////////////////////////////////////////////////////////////
// A SIMPLE SDL BASECODE BY BALDURK AT GLPROGRAMMING.COM
///////////////////////////////////////////////////////////////
// find us at glprogramming.com or on IRC at irc.enterthegame.com/#opengl
///////////////////////////////////////////////////////////////
// Last Modified: 19/05/04
///////////////////////////////////////////////////////////////
// SDL documentation here: http://sdldoc.csn.ul.ie/index.php
//
// OpenGL documentation here: http://www.opengl.org
//             and here: http://www.glprogramming.com/red
//             and here: http://www.glprogramming.com/blue
///////////////////////////////////////////////////////////////

// Necessary windows libraries and includes.
#ifdef _WIN32
#  pragma comment(lib, "glu32.lib")
#  pragma comment(lib, "opengl32.lib")
#  pragma comment(lib, "SDL.lib")
#  pragma comment(lib, "SDLmain.lib")
#  include <windows.h>
#endif

// Includes for the OpenGL, GLU and SDL headers.
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

// An array to keep key-states.
int keys[SDLK_LAST];

// The width, height and bitdepth
const int width = 800, height = 600, bitdepth = 32;

// A function to draw the scene.
// The parameter is the number of milliseconds passed
// since the previous frame.
// This is where you put your code that executes every frame.
void Draw(int TimePassed)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glColor3f(1.0f, 1.0f, 1.0f);
}

// This function performs any Initialisation you need to do for your project
// before the program starts up
void Init(void)
{
}

// The Basecode below here.

int main(int argc, char **argv)
{
  int i;

  // Initialise SDL, complain if we can't.
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) { // video only (no audio)
    fprintf(stderr, "InitSubSystem() failed. SDL Error: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  // Get a surface.
  // There is another flag, SDL_GL_DOUBLEBUFFER, to enable double buffering
  // but it seems to be enabled by default. If you find it differently,
  // you just need to use bitwise OR (|) to add in SDL_GL_DOUBLEBUFFER.
  SDL_Surface *Surface = SDL_SetVideoMode(width, height, bitdepth,
					  SDL_OPENGL | SDL_GL_DOUBLEBUFFER);

  // Check the surface, complain if we didn't get it.
  // We don't actually need the Surface variable,
  // we just use it to check the return value.
  if (not Surface) {
    fprintf(stderr, "SetVideoMode() Failed. SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  // Set up the GL viewport
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  gluPerspective(45.0f, (float)width/(float)height, 1.0f, 500.0f);
  glMatrixMode(GL_MODELVIEW);

  // Call the user defined Init function.
  Init();

  // initialise all the key stats to "up" initially
  for (i=0; i < SDLK_LAST; i++) keys[i] = 0;

  // initialise the time variables
  int TimePrev = SDL_GetTicks(), TimeCurrent, TimePassed;
  // main message pump
  while (1) {
    // Do the time-based movement calculations
    TimeCurrent = SDL_GetTicks();
    TimePassed  = TimeCurrent - TimePrev;

    // Call the user defined Draw function
    Draw(TimePassed);
    // Swap the front and back buffers
    SDL_GL_SwapBuffers();

    // Do the time-based movement calculations
    TimePrev = TimeCurrent;

    // Get an event
    SDL_Event Event;
    SDL_PollEvent(&Event);

    // Check its type.
    // If it's a quit-type event, break out of the SDL loop
    if ((Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE) ||
	Event.type == SDL_QUIT)
      break; // we get out of the while(1) like this.

    // If it's a keydown event, set the relevant key to true/down
    if (Event.type == SDL_KEYDOWN)
      keys[Event.key.keysym.sym] = 1;

    // If it's a keyup event, set the relevant to false/up
    if (Event.type == SDL_KEYUP)
      keys[Event.key.keysym.sym] = 0;
  }

  SDL_Quit();

  // Everything went fine.
  return 0;
}
