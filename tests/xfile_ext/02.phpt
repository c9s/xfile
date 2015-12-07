--TEST--
Test extension replacement
--FILE--
<?php
$ext = xfile_ext_replace("foo.php", "test");
var_export($ext);
--EXPECT--
'foo.test'
