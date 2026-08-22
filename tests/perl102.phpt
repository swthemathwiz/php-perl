--TEST--
Test 102: reading Perl cyclic structures (PHP 7 version of 52)
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '>=')) echo( 'skip PHP 7.x only' ); ?>
--FILE--
<?php
$perl = new Perl();
$x = $perl->eval(<<<PERL_END
  \$x = [1];
  \$x->[1] = \\\$x;
  return \$x;
PERL_END
);
var_dump($x);
$x[0] = 2;
var_dump($x);
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    *RECURSION*
  }
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    *RECURSION*
  }
}
ok
