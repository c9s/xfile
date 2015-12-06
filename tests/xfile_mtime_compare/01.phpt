--TEST--
test xfile_mtime_compare
--FILE--
<?php
$ret = xfile_mtime_compare("config.m4", "php_xfile.c");
--EXPECT--
