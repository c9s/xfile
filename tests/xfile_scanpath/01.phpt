--TEST--
test xfile scanpath
--FILE--
<?php
$paths = xfile_scanpath('tests');
print_r($paths);
--EXPECT--
Array
(
    [0] => tests/data
    [1] => tests/xfile_pretty_size
    [2] => tests/xfile_scanpath
)
