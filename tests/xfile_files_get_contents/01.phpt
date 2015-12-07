--TEST--
Test xfile_files_get_contents
--FILE--
<?php
$contents = xfile_files_get_contents(['tests/xfile_ext/01.phpt', 'tests/xfile_ext/02.phpt']);
--EXPECT--
