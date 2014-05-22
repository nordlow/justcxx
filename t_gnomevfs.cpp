#include <stdio.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-utils.h>

#define BYTES_TO_PROCESS 256

int print_error (GnomeVFSResult result, const char *uri_string);

int
main (int argc, char **argv)
{
  GnomeVFSHandle *read_handle, *write_handle;
  char *input_uri_string;
  char *output_uri_string = argv[2];
  GnomeVFSFileSize bytes_read, bytes_written;
  guint buffer[BYTES_TO_PROCESS];
  GnomeVFSResult result;

  /* remember to initialize GnomeVFS! */
  if (not gnome_vfs_init ()) {
    printf ("Could not initialize GnomeVFS\n");
    return 1;
  }

  /* Convert the user supplied filenames into proper GnomeVFS URIs */
  input_uri_string = gnome_vfs_make_uri_from_shell_arg (argv[1]);
  output_uri_string = gnome_vfs_make_uri_from_shell_arg (argv[2]);

  /* open the input file for read access */
  result = gnome_vfs_open (&read_handle, input_uri_string, GNOME_VFS_OPEN_READ);
  /* if the operation was not successful, print the error and abort */
  if (result != GNOME_VFS_OK) return print_error (result, input_uri_string);

  /* we use create instead of open,
   * because open will not create the file if it does
   * not already exist. The last argument is the permissions to use if the
   * file is created,
   * the second to last tells GnomeVFS that its ok if the file already exists,
   * and just open it */
  result = gnome_vfs_create (&write_handle, output_uri_string,
			     GNOME_VFS_OPEN_WRITE, FALSE, 0777);
  if (result != GNOME_VFS_OK) return print_error (result, output_uri_string);

  /* read data from the input uri */
  result = gnome_vfs_read (read_handle, buffer, BYTES_TO_PROCESS, &bytes_read);
  if (result != GNOME_VFS_OK) return print_error (result, input_uri_string);

  /* seek to the end of the output uri so we will append rather than overwrite
   * therefore, we seek 0 bytes relative to the end of the file */
  result = gnome_vfs_seek (write_handle, GNOME_VFS_SEEK_END, 0);

  /* now write the data we read out to the output uri */
  result = gnome_vfs_write (write_handle, buffer, bytes_read, &bytes_written);
  if (result != GNOME_VFS_OK) return print_error (result, output_uri_string);

  g_free (input_uri_string);
  g_free (output_uri_string);

  return 0;
}

int
print_error (GnomeVFSResult result, const char *uri_string)
{
  const char *error_string;
  /* get the string corresponding to this GnomeVFSResult value */
  error_string = gnome_vfs_result_to_string (result);
  printf ("Error %s occured opening location %s\n", error_string, uri_string);
  return 1;
}
