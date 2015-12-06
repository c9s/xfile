#ifndef PHP_XFILE_PATH_H
#define PHP_XFILE_PATH_H

char * path_concat( 
        char* path1, 
        int len1, 
        char * path2 , int len2  TSRMLS_DC);

char* path_concat_fill( 
        char * dst, 
        char * src, 
        int  subpath_len,
        zend_bool remove_first_slash TSRMLS_DC);

char * path_concat_from_zargs( int num_varargs , zval ***varargs TSRMLS_DC);

char* path_concat_from_zarray(zval **arr  TSRMLS_DC);

void path_remove_tailing_slash(char *path TSRMLS_DC);

void path_remove_tailing_slash_n(char *path, int len TSRMLS_DC);

#endif
