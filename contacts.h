#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * A contact.
 */
typedef struct
{
  char *name;
  char *email;
  char *homepage;
  char *address;
  char *home_phone;
  char *work_phone;
  char *cell_phone;
} contact_t;

extern contact_t contacts[];

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
