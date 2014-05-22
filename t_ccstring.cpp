#include "csc.hpp"
#include <iostream>
#include <string>

int main(int argc, char * argv[])
{
    using namespace std;

    {
        const char * str = "small-sample";
        csc s(str);
        csc cs(boost::cref(str));
        cout << "s: " << s << endl
             << "cs: " << cs << endl
            ;
    }

    {
        const char * str = "a-really-really-long-sample";
        //csc s(str);
        csc cs(boost::cref(str));
    }

    return 0;
}
