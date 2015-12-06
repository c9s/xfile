#ifndef PHP_XFILE_H
#define PHP_XFILE_H 1
#define PHP_XFILE_VERSION "1.0"
#define PHP_XFILE_EXTNAME "xfile"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend.h"
#include "zend_API.h"
#include "zend_compile.h"
#include "zend_hash.h"
#include "zend_extensions.h"

#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_exceptions.h"
#include "zend_hash.h"
#include "zend_interfaces.h"
#include "zend_operators.h"
#include "zend_qsort.h"
#include "zend_vm.h"


#include <ext/standard/php_standard.h>
#include <ext/standard/php_filestat.h>
#include <ext/standard/php_string.h>
#include <ext/standard/file.h>

// these flags are inside the ext/spl/spl_iterators.c, 
// we can not reuse it by including the header file.
typedef enum {
    RIT_LEAVES_ONLY = 0,
    RIT_SELF_FIRST  = 1,
    RIT_CHILD_FIRST = 2
} RecursiveIteratorMode;

#if HAVE_SPL
#include <ext/spl/spl_array.h>
#include <ext/spl/spl_directory.h>
#include <ext/spl/spl_engine.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/spl/spl_iterators.h>
#endif

PHP_FUNCTION(xfile_findbin);

PHP_FUNCTION(xfile_scanpath);
PHP_FUNCTION(xfile_scanpath_dir);
PHP_FUNCTION(xfile_pathjoin);
PHP_FUNCTION(xfile_pathsplit);

PHP_FUNCTION(paths_append);
PHP_FUNCTION(paths_prepend);
PHP_FUNCTION(paths_remove_basepath);
PHP_FUNCTION(paths_filter_dir);
PHP_FUNCTION(paths_filter_file);

PHP_FUNCTION(paths_lastmtime);
PHP_FUNCTION(paths_lastctime);
PHP_FUNCTION(xfile_mtime_compare);
PHP_FUNCTION(xfile_ctime_compare);

PHP_FUNCTION(unlink_if_exists);
PHP_FUNCTION(rmdir_if_exists);
PHP_FUNCTION(mkdir_if_not_exists);
PHP_FUNCTION(copy_if_not_exists);
PHP_FUNCTION(copy_if_newer);
PHP_FUNCTION(rmtree);
PHP_FUNCTION(xfile_ext_replace);
PHP_FUNCTION(xfile_ext);
PHP_FUNCTION(xfile_prettysize);
PHP_FUNCTION(filename_append_suffix);
PHP_FUNCTION(xfile_files_get_contents);
PHP_FUNCTION(xfile_files_get_contents_array);

#define false 0
#define true 1


extern zend_module_entry xfile_module_entry;
#define phpext_xfile_ptr &xfile_module_entry

#define SIZE_KB (unsigned long)(1024)
#define SIZE_MB (unsigned long)(1024 * 1024)
#define SIZE_GB (unsigned long)(1024 * 1024 * 1024)

zend_bool file_exists(char * filename, int filename_len TSRMLS_DC);
zend_bool stream_is_dir(php_stream *stream TSRMLS_DC);
zend_bool is_dir(char* dirname, int dirname_len TSRMLS_DC);
zend_bool is_file(char* dirname, int dirname_len TSRMLS_DC);
zend_bool unlink_file(char *filename, int filename_len, zval *zcontext TSRMLS_DC);


#endif
