--TEST--
test xfile_paths_append
--FILE--
<?php
$paths = xfile_paths_append([ 
    "dir1",
    "dir2",
    "dir3",
    "dir4/",
], "/subdir");
var_export($paths);
--EXPECT--
array (
  0 => 'dir1/subdir',
  1 => 'dir2/subdir',
  2 => 'dir3/subdir',
  3 => 'dir4/subdir',
)
