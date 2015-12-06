--TEST--
test xfile_ctime_compare
--FILE--
<?php
$ret = xfile_ctime_compare("config.m4", "php_xfile.c");
--EXPECT--
