#include <iostream>
#include <fstream>
#include <vector>
#include "algorithm_x.hpp"
#include "show.hpp"

using std::cout;
using std::cerr;
using std::endl;

int trylower(int a)
{
    if (std::isupper(a))
    {
	a = std::tolower(a);
    }
    return a;
}

/*!
 * Perform a humane search using histogram matching.
 *
 * Inputs:
 * - a: Source to from.
 * - b: Pattern.
 *
 * Outputs:
 * - c: The indexes of the hits.
 * - d: and their corresponding fits where each fitting value lies in [0 255].
 */
static void humane_search(const char * path,
			  const std::vector<uchar> & a,
			  const std::vector<uchar> & b,
			  std::vector<uint> & c,
			  std::vector<uint> & d)
{
    c.resize(0);
    d.resize(0);

    // source is smaller than pattern => source cannot contain pattern
    if (a.size() < b.size()) { return; }

    // number of positions of b relative to a where a match should be performed
    const uint an = a.size();
    const uint bn = b.size();
    const uint n = an - bn + 1;

    uint hn = 1 << std::bitsizeof<uchar>();
    // initial source block histogram
    std::vector<uint> ah(hn);
    for (uint i = 0; i < bn; i++)
    {
	ah[trylower(a[i])]++;
    }

    // pattern histogram
    std::vector<uint> bh(hn);
    for (uint i = 0; i < bn; i++)
    {
	bh[trylower(b[i])]++;
    }

    int cl = 1;			// current line
    for (uint i = 0; i < n; i++)
    {
	if (a[i] == 10) { cl++; } // next line

	uint match = 0;
	uint match_max = 0;

	// Mät den gemensamma förekomsten av tecken oberoende av position.
	for (uint j = 0; j < hn; j++)
	{
	    match += std::min(ah[j], bh[j]);
	}
	match_max += bn;

	// Mät antalet bokstäver som stämmer överens i position.
	for (uint j = 0; j < bn; j++)
	{
	    match += 2 * (trylower(a[i+j]) ==
			  trylower(b[j]));
	}
	match_max += 2 * bn;

	if (match >= 3 * match_max / 4)
	{
	    cout << path << ":" << cl
		 << ": match:" << match
		 << "/" << match_max
		 << endl;
	    c.push_back(i);
	    c.push_back(match);
	}

	ah[trylower(a[i])]--;	// remove first

	if (i + 1 < n)
	{
	    ah[trylower(a[i + 1])]++; // append last upcoming
	}
    }

}

int main(int argc, char *argv[])
{
    if (argc != 1 + 2)
    {
	cout << "usage: " << argv[0] << " pattern file" << endl;
        return 0;
    }

    const char * pattern = argv[1];
    const char * path = argv[2];

    std::ifstream is(path,
		      std::ios::in |
		      std::ios::binary);
    if (not is)
    {
	cerr << "error: could not open input file " << path << endl;
	return 0;
    }

    std::vector<uchar> a;
    bool binary = false;
    while (true)
    {
	char ch;
	is.get(ch);
	if (is.eof()) { break; }

	ch = std::tolower(ch);
	a.push_back(*reinterpret_cast<uchar *>(&ch));
	if (ch != 0 and
	    not (std::isprint(ch) or std::isspace(ch)))
	{
	    binary = true;
	}
    }

    std::vector<uchar> b;
    pnw::copy_cstr(b, pattern);

    std::vector<uint> c, d;

    humane_search(path, a, b, c, d);

    return 0;
}
