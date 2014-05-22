#include </usr/include/PackageKit/packagekit-glib/packagekit.h>

/*!
 * \file t_packagekit.c
 */

/* pkg-config --cflags packagekit-glib */
/* pkg-config --libs packagekit-glib */

int main(int argc, char * argv[])
{
  PkClient *client;
  client = pk_client_new ();
  pk_client_install_package (client, "openoffice-clipart");
  g_object_unref (client);
}
