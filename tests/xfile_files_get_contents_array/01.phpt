--TEST--
Test xfile_files_get_contents_array
--FILE--
<?php
$contents = xfile_files_get_contents_array(['tests/xfile_ext/01.phpt', 'tests/xfile_ext/02.phpt']);
echo count($contents), PHP_EOL;
var_export(array_keys($contents[0]));
--EXPECT--
2
array (
  0 => 'content',
  1 => 'path',
)
