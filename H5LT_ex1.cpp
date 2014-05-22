#include <H5LT.h>

/*!
 * Loads in från Octave:
 * > load ex1.hdf
 * > dset
 */
int main(void)
{
  hsize_t dims[2] = { 2, 3 };   /* 2 columns, 3 rows */
  int     data[6] = { 1, 2, 3, 4, 5, 6 };
  hid_t file_id = H5Fcreate ("ex1.hdf", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  herr_t status = H5LTmake_dataset(file_id, "/dset", 2, dims, H5T_NATIVE_INT, data);
  status = H5Fclose (file_id);
  return 0;
}
