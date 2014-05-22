#include <iostream>
#include <iomanip>
#include "vips_x.hpp"
#include <string>
#include <sstream>

vips::VImage test1(vips::VImage & in)
{
    vips::VIMask sharpen3x3 (3, 3, 8, 0,
                             -1, -1, -1,
                             -1, 16, -1,3
                             -1, -1, -1);
    in.extract_area (100, 100,
                     in.Xsize () - 200,
                     in.Ysize () - 200).
        affine (0.9, 0, 0, 0.9, 0, 0,
                0, 0, in.Xsize () * 0.9, in.Ysize () * 0.9).
        conv (sharpen3x3);
    in = in * 1.2;
    in *= 2.3;
    return in;
}

int
main (int argc, char **argv)
{
    int height = 400;
    try {
        if (argc >= 1+1) {      // at least one argument
            for (int i = 1; i < argc; i++) {
                std::string src(argv[i]);
                vips::VImage img(src.c_str()); // TIFF, JPEG, PBM/PGM/PPM, PNG, HDR, Matlab or VIPS
                img = resize_to_height(img, height);
                img += img;

                auto dst = src;
                std::ostringstream oss;
                oss << height;
                dst.insert(src.size()-4, "-h" + oss.str()); // add small suffix to filename
                img.write(dst.c_str()); // write output file
            }
        } else {
            std::cout << "Usage: " << argv[0] << " <IN-IMAGES...>" << std::endl;
        }
    }
    catch (vips::VError & e) {
        e.perror(argv[0]);
    }
    return (0);
}
