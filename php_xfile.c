#include "php_xfile.h"
#include "php_xfile_rmtree.h"
#include "path.h"
#include "zend_alloc.h"
#include <string.h>

#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_scanpath, 0, 0, 1)
    ZEND_ARG_INFO(0, dir)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_scanpath_dir, 0, 0, 1)
    ZEND_ARG_INFO(0, dir)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lastmtime, 0, 0, 1)
    ZEND_ARG_INFO(0, filelist)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lastctime, 0, 0, 1)
    ZEND_ARG_INFO(0, filelist)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pathsplit, 0, 0, 1)
    ZEND_ARG_INFO(0, path string)
    ZEND_ARG_INFO(0, max split)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pathjoin, 0, 0, 1)
    // pass_by_ref, name, allow_null
    ZEND_ARG_ARRAY_INFO(0, path array, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry xfile_functions[] = {
    PHP_FE(xfile_scanpath, arginfo_scanpath)
    PHP_FE(xfile_scanpath_dir, arginfo_scanpath_dir)
    PHP_FE(xfile_findbin, NULL)
    PHP_FE(xfile_pathjoin, NULL)
    PHP_FE(xfile_pathsplit, arginfo_pathsplit)
    PHP_FE(xfile_paths_append, NULL)
    PHP_FE(xfile_paths_prepend, NULL)
    PHP_FE(xfile_paths_remove_basepath, NULL)
    PHP_FE(xfile_paths_filter_dir, NULL)
    PHP_FE(xfile_paths_filter_file, NULL)
    PHP_FE(xfile_paths_lastmtime, arginfo_lastmtime)
    PHP_FE(xfile_paths_lastctime, arginfo_lastctime)
    PHP_FE(xfile_mtime_compare, NULL)
    PHP_FE(xfile_ctime_compare, NULL)
    PHP_FE(unlink_if_exists, NULL)
    PHP_FE(rmdir_if_exists, NULL)
    PHP_FE(mkdir_if_not_exists, NULL)
    PHP_FE(copy_if_not_exists, NULL)
    PHP_FE(copy_if_newer, NULL)
    PHP_FE(xfile_rmtree, NULL)
    PHP_FE(xfile_ext_replace, NULL)
    PHP_FE(xfile_ext, NULL)
    PHP_FE(xfile_prettysize, NULL)
    PHP_FE(filename_append_suffix, NULL)
    PHP_FE(xfile_files_get_contents, NULL)
    PHP_FE(xfile_files_get_contents_array, NULL)
    {NULL, NULL, NULL}
};


zend_module_entry xfile_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_XFILE_EXTNAME,
    xfile_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_XFILE_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_XFILE
ZEND_GET_MODULE(xfile)
#endif






zend_bool file_exists(char * filename, int filename_len TSRMLS_DC)
{
    zval tmp;
    php_stat(filename, filename_len, FS_EXISTS, &tmp TSRMLS_CC);
    zend_bool ret = Z_LVAL(tmp) ? true : false;
    zval_dtor( &tmp );
    return ret;
}

zend_bool stream_is_dir(php_stream *stream TSRMLS_DC)
{
    return (stream->flags & PHP_STREAM_FLAG_IS_DIR);
}

zend_bool is_dir(char* dirname, int dirname_len TSRMLS_DC)
{
    zval tmp;
    zend_bool ret;
    php_stat(dirname, dirname_len, FS_IS_DIR, &tmp TSRMLS_CC);
    ret = Z_LVAL(tmp) ? true : false;
    zval_dtor( &tmp );
    return ret;
}

zend_bool is_file(char* dirname, int dirname_len TSRMLS_DC)
{
    zval tmp;
    zend_bool ret;
    php_stat(dirname, dirname_len, FS_IS_FILE, &tmp TSRMLS_CC);
    ret = Z_LVAL(tmp) ? true : false;
    zval_dtor( &tmp );
    return ret;
}


zend_bool file_get_contents(char * filename, int filename_len, char **contents, int *contents_len TSRMLS_DC);

zend_bool file_get_contents(char * filename, int filename_len, char **contents, int *contents_len TSRMLS_DC)
{
    zend_bool use_include_path = 0;
    php_stream *stream;

    int len;
    long offset = -1;
    long maxlen = PHP_STREAM_COPY_ALL;
    zval *zcontext = NULL;
    php_stream_context *context = NULL;

    context = php_stream_context_from_zval(zcontext, 0);
    stream = php_stream_open_wrapper_ex(filename, "rb",
                (use_include_path ? USE_PATH : 0) | REPORT_ERRORS,
                NULL, context);
    if (!stream) {
        return false;
    }

    if (offset > 0 && php_stream_seek(stream, offset, SEEK_SET) < 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to seek to position %ld in the stream", offset);
        php_stream_close(stream);
        return false;
    }

    zend_bool ret;

    if ((*contents_len = php_stream_copy_to_mem(stream, contents, maxlen, 0)) > 0) {
        ret = true;
    } else if (len == 0) {
        ret = true;
    } else {
        ret = false;
    }
    php_stream_close(stream);
    return ret;
}



char * dir_dir_entry_handler(
        char* dirname, 
        int dirname_len,
        php_stream_dirent * entry TSRMLS_DC)
{
    char * path = path_concat(dirname, dirname_len, entry->d_name, strlen(entry->d_name) TSRMLS_CC);
    int    path_len = dirname_len + strlen(entry->d_name) + 1;
    if ( ! is_dir(path, path_len TSRMLS_CC) ) {
        efree(path);
        return NULL;
    }
    return path;
}

char* dir_entry_handler(
        char* dirname, 
        int dirname_len, 
        php_stream_dirent * entry TSRMLS_DC)
{
    return path_concat(dirname, dirname_len, entry->d_name, strlen(entry->d_name) TSRMLS_CC);
}


void phpdir_scandir_with_handler(
        zval * z_list,
        php_stream * stream, 
        char* dirname, 
        int dirname_len,
        char* (*handler)(char*, int, php_stream_dirent* TSRMLS_DC) TSRMLS_DC) 
{
    php_stream_dirent entry;
    while (php_stream_readdir(stream, &entry)) {
        if (strcmp(entry.d_name, "..") == 0 || strcmp(entry.d_name, ".") == 0) {
            continue;
        }

        char * newpath;
        if ( (newpath = (*handler)(dirname, dirname_len, &entry TSRMLS_CC)) != NULL ) {
            add_next_index_string(z_list, newpath , 0 );
        }
    }
}







PHP_FUNCTION(xfile_scanpath_dir)
{
    char *dirname;
    int dirname_len;
    php_stream *stream;

    // parse parameters
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dirname, &dirname_len ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Path argument is required.");
        RETURN_FALSE;
    }

    if ( dirname_len < 1 ) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Directory name cannot be empty");
        RETURN_FALSE;
    }

    // run is_dir
    if ( ! is_dir(dirname, dirname_len TSRMLS_CC) )
        RETURN_FALSE;

    stream = php_stream_opendir(dirname, REPORT_ERRORS, NULL );
    if ( ! stream )
        RETURN_FALSE;
    // it's not fclose-able
    stream->flags |= PHP_STREAM_FLAG_NO_FCLOSE;

    array_init(return_value);
    zval_copy_ctor(return_value);
    phpdir_scandir_with_handler(
            return_value,
            stream, 
            dirname, dirname_len, 
            dir_dir_entry_handler TSRMLS_CC);
    php_stream_close(stream);
}

PHP_FUNCTION(xfile_scanpath)
{
    char *dirname;
    int dirname_len;
    php_stream * stream;

    /* parse parameters */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                    &dirname, &dirname_len ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    if ( dirname_len < 1 ) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Directory name cannot be empty");
        RETURN_FALSE;
    }

    // run is_dir
    if ( ! is_dir(dirname, dirname_len TSRMLS_CC) ) {
        RETURN_FALSE;
    }


    stream = php_stream_opendir(dirname, REPORT_ERRORS, NULL );
    if ( ! stream ) {
        RETURN_FALSE;
    }
    // it's not fclose-able
    stream->flags |= PHP_STREAM_FLAG_NO_FCLOSE;

    array_init(return_value);
    phpdir_scandir_with_handler(
            return_value, 
            stream,
            dirname, dirname_len, 
            dir_entry_handler TSRMLS_CC);
    php_stream_close(stream);
}



PHP_FUNCTION(xfile_ctime_compare)
{
    char *filename1, *filename2;
    int   filename1_len, filename2_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                    &filename1, &filename1_len, &filename2, &filename2_len
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }
    zval time1;
    zval time2;
    php_stat(filename1, filename1_len, FS_CTIME, &time1 TSRMLS_CC);
    php_stat(filename2, filename2_len, FS_CTIME, &time2 TSRMLS_CC);

    if (Z_LVAL(time1) > Z_LVAL(time2) ) {
        RETURN_LONG(1);
    } else if (Z_LVAL(time1) == Z_LVAL(time2)) {
        RETURN_LONG(0);
    } else if (Z_LVAL(time1) < Z_LVAL(time2) ) {
        RETURN_LONG(-1);
    }
    RETURN_FALSE;

}

PHP_FUNCTION(xfile_mtime_compare)
{
    char *filename1, *filename2;
    int   filename1_len, filename2_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                    &filename1, &filename1_len, &filename2, &filename2_len
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }
    zval time1;
    zval time2;
    php_stat(filename1, filename1_len, FS_MTIME, &time1 TSRMLS_CC);
    php_stat(filename2, filename2_len, FS_MTIME, &time2 TSRMLS_CC);

    if (Z_LVAL(time1) > Z_LVAL(time2) ) {
        RETURN_LONG(1);
    } else if (Z_LVAL(time1) == Z_LVAL(time2)) {
        RETURN_LONG(0);
    } else if (Z_LVAL(time1) < Z_LVAL(time2) ) {
        RETURN_LONG(-1);
    }
    RETURN_FALSE;
}















PHP_FUNCTION(xfile_paths_lastctime)
{
    zval *zarr;
    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int array_count;
    long lastctime = 0;
    char *path;
    int path_len;
    zval time;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a",
                    &zarr
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    zarr_hash = Z_ARRVAL_P(zarr);
    array_count = zend_hash_num_elements(zarr_hash);

    if ( array_count == 0 )
        RETURN_FALSE;

    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            // for string type, we try to treat it as a path
            path = Z_STRVAL_PP(entry_data);
            path_len  = Z_STRLEN_PP(entry_data);

            php_stat(path, path_len, FS_CTIME, &time TSRMLS_CC);
            if ( Z_LVAL(time) > lastctime ) {
                lastctime = Z_LVAL(time);
            }
        }
    }
    RETURN_LONG(lastctime);
}




/* {{{ proto long xfile_paths_lastmtime(array &array_files)
   Return the last modification time in the file array. */
PHP_FUNCTION(xfile_paths_lastmtime)
{
    zval *zarr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a",
                    &zarr
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    long lastmtime = 0;

    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int array_count;

    char *path;
    int path_len;

    zval mtime;

    zarr_hash = Z_ARRVAL_P(zarr);
    array_count = zend_hash_num_elements(zarr_hash);


    if ( array_count == 0 )
        RETURN_FALSE;


    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            // for string type, we try to treat it as a path
            path = Z_STRVAL_PP(entry_data);
            path_len  = Z_STRLEN_PP(entry_data);

            php_stat(path, path_len, FS_MTIME, &mtime TSRMLS_CC);
            if (mtime.value.lval > lastmtime ) {
                lastmtime = mtime.value.lval;
            }
        }
    }
    RETURN_LONG(lastmtime);
}
/* }}} */



/* {{{ proto array xfile_pathsplit(array &array_files)
   Split the path */
PHP_FUNCTION(xfile_pathsplit)
{
    char *path;
    int  path_len;

    zval zdelim;
    zval zstr;

    long max_split = LONG_MAX;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
                    &path, &path_len, &max_split
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    if ( path_len == 0 )
        RETURN_FALSE;

    array_init(return_value);

    char delim_str[2];
    delim_str[0] = DEFAULT_SLASH;
    delim_str[1] = '\0';

    ZVAL_STRINGL(&zstr, path, path_len, 0);
    ZVAL_STRINGL(&zdelim, delim_str, 1, 0);

    // PHPAPI void php_explode(zval *delim, zval *str, zval *return_value, long limit)
    php_explode(&zdelim, &zstr, return_value, max_split); // LONG_MAX means no limit
}

PHP_FUNCTION(copy_if_newer)
{
    char *source, *target;
    int source_len, target_len;

    zval *zcontext = NULL;
    php_stream_context *context;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "pp|r", &source, &source_len, &target, &target_len, &zcontext) == FAILURE) {
        return;
    }

    if ( file_exists(target, target_len TSRMLS_CC) ) {
        zval source_time;
        zval target_time;
        php_stat(source, source_len, FS_MTIME, &source_time TSRMLS_CC);
        php_stat(target, target_len, FS_MTIME, &target_time TSRMLS_CC);

        // if it's not newer, simply return false
        if (Z_LVAL(source_time) < Z_LVAL(target_time) ) {
            RETURN_FALSE;
        }
    }

    if (php_check_open_basedir(source TSRMLS_CC)) {
        RETURN_FALSE;
    }

    context = php_stream_context_from_zval(zcontext, 0);
    if (php_copy_file_ctx(source, target, 0, context TSRMLS_CC) == SUCCESS) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}



PHP_FUNCTION(copy_if_not_exists)
{
    char *source, *target;
    int source_len, target_len;

    zval *zcontext = NULL;
    php_stream_context *context;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "pp|r", &source, &source_len, &target, &target_len, &zcontext) == FAILURE) {
        return;
    }

    if ( file_exists(target, target_len TSRMLS_CC) ) {
        RETURN_FALSE;
    }

    if (php_check_open_basedir(source TSRMLS_CC)) {
        RETURN_FALSE;
    }

    context = php_stream_context_from_zval(zcontext, 0);
    if (php_copy_file_ctx(source, target, 0, context TSRMLS_CC) == SUCCESS) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}

PHP_FUNCTION(mkdir_if_not_exists)
{
    char *dir;
    int dir_len;
    zval *zcontext = NULL;
    long mode = 0777;
    zend_bool recursive = 0;
    php_stream_context *context;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lbr", &dir, &dir_len, &mode, &recursive, &zcontext) == FAILURE) {
        RETURN_FALSE;
    }

    if ( dir_len == 0 )
        RETURN_FALSE;

    if ( file_exists(dir,dir_len TSRMLS_CC) )
        RETURN_FALSE;

    context = php_stream_context_from_zval(zcontext, 0);
    RETURN_BOOL(php_stream_mkdir(dir, mode, (recursive ? PHP_STREAM_MKDIR_RECURSIVE : 0) | REPORT_ERRORS, context));
}


PHP_FUNCTION(rmdir_if_exists)
{

    char *dir;
    int dir_len;
    zval *zcontext = NULL;
    long mode = 0777;
    zend_bool recursive = 0;
    php_stream_context *context;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lbr", &dir, &dir_len, &mode, &recursive, &zcontext) == FAILURE) {
        RETURN_FALSE;
    }

    if ( dir_len == 0 )
        RETURN_FALSE;

    if ( ! file_exists(dir,dir_len TSRMLS_CC) ) {
        RETURN_FALSE;
    }

    context = php_stream_context_from_zval(zcontext, 0);
    RETURN_BOOL(php_stream_rmdir(dir, REPORT_ERRORS, context));
}



zend_bool unlink_file(char *filename, int filename_len, zval *zcontext TSRMLS_DC)
{
    php_stream_wrapper *wrapper;
    php_stream_context *context = NULL;

    context = php_stream_context_from_zval(zcontext, 0);
    wrapper = php_stream_locate_url_wrapper(filename, NULL, 0 TSRMLS_CC);

    if (!wrapper || !wrapper->wops) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to locate stream wrapper");
        return false;
    }

    if (!wrapper->wops->unlink) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s does not allow unlinking", wrapper->wops->label ? wrapper->wops->label : "Wrapper");
        return false;
    }
    return wrapper->wops->unlink(wrapper, filename, REPORT_ERRORS, context TSRMLS_CC);
}


PHP_FUNCTION(unlink_if_exists)
{
    char *filename;
    int filename_len;
    zval *zcontext = NULL;
    php_stream_wrapper *wrapper;
    php_stream_context *context = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|r", &filename, &filename_len, &zcontext) == FAILURE) {
        RETURN_FALSE;
    }

    if ( filename_len == 0 )
        RETURN_FALSE;

    zval tmp;
    zend_bool ret;
    php_stat(filename, filename_len, FS_EXISTS, &tmp TSRMLS_CC);
    zval_dtor( &tmp );
    if ( Z_LVAL(tmp) == false ) {
        RETURN_FALSE;
    }
    RETURN_BOOL( unlink_file(filename, filename_len, zcontext TSRMLS_CC) );
}




/* filter out dir from paths  */
PHP_FUNCTION(xfile_paths_filter_dir)
{
    zval *zarr;
    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int zarr_count;
    char * path;
    int    path_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarr) == FAILURE) {
        RETURN_FALSE;
    }
    zarr_hash = Z_ARRVAL_P(zarr);
    zarr_count = zend_hash_num_elements(zarr_hash);
    array_init(return_value);
    zval_copy_ctor(return_value);
    if ( zarr_count == 0 ) {
        return;
    }
    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            path = Z_STRVAL_PP(entry_data);
            path_len = Z_STRLEN_PP(entry_data);

            if ( is_dir(path, path_len TSRMLS_CC) ) {
                add_next_index_stringl(return_value, path, path_len, 1);
            }
        }
    }
}



/* filter out dir from paths  */
PHP_FUNCTION(xfile_paths_filter_file)
{
    zval *zarr;
    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int zarr_count;
    char * path;
    int    path_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarr) == FAILURE) {
        RETURN_FALSE;
    }
    zarr_hash = Z_ARRVAL_P(zarr);
    zarr_count = zend_hash_num_elements(zarr_hash);
    array_init(return_value);
    zval_copy_ctor(return_value);
    if ( zarr_count == 0 ) {
        return;
    }
    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            path = Z_STRVAL_PP(entry_data);
            path_len = Z_STRLEN_PP(entry_data);

            if ( is_file(path, path_len TSRMLS_CC) ) {
                add_next_index_stringl(return_value, path, path_len, 1);
            }
        }
    }
}






PHP_FUNCTION(xfile_paths_append)
{
    zval *zarr;

    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int zarr_count;

    char *str_append;
    int   str_append_len;

    char *str;
    int   str_len;

    char *newpath;
    int   newpath_len;

    zend_bool modify = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as|b", &zarr, &str_append, &str_append_len, &modify) == FAILURE) {
        RETURN_FALSE;
    }

    if ( str_append_len == 0 )
        RETURN_FALSE;

    zarr_hash = Z_ARRVAL_P(zarr);
    zarr_count = zend_hash_num_elements(zarr_hash);

    if ( modify ) {
        if (zarr_count == 0) {
            RETURN_FALSE;
        }
    } else {
        array_init(return_value);
        zval_copy_ctor(return_value);
        if ( zarr_count == 0 ) {
            return;
        }
    }


    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            str = Z_STRVAL_PP(entry_data);
            str_len = Z_STRLEN_PP(entry_data);

            newpath = path_concat(str, str_len, str_append, str_append_len TSRMLS_CC);
            newpath_len = strlen(newpath);

            if (modify) {
                // free up the previous string
                efree(Z_STRVAL_PP(entry_data));
                Z_STRVAL_PP(entry_data) = newpath;
                Z_STRLEN_PP(entry_data) = newpath_len;
            } else {
                add_next_index_stringl(return_value, newpath, newpath_len, 0);
            }
        }
    }
}


PHP_FUNCTION(xfile_paths_remove_basepath)
{
    zval *zarr;

    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int zarr_count;

    char *basepath;
    int   basepath_len;

    char *path;
    int   path_len;

    char *newpath;
    int   newpath_len;

    zend_bool modify = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as|b", 
                &zarr, &basepath, &basepath_len, &modify) == FAILURE) {
        RETURN_FALSE;
    }

    if ( basepath_len == 0 )
        RETURN_FALSE;

    zarr_hash = Z_ARRVAL_P(zarr);
    zarr_count = zend_hash_num_elements(zarr_hash);

    if ( modify ) {
        if (zarr_count == 0) {
            RETURN_FALSE;
        }
    } else {
        array_init(return_value);
        zval_copy_ctor(return_value);
        if ( zarr_count == 0 ) {
            return;
        }
    }



    // append DEFAULT_SLASH to basepath
    /*
    char slash[2];
    slash[0] = DEFAULT_SLASH;
    slash[1] = '\0';
    basepath_len++;
    char * tmp = basepath;
    basepath = emalloc( sizeof(char) * basepath_len );
    memcpy(basepath, tmp , basepath_len - 1 );
    memcpy(basepath + basepath_len, slash, 1);
    */

    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            path = Z_STRVAL_PP(entry_data);
            path_len = Z_STRLEN_PP(entry_data);

            // found basepath
            if ( strstr(path, basepath) == path ) {
                newpath_len =  path_len - basepath_len;
                newpath = estrndup(path + basepath_len, newpath_len );
                if ( modify ) {
                    // free up the previous string
                    efree(Z_STRVAL_PP(entry_data));
                    Z_STRVAL_PP(entry_data) = newpath;
                    Z_STRLEN_PP(entry_data) = newpath_len;
                } else {
                    add_next_index_stringl(return_value, newpath, newpath_len, 0);
                }
            }
        }
    }
}




PHP_FUNCTION(xfile_paths_prepend)
{
    zval *zarr;

    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int zarr_count;

    char *str_prepend;
    int   str_prepend_len;

    char *str;
    int   str_len;

    char *newpath;
    int   newpath_len;

    zend_bool modify = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as|b", 
                &zarr, &str_prepend, &str_prepend_len, &modify) == FAILURE) {
        RETURN_FALSE;
    }

    if ( str_prepend_len == 0 )
        RETURN_FALSE;

    zarr_hash = Z_ARRVAL_P(zarr);
    zarr_count = zend_hash_num_elements(zarr_hash);


    if ( modify ) {
        if (zarr_count == 0) {
            RETURN_FALSE;
        }
    }
    if (zarr_count == 0) {
        return;
    }

    array_init(return_value);
    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            str = Z_STRVAL_PP(entry_data);
            str_len = Z_STRLEN_PP(entry_data);

            newpath = path_concat(str_prepend, str_prepend_len, str, str_len TSRMLS_CC);
            newpath_len = strlen(newpath);

            if ( modify ) {
                // free up the previous string
                efree(Z_STRVAL_PP(entry_data));
                Z_STRVAL_PP(entry_data) = newpath;
                Z_STRLEN_PP(entry_data) = newpath_len;
            } else {
                add_next_index_stringl(return_value, newpath, newpath_len, 0);
            }
        }
    }
}






PHP_FUNCTION(xfile_files_get_contents_array)
{
    zval *zarr;
    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int zarr_count;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarr) == FAILURE) {
        RETURN_FALSE;
    }

    zarr_hash = Z_ARRVAL_P(zarr);
    zarr_count = zend_hash_num_elements(zarr_hash);

    array_init(return_value);
    zval_copy_ctor(return_value);

    if (zarr_count == 0)
        return;

    char *filename = NULL;
    int filename_len = 0;

    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            filename = Z_STRVAL_PP(entry_data);
            filename_len = Z_STRLEN_PP(entry_data);


            if ( is_file(filename, filename_len TSRMLS_CC) ) {
                zval *z_newitem;
                MAKE_STD_ZVAL(z_newitem);
                array_init(z_newitem);
                char *contents = NULL;
                int   contents_len = 0;
                if ( file_get_contents(filename, filename_len, &contents, &contents_len TSRMLS_CC) ) {
                    add_assoc_stringl(z_newitem, "content", contents, contents_len, 0);
                    add_assoc_stringl(z_newitem, "path", filename, filename_len, 1);
                }
                add_next_index_zval(return_value, z_newitem);
            }
        }
    }
}








PHP_FUNCTION(xfile_files_get_contents)
{
    zval *zarr;

    zval **entry_data;
    HashTable *zarr_hash;
    HashPosition pointer;
    int zarr_count;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarr) == FAILURE) {
        RETURN_FALSE;
    }

    zarr_hash = Z_ARRVAL_P(zarr);
    zarr_count = zend_hash_num_elements(zarr_hash);

    if (zarr_count == 0)
        RETURN_FALSE;

    char *filename = NULL;
    int filename_len = 0;

    // int   output_cap = 20480 * 2;
    int   output_cap = 8192;
    int   output_len = 0;
    char *output = emalloc(output_cap);

    for(zend_hash_internal_pointer_reset_ex(zarr_hash, &pointer); 
            zend_hash_get_current_data_ex(zarr_hash, (void**) &entry_data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(zarr_hash, &pointer)) 
    {
        if ( Z_TYPE_PP(entry_data) == IS_STRING ) {
            filename = Z_STRVAL_PP(entry_data);
            filename_len = Z_STRLEN_PP(entry_data);


            if ( is_file(filename, filename_len TSRMLS_CC) ) {
                char *contents = NULL;
                int   contents_len = 0;
                if ( file_get_contents(filename, filename_len, &contents, &contents_len TSRMLS_CC) ) {
                    if (output_len + contents_len > output_cap) {
                        output_cap = output_len + contents_len + 8192;
                        output = erealloc(output, output_cap);
                    }
                    memcpy(output + output_len, contents, contents_len );
                    output_len += contents_len;
                    efree(contents);
                }
            }
        }
    }
    if ( output_len >= output_cap ) {
        output = erealloc(output, output_cap + 1);
    }
    *(output + output_len) = '\0';
    RETURN_STRINGL(output, output_len, 0);
}


PHP_FUNCTION(xfile_ext)
{
    char *filename;
    int   filename_len;
    char *dot;

    char *extension;
    int   extension_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                    &filename, &filename_len ) == FAILURE) {
        RETURN_FALSE;
    }

    if ( filename_len == 0 )
        RETURN_FALSE;

    dot = strrchr(filename, (int) '.');
    if ( dot != NULL ) {
        extension_len = filename_len - (dot - filename) - 1;

        if ( extension_len == 0 )
            RETURN_FALSE;

        extension = dot + 1;
        RETURN_STRINGL(extension, extension_len , 1);
    }
    RETURN_FALSE;
}


/* filename_append_suffix( filename, suffix) */
PHP_FUNCTION(filename_append_suffix)
{
    char *filename;
    char *newfilename;
    char *suffix;
    char *dot;

    int   filename_len = 0;
    int   suffix_len = 0;
    int   newfilename_len = 0;

    int   len;
    char *dst;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filename, &filename_len, &suffix, &suffix_len ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    if ( suffix_len  == 0 )
        RETURN_FALSE;

    if ( filename_len == 0 )
        RETURN_FALSE;

    dot = strrchr(filename, (int) '.');
    if ( dot != NULL ) {
        newfilename_len = filename_len + suffix_len;
        newfilename = emalloc( sizeof(char) * (newfilename_len+1) );

        dst = newfilename;
        len = (dot - filename);
        memcpy(dst, filename, len);
        dst += len;

        memcpy(dst, suffix, suffix_len);
        dst += suffix_len;

        len = filename_len - (dot - filename);
        memcpy(dst, dot, len);
        dst += len;
        *dst = '\0';
    } else {
        // simply append the suffix
        newfilename_len = filename_len + suffix_len;
        newfilename = emalloc(sizeof(char) * (newfilename_len + 1));
        memcpy( newfilename , filename , filename_len );
        memcpy( newfilename + filename_len , suffix , suffix_len );
        *(newfilename + newfilename_len) = '\0';
    }
    RETURN_STRINGL(newfilename, newfilename_len, 0);
}

PHP_FUNCTION(xfile_ext_replace)
{
    char *filename;
    int   filename_len;

    char *extension;
    int   extension_len;

    char *newfilename;
    int newfilename_len;

    char *dot;
    char *basename;
    int basename_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                    &filename, &filename_len, &extension, &extension_len
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    if ( filename_len == 0 )
        RETURN_FALSE;


    dot = strrchr(filename, (int) '.');
    if ( dot != NULL ) {
        basename_len = dot - filename + 1; // contains the dot
    } else {
        basename_len = filename_len;
        char *new_extension = emalloc( sizeof(char) * (extension_len + 1) );
        *new_extension = '.';
        strncpy(new_extension + 1, extension, extension_len);
        extension = new_extension;
        extension_len++;
    }

    newfilename_len = basename_len + extension_len;
    newfilename = emalloc((newfilename_len + 1) * sizeof(char));
    memcpy(newfilename, filename, basename_len);
    memcpy(newfilename + basename_len, extension, extension_len );
    *(newfilename + newfilename_len) = '\0';
    RETURN_STRINGL(newfilename, newfilename_len, 0);
}

PHP_FUNCTION(xfile_prettysize)
{
    unsigned long bytes = 0;
    char *str;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                    &bytes
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    str = emalloc(sizeof(char) * 16);
    if ( bytes < SIZE_KB ) {
        sprintf(str, "%ld B", bytes);
    } else if ( bytes < SIZE_MB) {
        sprintf(str, "%ld KB", (unsigned long) (bytes / SIZE_KB) );
    } else if ( bytes < SIZE_GB) {
        sprintf(str, "%.1f MB", (double) (bytes / SIZE_MB) );
    } else {
        sprintf(str, "%.1f GB", (double) (bytes / SIZE_GB));
    }
    RETURN_STRING( str, 0);
}

PHP_FUNCTION(xfile_findbin)
{
    char *bin;
    int   bin_len;

    char  *path = NULL;
    int    path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
                    &bin, &bin_len, &path, &path_len) == FAILURE) {
        RETURN_FALSE;
    }

    if ( path == NULL ) {
        path = getenv("PATH");
        path_len = strlen(path);
    }

    if ( path == NULL ) {
        RETURN_FALSE;
    }

    char *p;
    char  *binpath;
    int   binpath_len;

    p = strtok(path,":");
    binpath = emalloc(sizeof(char) * 128);

    while( p != NULL ) {
        binpath_len = strlen(p) + 1 + bin_len;

        if ( binpath_len > 128 ) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "binpath overflow");
            RETURN_FALSE;
        }

        sprintf(binpath, "%s/%s",p,bin);

        if ( is_file(binpath,binpath_len TSRMLS_CC) ) {
            RETURN_STRINGL(binpath, binpath_len,0);
        }

        p = strtok( NULL ,":");
    }
    RETURN_FALSE;
}


PHP_FUNCTION(xfile_pathjoin)
{
    int num_varargs;
    zval ***varargs = NULL;
    char *newpath;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                    &varargs, &num_varargs
                    ) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
        RETURN_FALSE;
    }

    if ( num_varargs == 1 && Z_TYPE_PP(varargs[0]) == IS_ARRAY ) {
        newpath = path_concat_from_zarray(varargs[0] TSRMLS_CC);
    } else if ( num_varargs > 1  && Z_TYPE_PP(varargs[0]) == IS_STRING ) {
        newpath = path_concat_from_zargs( num_varargs , varargs TSRMLS_CC);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong parameters.");
    }

    if (varargs) {
        efree(varargs);
    }
    RETURN_STRING(newpath,0);
}




