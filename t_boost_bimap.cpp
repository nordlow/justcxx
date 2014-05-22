#include <iostream>
#include <string>
#include <boost/bimap.hpp>

int main(int argc, char * argv[], char * envp[])
{
    typedef boost::bimap< int, int > Duals;
    typedef Duals::value_type Dual; // Element
    Duals duals;
    return 0;
}
