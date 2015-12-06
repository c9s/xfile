--TEST--
test xfile_pathsplit
--FILE--
<?php
$path = xfile_pathjoin('foo', 'bar');
var_export($path);
--EXPECT--
'foo/bar'
