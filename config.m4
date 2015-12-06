
PHP_ARG_ENABLE(xfile,
    [Whether to enable the "xfile" extension],
    [  --enable-xfile      Enable "xfile" extension support])

if test $PHP_XFILE != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(XFILE_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, XFILE_SHARED_LIBADD)
    PHP_NEW_EXTENSION(xfile, php_xfile.c php_xfile_rmtree.c path.c, $ext_shared)
fi
