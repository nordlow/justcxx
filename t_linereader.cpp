#include "LineReader.hpp"

#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char * argv[])
{
    LineReader lr(">> ");

    std::string l;
    const char * history_filename = "t_linereader_history";

    lr.read_history(history_filename);

    while (true)
    {
        if (lr.into(l))
        {
            cout << "read: " << l << endl;
        }
        else
        {
            break;
        }

        lr.add_history(l.c_str());

	if (l == "list")
        {
            lr.print_history(cout);
        }
    }

    lr.write_history(history_filename);

    return 0;
}
