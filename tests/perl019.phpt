--TEST--
Test 19: die() in user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  die("Bye bye!");
}
PERL_END
);

$perl->f();
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught PerlException: [perl] call error: Bye bye! at (eval %d) line %d.
 in %sperl019.php:%d
Stack trace:
#0 %sperl019.php(10): Perl->%sf%s
#1 {main}
  thrown in %sperl019.php on line %d
