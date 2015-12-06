--TEST--
test xfile_paths_prepend
--FILE--
<?php
$paths = xfile_paths_prepend([ 
    "file1",
    "file2",
    "file3",
    "/file4",
], "/root");
var_export($paths);
--EXPECT--
array (
  0 => '/root/file1',
  1 => '/root/file2',
  2 => '/root/file3',
  3 => '/root/file4',
)
