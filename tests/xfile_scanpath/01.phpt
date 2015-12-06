--TEST--
test xfile scanpath
--FILE--
<?php
@mkdir('tmp', 0755);
touch('tmp/1');
touch('tmp/2');
touch('tmp/foo');
touch('tmp/bar');
$paths = xfile_scanpath('tmp');
sort($paths);
var_export($paths);
xfile_rmtree('tmp');
--EXPECT--
array (
  0 => 'tmp/1',
  1 => 'tmp/2',
  2 => 'tmp/bar',
  3 => 'tmp/foo',
)
