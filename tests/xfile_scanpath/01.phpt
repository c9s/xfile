--TEST--
test xfile scanpath
--FILE--
<?php
$paths = xfile_scanpath('modules');
print_r($paths);
--EXPECT--
Array
(
    [0] => modules/xfile.la
    [1] => modules/xfile.so
)
