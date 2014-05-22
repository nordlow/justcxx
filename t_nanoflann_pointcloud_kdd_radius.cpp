/*!
 * \file t_nanoflann_pointcloud_kdd_radius.cpp
 * \todo Calculate Total Center and Mass of Internal Nodes upon insertion. Two more adds should be cheap.
 * \todo Use Mass-Weighted Standard Deviation Radius from Center instead of just radius as used in standard Barnes-Hut.
 * \see https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation
 * \see http://arborjs.org/docs/barnes-hut
 */

#include <iostream>
#include <chrono>
#include <array>
#include <nanoflann.hpp>
#include "geometry/vec.hpp"
#include "numeric_x.hpp"

using namespace std;
using namespace nanoflann;

typedef std::chrono::high_resolution_clock C;

template<class T, size_t N>
struct PointCloud
{
    typedef vec<T,N> Point;
    std::vector<Point> pts;

    // Must return the number of data points
    size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
    T kdtree_distance(const T *p1, const size_t idx_p2, size_t size) const
    {
        return sqrnorm(*p1 - pts[idx_p2]);
    }

    // Returns the dim'th component of the idx'th point in the class:
    T kdtree_get_pt(const size_t idx, int dim) const { return pts[idx][dim];}

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    // Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    // Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template<class BBOX> bool kdtree_get_bbox(BBOX &bb) const { return false; }
};

/*!
 * Generate Random Point Cloud.
 */
template<class T, size_t N>
void generateRandomPointCloud(PointCloud<T, N> &pc, const size_t nPoints, const T max_range = 10)
{
    pc.pts.resize(nPoints);
    for (size_t i = 0; i < nPoints; i++) {
        pc.pts[i][0] = max_range * (rand() % 1000) / T(1000);
        pc.pts[i][1] = max_range * (rand() % 1000) / T(1000);
        pc.pts[i][2] = max_range * (rand() % 1000) / T(1000);
    }
}

template<class T, size_t N>
void kdtree_demo(const size_t nPoints)
{
    PointCloud<T, N> pc;
    generateRandomPointCloud(pc, nPoints);

    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<T, PointCloud<T, N> >, PointCloud<T, N>, N> KDT;

    // Benchmark dot product just for comparison
    {
        const auto tA = C::now();
        auto dp = std::dot(pc.pts, pc.pts);
        const auto count = std::chrono::duration_cast<std::chrono::milliseconds>(C::now() - tA).count();
        cout << "dot of " << nPoints << " took " << count << "ms\n";
    }

    // Benchmark
    const auto tA = C::now();
    const auto max_leaf = 10;
    KDT ix(N, pc, KDTreeSingleIndexAdaptorParams(max_leaf) );
    ix.buildIndex();
    const auto count = std::chrono::duration_cast<std::chrono::milliseconds>(C::now() - tA).count();
    cout << "Building of " << nPoints << "-point tree took " << count << "ms\n";

#if 0
    // Test resize of dataset and rebuild of ix:
    pc.pts.resize(pc.pts.size()*0.5);
    ix.buildIndex();
#endif

    const std::array<T,N> query_pt = {{ 0.5, 0.5 }};

    // Search for the \c nPoints closest points
    {
        const size_t num_results = 5;
        std::vector<size_t> ret_index(num_results);
        std::vector<T> out_dist_sqr(num_results);
        ix.knnSearch(&query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);

        cout << "knnSearch(): num_results=" << num_results << "\n";
        for (size_t i=0;i<num_results;i++)
            cout << "idx["<< i << "]=" << ret_index[i] << " dist["<< i << "]=" << out_dist_sqr[i] << endl;
        cout << "\n";
    }

    // Search for the \c nPoints closest points
    {
        const T search_radius = static_cast<T>(0.1);
        std::vector<std::pair<size_t,T> > ret_matches;

        nanoflann::SearchParams params;
        //params.sorted = false;

        const size_t nMatches = ix.radiusSearch(&query_pt[0],search_radius, ret_matches, params);

        cout << "radiusSearch(): radius=" << search_radius << " -> " << nMatches << " matches\n";
        for (size_t i=0;i<nMatches;i++)
            cout << "idx["<< i << "]=" << ret_matches[i].first << " dist["<< i << "]=" << ret_matches[i].second << endl;
        cout << "\n";
    }

}

int main(int argc, char** argv)
{
    typedef float T;
    kdtree_demo<T, 2>(25000);
    kdtree_demo<T, 2>(50000);
    kdtree_demo<T, 2>(100000);
    kdtree_demo<T, 2>(200000);
    return 0;
}
