--TEST--
test pretty size
--FILE--
<?php
echo xfile_prettysize(1024), "\n";
echo xfile_prettysize(2248), "\n";
echo xfile_prettysize(1024 * 1024 * 4), "\n";
--EXPECT--
1 KB
2 KB
4.0 MB
