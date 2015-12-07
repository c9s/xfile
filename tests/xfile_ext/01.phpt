--TEST--
Test extension
--FILE--
<?php
$ext = xfile_ext("foo.php");
var_export($ext);
--EXPECT--
'php'
