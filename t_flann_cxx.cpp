#include <flann/flann.hpp>
#include <flann/io/hdf5.h>

#include <iostream>

int main(int argc, char** argv)
{
    using namespace flann;

    int nn = 3;

    typedef Matrix<float> Mf;
    typedef Matrix<int> Mi;

    Mf dataset;
    Mf query;
    load_from_file(dataset, "dataset.hdf5","dataset");
    load_from_file(query, "dataset.hdf5","query");

    Mi indices(new int[query.rows*nn], query.rows, nn);
    Mf dists(new float[query.rows*nn], query.rows, nn);

    // construct an randomized kd-tree index using 4 kd-trees
    Index<L2<float> > index(dataset, flann::KDTreeIndexParams(4));
    index.buildIndex();

    // do a knn search, using 128 checks
    index.knnSearch(query, indices, dists, nn, flann::SearchParams(128));

    flann::save_to_file(indices,"result.hdf5","result");

    delete[] dataset.ptr();
    delete[] query.ptr();
    delete[] indices.ptr();
    delete[] dists.ptr();

    return 0;
}
