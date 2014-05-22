#include <string.h>
#include "fhsutils.h"

const char *
FHS_PATH_lookupDoc(FHS_PATH_t fhs_path)
{
  const char * doc = NULL;
  switch (fhs_path) {
  case FHS_PATH___: doc = 0; break;
  case FHS_PATH_bin: doc = "Essential command binaries for all users"; break;
  case FHS_PATH_boot: doc = "Boot loader files (e.g., kernels, initrd)"; break;
  case FHS_PATH_dev: doc = "Essential devices"; break;
  case FHS_PATH_etc: doc = "Host-specific system-wide configuration files (from et cetera)"; break;
  case FHS_PATH_etc_opt: doc = "Configuration files for /opt/"; break;
  case FHS_PATH_etc_X11: doc = "(optional) Configuration files for the X Window System, version 11"; break;
  case FHS_PATH_etc_sgml: doc = "(optional) Configuration files for SGML"; break;
  case FHS_PATH_etc_xml: doc = "(optional) Configuration files for XML"; break;
  case FHS_PATH_home: doc = "(optional) Users' home directories"; break;
  case FHS_PATH_lib: doc = "Libraries essential for the binaries in /bin/ and /sbin/"; break;
  case FHS_PATH_mnt: doc = "Temporarily mounted filesystems"; break;
  case FHS_PATH_media: doc = "Mount points for removable media such as CD-ROMs"; break;
  case FHS_PATH_opt: doc = "Static application software packages"; break;
  case FHS_PATH_proc: doc = "Virtual filesystem documenting kernel and process status, mostly text files"; break;
  case FHS_PATH_root: doc = "(optional) Home directory for the root user"; break;
  case FHS_PATH_sbin: doc = "System administrative binaries (system binaries)"; break;
  case FHS_PATH_tmp: doc = "Temporary files"; break;
  case FHS_PATH_srv: doc = "Site-specific data which is served by the system"; break;
  case FHS_PATH_usr: doc = "Secondary hierarchy for shareable, read-only data (unix system resources). This is meant to be shared by multiple hosts and must not contain any host-specific data"; break;
  case FHS_PATH_usr_bin: doc = "Same as for top-level hierarchy"; break;
  case FHS_PATH_usr_include: doc = "Standard include files"; break;
  case FHS_PATH_usr_lib: doc = "Same as for top-level hierarchy"; break;
  case FHS_PATH_usr_sbin: doc = "Same as for top-level hierarchy"; break;
  case FHS_PATH_usr_share: doc = "Architecture-independent (shared) data"; break;
  case FHS_PATH_usr_src: doc = "(optional) Source code"; break;
  case FHS_PATH_usr_X11R6: doc = "(optional) X Window System, Version 11 Release 6"; break;
  case FHS_PATH_usr_local: doc = "Tertiary hierarchy for local data, specific to this host"; break;
  case FHS_PATH_var: doc = "Variable files, such as logs, databases, websites, and temporary e-mail files"; break;
  case FHS_PATH_undefined_: doc = 0; break;
  }
  return doc;
}

FHS_PATH_t
cbuf_lookup_FHS_PATH(const char * pathG, size_t lenG)
{
  FHS_PATH_t fhs_path = FHS_PATH___;
  if (!pathG) { fhs_path = FHS_PATH_undefined_; }
  else if (strncmp(pathG, "/bin", lenG) == 0) { fhs_path = FHS_PATH_bin; }
  else if (strncmp(pathG, "/boot", lenG) == 0) { fhs_path = FHS_PATH_boot; }
  else if (strncmp(pathG, "/dev", lenG) == 0) { fhs_path = FHS_PATH_dev; }
  else if (strncmp(pathG, "/etc", lenG) == 0) { fhs_path = FHS_PATH_etc; }
  else if (strncmp(pathG, "/etc/opt", lenG) == 0) { fhs_path = FHS_PATH_etc_opt; }
  else if (strncmp(pathG, "/etc/X11", lenG) == 0) { fhs_path = FHS_PATH_etc_X11; }
  else if (strncmp(pathG, "/etc/sgml", lenG) == 0) { fhs_path = FHS_PATH_etc_sgml; }
  else if (strncmp(pathG, "/etc/xml", lenG) == 0) { fhs_path = FHS_PATH_etc_xml; }
  else if (strncmp(pathG, "/home", lenG) == 0) { fhs_path = FHS_PATH_home; }
  else if (strncmp(pathG, "/lib", lenG) == 0) { fhs_path = FHS_PATH_lib; }
  else if (strncmp(pathG, "/mnt", lenG) == 0) { fhs_path = FHS_PATH_mnt; }
  else if (strncmp(pathG, "/media", lenG) == 0) { fhs_path = FHS_PATH_media; }
  else if (strncmp(pathG, "/opt", lenG) == 0) { fhs_path = FHS_PATH_opt; }
  else if (strncmp(pathG, "/proc", lenG) == 0) { fhs_path = FHS_PATH_proc; }
  else if (strncmp(pathG, "/root", lenG) == 0) { fhs_path = FHS_PATH_root; }
  else if (strncmp(pathG, "/sbin", lenG) == 0) { fhs_path = FHS_PATH_sbin; }
  else if (strncmp(pathG, "/tmp", lenG) == 0) { fhs_path = FHS_PATH_tmp; }
  else if (strncmp(pathG, "/srv", lenG) == 0) { fhs_path = FHS_PATH_srv; }
  else if (strncmp(pathG, "/usr", lenG) == 0) { fhs_path = FHS_PATH_usr; }
  else if (strncmp(pathG, "/usr/bin", lenG) == 0) { fhs_path = FHS_PATH_usr_bin; }
  else if (strncmp(pathG, "/usr/include", lenG) == 0) { fhs_path = FHS_PATH_usr_include; }
  else if (strncmp(pathG, "/usr/lib", lenG) == 0) { fhs_path = FHS_PATH_usr_lib; }
  else if (strncmp(pathG, "/usr/sbin", lenG) == 0) { fhs_path = FHS_PATH_usr_sbin; }
  else if (strncmp(pathG, "/usr/share", lenG) == 0) { fhs_path = FHS_PATH_usr_share; }
  else if (strncmp(pathG, "/usr/src", lenG) == 0) { fhs_path = FHS_PATH_usr_src; }
  else if (strncmp(pathG, "/usr/X11R6", lenG) == 0) { fhs_path = FHS_PATH_usr_X11R6; }
  else if (strncmp(pathG, "/usr/local", lenG) == 0) { fhs_path = FHS_PATH_usr_local; }
  return fhs_path;
}

FHS_PATH_t
cstr_lookup_FHS_PATH(const char * pathG)
{
  size_t lenG = pathG ? strlen(pathG) : 0;
  return cbuf_lookup_FHS_PATH(pathG, lenG);
}

FHS_PATH_t
csc_lookup_FHS_PATH(const csc & pathG)
{
  return cbuf_lookup_FHS_PATH(pathG.data(), pathG.size());
}
