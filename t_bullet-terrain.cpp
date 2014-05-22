/*
 * \brief Bullet Terrain Test
 * \see http://www.alexmac.cc/bullet-test/
 * \author Alexander Macdonald
 * \build g++ -I/usr/include/bullet -I/usr/include/bullet/BulletDynamics -I/usr/include/bullet/BulletCollision -I/usr/include/bullet/LinearMath small-terrain-test.cpp -o small-terrain-test -lSDL -lGL -lGLU -lglut -lbulletdynamics -lbulletmath -lbulletcollision
 */

#include <iostream>
#include <vector>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//Bullet
#include "Dynamics/btDynamicsWorld.h"
#include "Dynamics/btDiscreteDynamicsWorld.h"
#include "Dynamics/btRigidBody.h"
#include "ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BroadphaseCollision/btAxisSweep3.h"
#include "CollisionShapes/btCollisionShape.h"
#include "CollisionShapes/btSphereShape.h"
#include "CollisionShapes/btTriangleIndexVertexArray.h"
#include "CollisionShapes/btBvhTriangleMeshShape.h"
#include "CollisionShapes/btStaticPlaneShape.h"
#include "btVector3.h"
#include "btMatrix3x3.h"
#include "btTransform.h"
#include "btDefaultMotionState.h"

int mainloop();
int width = 640, height = 480, depth = 0;
int flags = SDL_OPENGL | SDL_RESIZABLE | SDL_HWSURFACE | SDL_HWACCEL;
SDL_Surface *window = NULL;

float ball_radius;
GLUquadric *ball;

struct Vertex {
  float x;
  float y;
  float z;
};

void CopyVertex(Vertex *src, Vertex *dst)
{
  (*dst).x = (*src).x;
  (*dst).y = (*src).y;
  (*dst).z = (*src).z;
}

struct Triangle {
  int v1;
  int v2;
  int v3;
};

struct Matrix {
  float row1[4];
  float row2[4];
  float row3[4];
  float row4[4];
};

unsigned int last_time = 0;
unsigned int frame_time = 0;


// -----------------------------------------------------------------------------
// Terrain
// -----------------------------------------------------------------------------

bool terrain_updated = true;
Vertex *terrain_vertex_colors;
Vertex *terrain_triangle_normals;
Vertex *terrain_vertices;
Triangle  *terrain_triangles;
int terrain_triangle_count, terrain_vertex_count;

int terrain_w, terrain_h;

void init_terrain()
{
  terrain_w = terrain_h = 100;

  terrain_triangle_count = (terrain_w - 1) * (terrain_h - 1) * 2;
  terrain_vertex_count = terrain_w * terrain_h;
	
  terrain_vertices = new Vertex[terrain_vertex_count];
  terrain_triangles = new Triangle[terrain_triangle_count];
	
  terrain_vertex_colors = new Vertex[terrain_vertex_count];
	
  for(int i=0; i<terrain_vertex_count; i++)
  {
    terrain_vertices[i].x = i % terrain_w;
    terrain_vertices[i].z = i / terrain_w;
    terrain_vertices[i].y = 0.0f;
		
    terrain_vertex_colors[i].x = 0.0f;
    terrain_vertex_colors[i].y = 0.5f;
    terrain_vertex_colors[i].z = 0.0f;
  }
	
  int pos = 0;
	
  for(int y=1; y<terrain_h; y++)
  {
    for(int x=1; x<terrain_w; x++)
    {
      int i1 = ((y-1) * terrain_w) + (x-1);
      int i2 = (y * terrain_w) + (x-1);
      int i3 = ((y-1) * terrain_w) + x;
      int i4 = (y * terrain_w) + x;
			
      terrain_triangles[pos].v1 = i1;
      terrain_triangles[pos].v2 = i2;
      terrain_triangles[pos].v3 = i3;
			
      terrain_triangles[pos+1].v1 = i2;
      terrain_triangles[pos+1].v2 = i4;
      terrain_triangles[pos+1].v3 = i3;
			
      pos += 2;
    }
  }
}

void terrain_raise(float xp, float yp, float amount, float area)
{
  terrain_updated = true;
	
  for(int i=0; i<terrain_vertex_count; i++)
  {
    float x = i % terrain_w;
    float y = i / terrain_w;
		
    float xd = xp - x;
    float yd = yp - y;
    float d = (float)sqrt((xd*xd) + (yd*yd));
				
    float a = (terrain_vertices[i].y + amount) - (d * area);

    if (a > terrain_vertices[i].y) {
      terrain_vertices[i].y = a;
      terrain_vertex_colors[i].x = a / 10.0f;
    }
  }
}

void terrain_lower(float xp, float yp, float amount, float area)
{
  terrain_updated = true;
	
  for(int i=0; i<terrain_vertex_count; i++)
  {
    float x = i % terrain_w;
    float y = i / terrain_w;
		
    float xd = xp - x;
    float yd = yp - y;
    float d = (float)sqrt((xd*xd) + (yd*yd));
				
    float a = (terrain_vertices[i].y - amount) + (d * area);

    if (a < terrain_vertices[i].y) {
      terrain_vertices[i].y = a;
      terrain_vertex_colors[i].x = a / 10.0f;
    }
  }
}
void render_terrain()
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
	
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), terrain_vertices);
  glColorPointer(3, GL_FLOAT, sizeof(Vertex), terrain_vertex_colors);
  glDrawElements(GL_TRIANGLES, terrain_triangle_count*3, GL_UNSIGNED_INT, terrain_triangles);
	
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}
	
void render_terrain_wireframe()
{
  glEnableClientState(GL_VERTEX_ARRAY);
	
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), terrain_vertices);
  glDrawElements(GL_TRIANGLES, terrain_triangle_count*3, GL_UNSIGNED_INT, terrain_triangles);
	
  glDisableClientState(GL_VERTEX_ARRAY);
}

// -----------------------------------------------------------------------------
// Bullet Physics
// -----------------------------------------------------------------------------

const int maxProxies = 32766;

btDynamicsWorld* world;
btCollisionDispatcher* dispatcher;
btOverlappingPairCache* broadphase;
btSequentialImpulseConstraintSolver* solver;

btRigidBody* mesh_body;
btBvhTriangleMeshShape* mesh;
btTriangleIndexVertexArray *mesh_interface;

std::vector<btCollisionShape*> collision_meshes = std::vector<btCollisionShape*>();
std::vector<btRigidBody*> bodies = std::vector<btRigidBody*>();

void init_physics()
{
  btVector3 worldAabbMin(-100,-100,-100);
  btVector3 worldAabbMax(100,100,100);
	
  dispatcher = new btCollisionDispatcher();
  broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
  solver = new btSequentialImpulseConstraintSolver();

  world = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver);
  world->setGravity(btVector3(0,-50,0));

  // Trimesh
  mesh_interface = new btTriangleIndexVertexArray(terrain_triangle_count,
                                                  &(terrain_triangles[0].v1),
                                                  sizeof(Triangle),
                                                  terrain_vertex_count,
                                                  &(terrain_vertices[0].x),
                                                  sizeof(Vertex));
	
  mesh = new btBvhTriangleMeshShape(mesh_interface, true);
	
  btTransform mesh_pos;
  mesh_pos.setIdentity();
  mesh_pos.setOrigin(btVector3(0,0,0));

  btDefaultMotionState* motionstate = new btDefaultMotionState(mesh_pos);
  mesh_body = new btRigidBody(0.0f,motionstate,mesh);
  mesh_body->setCollisionFlags( mesh_body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

  world->addRigidBody(mesh_body);
}

void add_ball(float xp, float yp, float zp)
{
  float mass = 5.0f;
  btVector3 inertia(0,0,0);
	
  btTransform trans;
  trans.setIdentity();
  trans.setOrigin(btVector3(xp,yp,zp));
	
  btCollisionShape *geom = new btSphereShape (ball_radius);
  geom->calculateLocalInertia(mass,inertia);
	
  btDefaultMotionState* motionstate = new btDefaultMotionState(trans);
  btRigidBody* body = new btRigidBody(mass,motionstate,geom,inertia);

  world->addRigidBody(body);

  bodies.push_back(body);
  collision_meshes.push_back(geom);
	
  std::cout << "ball added: " << xp << ", " << yp << ", " << zp << std::endl;
}

void render_balls()
{
  int num = bodies.size();
	
  btDefaultMotionState* ms;
  float m[16];
	
  for(int i=0; i<num; i++)
  {
    btPoint3 com = bodies[i]->getCenterOfMassPosition();
		
    ms = (btDefaultMotionState*)bodies[i]->getMotionState();
    ms->m_graphicsWorldTrans.getOpenGLMatrix(m);
		
    glColor3f(0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glMultMatrixf(m);
    gluSphere(ball, ball_radius, 10, 10);
    glPopMatrix();
  }
}

void step_physics()
{
  if (terrain_updated) {
    world->updateAabbs();
    mesh->refitTree();
    world->getBroadphase()->cleanProxyFromPairs(mesh_body->getBroadphaseHandle());
  }

  world->stepSimulation(1.0 / 60.0f);
}

// -----------------------------------------------------------------------------
// function: main
// -----------------------------------------------------------------------------
int main(int argn, char *args[])
{
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    std::cerr << "couldnt initialize graphics: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1);

  if((window = SDL_SetVideoMode(width, height, depth, flags)) == NULL)
  {
    std::cerr << "couldnt create window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  SDL_WM_SetCaption("Test App", "Test App");
	
  glShadeModel(GL_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glPointSize(5.0f);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glClearColor(0.0f, 0, 0, 0);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  mainloop();
}

// ------------------------------------------------------------------------------------------------------------
// Rendering
// ------------------------------------------------------------------------------------------------------------

void render_cursor(float x, float z)
{
  glBegin(GL_LINES);
  glColor3f(1.0f, 0.0f, 1.0f);
  glVertex3f(x, -100.0f, z);
  glVertex3f(x, 10.0f, z);
  glEnd();
}

void render_axes(float size)
{
  glBegin(GL_LINES);
  // Red -- X
  glColor3f(0.0f, 0.0f, 0.0f);
  glVertex3f(-1.0f, 0.0f, 0.0f);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(+1.0f, 0.0f, 0.0f);
		
  // Green -- Y
  glColor3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, -1.0f, 0.0f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, +1.0f, 0.0f);
		
  // Blue -- Z
  glColor3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, -1.0f);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, +1.0f);
  glEnd();
}
	
void print_errors()
{
  int e = (int)glGetError();
	
  if (e != GL_NO_ERROR)
  {
    std::cout << "OpenGL Error (" << e << "): " << gluErrorString(GL_NO_ERROR) << std::endl;
  }
}

// ------------------------------------------------------------------------------------------------------------
// function: mainloop
// ------------------------------------------------------------------------------------------------------------
int mainloop()
{
  SDL_Event event;

  init_terrain();
	
  init_physics();
	
  ball_radius = 2.0f;
  ball = gluNewQuadric();
	
  bool running = true, raise = false, lower = false;
		
  float cam_x = 2.0f, cam_y = -32.0f, cam_z = 2.0f, cam_xr = 40.0f, cam_yr = 135.0f;
  float cx = 0.0f, cz = 0.0f;
	
  frame_time = 10;
  last_time = glutGet (GLUT_ELAPSED_TIME);

  while(running)
  {
    while( SDL_PollEvent(& event) )
    {
      switch ( event.type )
      {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        switch(event.key.keysym.sym)
        {
        case SDLK_w:
          cam_z += 0.2f;
          break;
        case SDLK_a:
          cam_x += 0.2f;
          break;
        case SDLK_s:
          cam_z -= 0.2f;
          break;
        case SDLK_d:
          cam_x -= 0.2f;
          break;
        case SDLK_SPACE:
          add_ball(cx, 20.0f, cz);
          break;
        default:
          break;
        }
        break;
      case SDL_MOUSEMOTION:
        cx += (event.motion.xrel / 5.0f);
        cz += (event.motion.yrel / 5.0f);
        break;
      case SDL_MOUSEBUTTONDOWN:
        switch(event.button.button)
        {
        case SDL_BUTTON_LEFT:
          raise = true;
          break;
        case SDL_BUTTON_RIGHT:
          lower = true;
          break;
        default:
          break;
        }
        break;
      case SDL_MOUSEBUTTONUP:
        switch(event.button.button)
        {
        case SDL_BUTTON_LEFT:
          raise = false;
          break;
        case SDL_BUTTON_RIGHT:
          lower = false;
          break;
        default:
          break;
        }
        break;
      case SDL_VIDEORESIZE:
        window = SDL_SetVideoMode(event.resize.w, event.resize.h, depth, flags);
        if(window == NULL)
        {
          std::cerr << "couldnt resize window: " << SDL_GetError() << std::endl;
          SDL_Quit();
          return -1;
        }
        glViewport(0, 0, event.resize.w, event.resize.h);
        break;
      default:
        break;
      }
    }
			
    if(raise) {
      terrain_raise(cx, cz, 0.5f, 0.04f);
    }
		
    if(lower) {
      terrain_lower(cx, cz, 0.5f, 0.04f);
    }
		
    step_physics();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //Camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 4 / 3, 0.01f , 500.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
		
    glRotatef(cam_xr, 1.0f, 0.0f, 0.0f);
    glRotatef(cam_yr, 0.0f, 1.0f, 0.0f);
    glTranslatef(cam_x, cam_y, cam_z);
		
    render_axes(1.0f);
		
    render_cursor(cx, cz);
		
    render_terrain();
		
    render_balls();
		
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0f, 0.0f, 0.0f);
    render_terrain_wireframe();
		
    print_errors();

    SDL_GL_SwapBuffers();
		
    unsigned int t = glutGet (GLUT_ELAPSED_TIME);
    frame_time = (t - last_time);
    last_time = t;
  }
	
  return 0;
}

