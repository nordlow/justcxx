#include "match.hpp"
#include "show.hpp"
#include "LineReader.hpp"

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

void test_extmatch()
{
    std::string str, ext;

    cout << "str: "; cin >> str;
    cout << "ext: "; cin >> ext;

    cout << "extmatch: " << extmatch(str, ext) << endl;
}

int main(int argc, char * argv[])
{
    LineReader lr("patt: ");
    std::string data(
	"Anna-Karin Lindh "
	"Johan From ");

    std::string patt;
    std::vector<uint> ranks;

    while (true)
    {
	cout << "data: "; cout << data << endl;

	lr.into(patt);
	lr.add_history(patt.c_str());

	cout << "stat: ";
	os_spaces(cout, patt.size() / 2);

	int best_i = fuzzy_find_best_match(data.c_str(), data.size(),
					   patt.c_str(), patt.size(),
					   ranks);

	cout << "best: " << best_i << endl;
	cout << "ranks: " << ranks << endl;

    }

    return 0;
}
