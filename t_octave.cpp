// Use can also use
// mkoctfile --link-stand-alone t_octave.cpp
// \see http://www.mathias-michel.de/download/howto-octave-c++.ps

#include <octave/oct.h>         // octave builtin functions
#include <octave/octave.h>
#include <octave/parse.h>
#include <iostream>

using std::cout;
using std::endl;

int
main (int argc, char **argv)
{
    if (octave_main (argc, argv, 1))
    {
        ColumnVector num (2);
        num(0) = 9000;
        num(1) = 1;

        octave_value_list farg; // octave function call argument
        farg(0) = octave_value (num);

        octave_value_list fret = feval ("rand", farg, 1); // call it

        if (fret.length () > 0) {
            Matrix unis (fret(0).matrix_value ());
            cout << __FILE__ << ":" << __LINE__ << ":" << unis << endl;
        } else {
            error ("feval failed");
        }
    } else {
        error ("Octave interpreter initialization failed");
    }
    return error_state ? 1 : 0;
}
