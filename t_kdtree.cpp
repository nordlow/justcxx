#include <iostream>
#include <vector>
#include <algorithm>
#include "algorithm_x.hpp"
#include "show.hpp"
#include "geometry/shape.hpp"
#include "kdtree/kdtree.hpp"
#include "Timer.hpp"
#include "rand.hpp"

// ============================================================================

using std::cout;
using std::cin;
using std::endl;

namespace kdtree
{

const bool show_bars = false;
const double fac = 200;

template<typename T, template<typename> class C>
void insert_test(tree<T> & tr,
		 std::vector<C<T>*> grid,
		 uint m,
		 uint n,
		 bool show)
{
    Timer timer;
    for (uint k = 0; k < m * n; k++)
    {
	C<T> * elm = grid[k];
	if (show)
	{
	    cout << "inserting " << *elm
		 << " nr:" << k << "/" << m*n << "...";
	}
	tr.insert(elm);
	if (false)
	{
	    tr.print(cout, true);
	    os_sep(cout);
	}
    }
    timer.stop();

    cout << "Insertion " << timer << " ";

    if (show_bars)
    {
	uint x =
	    static_cast<uint>(rint(timer.get_elapsed() * fac));
	os_chars(cout, x, '#');
    }

    cout << endl;
}

template<typename T, template<typename> class C>
void find_test(tree<T> & tr,
		 std::vector<C<T>*> grid,
		 uint m,
		 uint n,
		 bool show)
{
    Timer timer;

    tr.reset_fbs_stat();

    uint hitnum = 0;
    for (uint k = 0; k < m * n; k++)
    {
	const C<T> * elm = grid[k];

	if (show)
	{
	    cout << "finding " << *elm
		 << " nr:" << k << "/" << m*n << "...";
	}

	node<T> * hit = tr.find(elm);

	if (hit)
	{
	    hitnum++;
	}

	if (show)
	{
	    cout << (hit ? "HIT" : "MISS") << endl;
	    hit->print(cout, 0);
	}

	if (hit)
	{
	    hit->tree_delete(false); // clear find result but not the shape leaves
	}
    }
    timer.stop();

    cout << "Find "
	 << timer
	 << " (" << hitnum
	 << ") "
	 << ((hitnum == grid.size()) ? "SUCCESS" : "FAILURE")
	 << " ";

    if (show_bars)
    {
	uint x =
	    static_cast<uint>(rint(timer.get_elapsed() * fac));
	os_chars(cout, x, '#');
    }

    cout << endl;
}

template<typename T, template<typename> class C>
void remove_test(tree<T> & tr,
		 std::vector<C<T>*> grid,
		 uint m,
		 uint n,
		 bool show)
{
    Timer timer;

    uint rmnum = 0;
    for (uint k = 0; k < m * n; k++)
    {
	C<T> * elm = grid[k];

	if (show)
	{
	    cout << "removing " << *elm
		 << " nr:" << k << "/" << m*n << "...";
	}

	bool ok = tr.remove(elm);

	if (ok)
	{
	    rmnum++;
	}

	if (show)
	{
	    cout << (ok ? "HIT" : "MISS") << endl;
	}
    }
    timer.stop();

    cout << "Remove "
	 << " " << timer
	 << " (" << rmnum
	 << ") " << ((rmnum == grid.size()) ? "SUCCESS" : "FAILURE")
	 << " ";

    if (show_bars)
    {
	uint x =
	    static_cast<uint>(rint(timer.get_elapsed() * fac));
	os_chars(cout, x, '#');
    }

    cout << endl;
}

template<typename T>
inline void testgrid(std::vector<vec2<T>*> & a,
		     T step, T dim,
		     uint m,
		     uint n,
		     bool random)
{
    a.resize(m * n);

    // create a rectangular grid of boxes
    for (uint i = 0; i < m; i++)
    {
	for (uint j = 0; j < n; j++)
	{
	    if (random)
	    {
		a[i * n + j] = new vec2<T>(
		    step * i + dim * rrand(0.0, 0.25),
		    step * j + dim * rrand(0.0, 0.25));
	    }
	    else
	    {
		a[i * n + j] = new vec2<T>(step * i,
					   step * j);
	    }
	}
    }
}

template<typename T>
inline void testgrid(std::vector<box2<T>*> & a,
		     T step, T dim,
		     uint m,
		     uint n,
		     bool random)
{
    a.resize(m * n);

    // create a rectangular grid of boxes
    for (uint i = 0; i < m; i++)
    {
	for (uint j = 0; j < n; j++)
	{
	    if (random)
	    {
		a[i * n + j] = new box2<T>(
		    step * i + dim * rrand(0.0, 0.25),
		    step * j + dim * rrand(0.0, 0.25),
		    step * i + dim * rrand(0.5, 0.75),
		    step * j + dim * rrand(0.5, 0.75));
	    }
	    else
	    {
		a[i * n + j] = new box2<T>(
		    step * i,
		    step * j,
		    step * i + dim,
		    step * j + dim);
	    }
	}
    }
}

template<typename T>
inline void testgrid(std::vector<shape2<T>*> & a,
		     T step, T dim,
		     uint m,
		     uint n,
		     bool random)
{
    a.resize(m * n);

    // create a rectangular grid of boxes
    for (uint i = 0; i < m; i++)
    {
	for (uint j = 0; j < n; j++)
	{
	    if (random)
	    {
		a[i * n + j] = new shape2<T>(
		    box2<T>(step * i + dim * rrand(0.0, 0.25),
			    step * j + dim * rrand(0.0, 0.25),
			    step * i + dim * rrand(0.5, 0.75),
			    step * j + dim * rrand(0.5, 0.75)),
		    PAPAYA_WHIP);
	    }
	    else
	    {
		a[i * n + j] = new shape2<T>(box2<T>(step * i,
						     step * j,
						     step * i + dim,
						     step * j + dim),
					     PAPAYA_WHIP);
	    }
	}
    }
}

template<typename T, template<typename> class C>
void major_test(T step, T dim,
		uint m,
		uint n,
		bool random = false,
		bool shuffle = false,
		bool show = false,
		bool end_show = false)
{
    std::vector<C<T>*> grid;
    testgrid(grid, step, dim, m, n, random);

    // shuffle the boxes around
    if (shuffle)
    {
	cout << "Shuffle" << endl;
	std::random_shuffle(begin(grid), end(grid));
    }

    // create the tree
    tree<T> tr;

    // we need a timer for benchmarkings
    Timer timer;

    // standard search
    timer.reset();
    for (uint k = 0; k < m*n; k++)
    {
	for (uint l = 0; l < n*n; l++)
	{
	    if (grid[k] == grid[l])
	    {
		break;
	    }
	}
    }
    timer.stop();
    cout << "Scan " << timer << "." << endl;

    // insert them into tree
    insert_test(tr, grid, m, n, show);
    // search them in the tree
    find_test(tr, grid, m, n, show);
    // print it
    tr.print(cout, end_show);
    // remove them from the tree
    remove_test(tr, grid, m, n, show);
    // print it
    tr.print(cout, end_show);

    // remove boxes
    timer.restart();
    delete_all_and_clear(grid);
    timer.stop();
    cout << "Remove (grid) " << timer
	 << "." << endl;

    cout << endl;
}

}

int main(int argc, char *argv[])
{
    while (true)
    {
	uint m = 4, n = 4;
	bool random = false, shuffle = true;

	cout << "m: "; cin >> m;
	cout << "n: "; cin >> n;
	cout << "shuffle: "; cin >> shuffle;

	float step = 2.0, dim = 1.0;

	cout << "step: "; cin >> step;
	cout << "dim: "; cin >> dim;

	bool part_show = false;
	cout << "part_show: "; cin >> part_show;

	bool end_show = false;
	cout << "end_show: "; cin >> end_show;

	uint niters = 1;
	cout << "niters: "; cin >> niters;

	for (uint i = 0; i < niters; i++)
	{
	    kdtree::major_test<float, vec2>(step, dim, m, n,
					    random, shuffle,
					    part_show, end_show);

	    kdtree::major_test<float, shape2>(step, dim, m, n,
					      random, shuffle,
					      part_show, end_show);
	}

    }

    return 0;
}
