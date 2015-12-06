#ifndef DIRP_H
#define DIRP_H

// phpdir
typedef struct { 
    php_stream_context *context;

    php_stream *stream;

    // zcontext is a zval resource
    zval *zcontext;
} phpdir;

PHPAPI phpdir * phpdir_open(char * dirname TSRMLS_DC);

void phpdir_scandir_with_handler( 
        zval * z_list,
        phpdir * phpdir, 
        char* dirname, 
        int dirname_len, 
        char* (*func)(char*, int, php_stream_dirent* TSRMLS_DC) 
        TSRMLS_DC);

PHPAPI void phpdir_close( phpdir * phpdir  TSRMLS_DC);

PHPAPI char * phpdir_entry_handler( char* dirname, int dirname_len, php_stream_dirent * entry TSRMLS_DC);
PHPAPI char * phpdir_dir_entry_handler( char* dirname, int dirname_len, php_stream_dirent * entry  TSRMLS_DC);

#endif
