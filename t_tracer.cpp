#include "utils.hpp"
#include "tracer/tracer.hpp"

int main(int argc, char * argv[])
{
    using namespace tracer;

    char * texpath = NULL;
    if (argc == 2)
    {
	texpath = argv[1];
    }
    else
    {
	cout << "usage: " << argv[0] << " file.jpg " << endl;
	return -1;
    }

    uint w, h;
    handle_main_args_wh(argc, argv, w, h);
    w = 600; h = 600;

    const uint n = 1;
    const Vec1 f = 10;

    Camera camera = Camera(Vec3(0,0,0),
			   Vec3(1,0,0), Vec3(0,-1,0),
			   static_cast<double>(w) / h, 1, 5);

    PointLight pointlight(Vec3(0,0,0), Color(255,255,255));

    Scene scene(camera, pointlight, Color(0,0,0));
    if (true)
    {
	scene.sphere_grid(Vec3(0,0,-20 * f), 1 * f, 2 * f, n,n,n, texpath);
    }
    else
    {
	scene.box_grid(Vec3(0,0,-20 * f), 1 * f, 2 * f, n,n,n);
    }
    scene.prepare();

    Viewer v(scene, w, h);
    v.mainloop();

    return 0;
}
