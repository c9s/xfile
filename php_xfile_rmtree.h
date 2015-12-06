#ifndef PHP_FUTIL_RMTREE_H
#define PHP_FUTIL_RMTREE_H 1


PHP_FUNCTION(rmtree);

int rmtree_iterator(zend_object_iterator *iter, void *puser TSRMLS_DC);

zval *recursive_directory_iterator_create(char * dir, int dir_len, long options TSRMLS_DC);
zval *recursive_iterator_iterator_create(zval * iter, long options TSRMLS_DC);

#endif
