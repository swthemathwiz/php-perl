--TEST--
Test 75: Run module info function
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
Perl runtime version => \d+\.\d+\.\d+
.*
ok
