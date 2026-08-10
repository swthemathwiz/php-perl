--TEST--
Test 75: running module info function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
phpinfo(INFO_MODULES);
echo "ok\n";
?>
--EXPECTREGEX--
.*
perl

Perl support => enabled
.*
Perl version => \d+\.\d+\.\d+
Perl compile version => \d+\.\d+\.\d+
.*
ok
