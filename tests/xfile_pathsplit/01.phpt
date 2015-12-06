--TEST--
test xfile_pathsplit
--FILE--
<?php
$parts = xfile_pathsplit('/foo/bar/zoo');
var_export($parts);
--EXPECT--
array (
  0 => '',
  1 => 'foo',
  2 => 'bar',
  3 => 'zoo',
)
