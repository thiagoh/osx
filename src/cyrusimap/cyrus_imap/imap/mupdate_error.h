/*
 * mupdate_err.h:
 * This file is automatically generated; please do not edit it.
 */

extern const struct error_table et_mupd_error_table;

#define MUPDATE_FAIL                             (-1627742720L)
#define MUPDATE_FAIL_RESERVE                     (-1627742719L)
#define MUPDATE_NOCONN                           (-1627742718L)
#define MUPDATE_NOAUTH                           (-1627742717L)
#define MUPDATE_TIMEOUT                          (-1627742716L)
#define MUPDATE_BADPARAM                         (-1627742715L)
#define MUPDATE_PROTOCOL_ERROR                   (-1627742714L)
#define MUPDATE_MAILBOX_UNKNOWN                  (-1627742713L)
extern void initialize_mupd_error_table ();
#define ERROR_TABLE_BASE_mupd (-1627742720L)

/* for compatibility with older versions... */
#define init_mupd_err_tbl initialize_mupd_error_table
#define mupd_err_base ERROR_TABLE_BASE_mupd
