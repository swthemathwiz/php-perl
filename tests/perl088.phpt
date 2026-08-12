--TEST--
Test 88: passing a PHP closure to Perl fails
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub apply {
  my (\$cb, \$x) = @_;
  return \$cb->(\$x);
}
PERL_END
);
$perl->apply(function($x) { return $x * 2; }, 5);
# Should not reach here
echo "error\n";
?>
--EXPECTF--
Fatal error: [perl] Can't convert class 'Closure' object to Perl in %sperl088.php on line %d%r(\w*\nStack trace:.*)?%r