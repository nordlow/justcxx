/*!
 * \file t_fstream.cpp
 * \brief Test fstream.
 */

#include <fstream>
#include <iostream>
#include <string>

using std::cout;
using std::endl;

int main(int argc, const char * argv[], const char * envp[]) {
    cout << "sizeof(ifstream):" << sizeof(std::ifstream) << endl;
    cout << "sizeof(ofstream):" << sizeof(std::ofstream) << endl;
    cout << "sizeof(fstream):" << sizeof(std::fstream) << endl;
    return 0;
}
