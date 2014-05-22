#include "gec/gec.cpp"
#include "show.hpp"
#include "rand.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char * argv[])
{
    using namespace gec;

    // population size count (number of individuals)
    size_t count = 10000;

    size_t nops = 8;

    Pop pop(count, nops);
    pop.random_nets();

    // training vector
    const size_t vn = 32;
    std::vector<bool> v(vn);

    // type of training pattern
    size_t train = 5;

    switch (train) {
    case 0: rand(v); break;
    case 1: for (size_t i = 0; i < v.size(); i++) { v[i] = 0; } break;
    case 2: for (size_t i = 0; i < v.size(); i++) { v[i] = i & 1; } break;
    case 3: for (size_t i = 0; i < v.size(); i++) { v[i] = ((i & (1 << 0)) ^ (i & (1 << 1))); } break;
    case 4: for (size_t i = 0; i < v.size(); i++) { v[i] = i & (1 << 3); } break;
    case 5: for (size_t i = 0; i < v.size(); i++) { v[i] = i & (1 << 4); } break;
    default: break;
    }

    cout << "vn:" << vn
         << " v:" << v << endl;

    while (true) {
        pop.reset_stat();
        pop.reset_nets();
        pop.multi_evaluate_hamming_to(v);
        pop.calc_arcstat();
        pop.rank();

        cout << pop << endl;
        if (pop.nets.size() > 0) {
            cout << "best: " << *pop.nets[0] << endl;
            UIntV a; pop.read_opnums(a);
            cout << "opnums:" << a << endl;
        }
        cout << endl << endl;

        pop.evolve();

    }

    return 0;
}
