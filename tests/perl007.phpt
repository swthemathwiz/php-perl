--TEST--
Test 7: Evaluating invalid Perlcode (eval)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
log_errors_max_len=4096
--FILE--
<?php
$perl = new Perl();
var_dump($perl->eval('$a = $s{$d}.'));
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught PerlException: [perl] eval error: syntax error at (eval %d) line %d, at EOF%A
 in %sperl007.php:3
Stack trace:
#0 %sperl007.php(3): Perl->eval('$a = $s{$d}.')
#1 {main}
  thrown in %sperl007.php on line %d
