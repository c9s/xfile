--TEST--
Test xfile_rmtree
--FILE--
<?php
@mkdir('tmp/foo/bar', 0755, true);
@mkdir('tmp/foo/zoo', 0755, true);
@mkdir('tmp/bar/foo', 0755, true);
@mkdir('tmp/bar/bar', 0755, true);
@mkdir('tmp/bar/zoo', 0755, true);
$ret = xfile_rmtree('tmp');
var_export($ret);
--EXPECT--
true
