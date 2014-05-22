// \see http://www.kerrywong.com/2008/09/13/tbb-mandelbrot-set/

#include <iostream>
#include <fstream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/partitioner.h"
#include "tbb/blocked_range2d.h"
#include "tbb_video.hpp"

using namespace std;
using namespace tbb;

int max_iteration = 1020;
const int screen_size =1000;

float x_min = -1.5f;
float y_min = -1.1f;
float x_max = 0.5f;
float y_max = 1.1f;
float x_range = x_max - x_min;
float y_range = y_max -y_min;

//The commented out lines in this program are used to write
//screen content to text files. I used the output file to
//verify that both methods I used generated identical results.

//int _buf[screen_size][screen_size];

class Mandelbrot_TBB
{
public:
    void operator() (const blocked_range2d<size_t> &r) const {
        drawing_area drawing(0, 0, screen_size,screen_size);

        for (size_t x = r.rows().begin(); x < r.rows().end(); x++) {
            for (size_t y = r.cols().begin(); y < r.cols().end(); y++) {
                float zx = 0;
                float zy = 0;

                float cx = (float)x / (float)screen_size * x_range + x_min;
                float cy = (float)y / (float)screen_size * y_range + y_min;

                int i = 0;      // iteration index
                while (zx * zx + zy * zy <= 4 && i < max_iteration) {
                    float xtemp = zx * zx - zy * zy + cx;
                    zy = 2 * zx * zy + cy;
                    zx = xtemp;
                    i++;
                }

                const int itr = i % 255;
                const color_t c = itr << 16 | itr << 8 | itr;

                drawing.set_pixel(x,y,c);
                //_buf[x][y] = itr;
            }
        }
    }
};

class Mandelbrot_TBB_1
{
public:
    void operator() (const blocked_range2d<size_t> &r) const
    {
        drawing_area drawing(0, 0, screen_size,screen_size);

        for (size_t x = r.rows().begin(); x < r.rows().end(); x+=2) {
            for (size_t y = r.cols().begin(); y < r.cols().end(); y++) {
                float zx1 = 0;
                float zx2 = 0;
                float zy1 = 0;
                float zy2 = 0;

                float cx1 = (float)x / (float)screen_size * x_range + x_min;
                float cx2 = (float)(x + 1) / (float)screen_size * x_range + x_min;

                float cy = (float)y / (float)screen_size * y_range + y_min;

                int i1 = 0;
                int i2 = 0;
                bool loop_stop1 = false;
                bool loop_stop2 = false;

                while (not (loop_stop1 and
                            loop_stop2)) {
                    float xtemp1;
                    float xtemp2;

                    if ((zx1 * zx1 + zy1 * zy1 <= 4) && (i1 < max_iteration)) {
                        xtemp1 = zx1 * zx1 - zy1 * zy1 + cx1;
                        zy1 = 2 * zx1 * zy1 + cy;
                        zx1 = xtemp1;
                        i1++;
                    } else {
                        loop_stop1 = true;
                    }

                    if ((zx2 * zx2 + zy2 * zy2 <= 4) && (i2< max_iteration)) {
                        xtemp2 = zx2 * zx2 - zy2 * zy2 + cx2;
                        zy2 = 2 * zx2 * zy2 + cy;
                        zx2 = xtemp2;
                        i2++;
                    } else {
                        loop_stop2 = true;
                    }
                }

                int itr;

                itr = (i1) % 255;
                color_t c = itr << 16 | itr << 8 | itr;
                drawing.set_pixel(x,y,c);
                //_buf[x][y] = itr;

                itr = i2  % 255;
                c = itr << 16 | itr << 8 | itr;
                drawing.set_pixel(x+1,y,c);
                //_buf[x+1][y] = itr;
            }
        }
    }
};

class Mandelbrot_Display
{
public:
    void run(blocked_range2d<size_t> &r) {
        //parallel_for(r, Mandelbrot_TBB());
        parallel_for(r, Mandelbrot_TBB_1());
    }
};

int main()
{
    task_scheduler_init init;
    static tick_count t_start, t_end;

    video v; v.init_window(screen_size,screen_size);

    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout.precision(2);

    Mandelbrot_Display disp;

    blocked_range2d<size_t> r(0, screen_size, 20, 0, screen_size, 20);

    t_start = tick_count::now();
    disp.run(r);
    t_end = tick_count::now();
    cout << (t_end - t_start).seconds() * 1000 << " ms" << endl;

    //ofstream myfile;
    //file.open ("mandelbrot_tbb_1.txt");
    //for (int x = 0; x<screen_size;x++)
    //{
    //    for (int y = 0; y<screen_size; y++)
    //    {
    //        myfile << _buf[x][y] << "\t";
    //    }
    //    myfile << endl;
    //}
    //myfile.close();

    v.main_loop();

    return 0;
}
