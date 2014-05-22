#include "bit_matrix.hpp"
#include "sortn.hpp"

void bit_matrix::zero()
{
    for (auto d = begin(data); d != end(data); d++)
    {
        *d = 0;
    }
}

// \todo TODO
#if 0
void bit_matrix::random_adjacency()
{
    if (! is_square())
    {
        return;
    }

    zero();

    int side = n_rows();

    if (side < 2)
    {
        return;                 // Nothing to do.
    }

    Trip1D trip(side);
    std::random_shuffle(begin(trip), end(trip)); // \todo

    auto t0 = begin(trip);
    auto t1 = t0;
    for (int c = 0; c < side; c++)
    {
        if (++t1 == end(trip))
        {
            t1 = begin(trip);
        }

        size_t a = *t0;
        size_t b = *t1;

        sort2(a, b);
        set_bit(a, b);

        // cycle through

        if (++t0 == end(trip))
        {
            t0 = begin(trip);
        }
    }
}
#endif

void bit_matrix::bidirect()
{
    if (! is_square())
    {
        return;
    }
}

void bit_or(bit_matrix& a, const bit_matrix& b, const bit_matrix& c)
{
    const size_t m = std::min(b.n_rows(), c.n_rows());
    const size_t n = std::min(b.n_cols(), c.n_cols());

    a.resize(m, n);

    for (size_t i = 0; i < m; i++)
    {
	for (size_t j = 0; j < n; j++)
	{
	    a.put_bit(i, j,
		       b(i, j) bitor c(i, j));
	}
    }
}

void bit_and(bit_matrix& a, const bit_matrix& b, const bit_matrix& c)
{
    const size_t m = std::min(b.n_rows(), c.n_rows());
    const size_t n = std::min(b.n_cols(), c.n_cols());

    a.resize(m, n);

    for (size_t i = 0; i < m; i++)
    {
	for (size_t j = 0; j < n; j++)
	{
	    a.put_bit(i, j,
		       b(i, j) bitand c(i, j));
	}
    }
}

void bit_xor(bit_matrix& a, const bit_matrix& b, const bit_matrix& c)
{
    const size_t m = std::min(b.n_rows(), c.n_rows());
    const size_t n = std::min(b.n_cols(), c.n_cols());

    a.resize(m, n);

    for (size_t i = 0; i < m; i++)
    {
	for (size_t j = 0; j < n; j++)
	{
	    a.put_bit(i, j,
		       b(i, j) ^ c(i, j));
	}
    }
}
