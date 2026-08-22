--TEST--
Test 105: passing PHP cyclic structures (legacy PHP 7 version of Test 53)
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '>=')) echo( 'skip PHP 7.x only' ); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  \$x = shift(@_);
  \$x->[0] = 3;
  return \$x;
}
PERL_END
);

$x = array(1);
$x[1] =& $x;
$x[0] = 2;
ksort($x);
var_dump($x);
$y = $perl->f($x);
ksort($y);
var_dump($y);
$x[0] = 4;
ksort($x);
var_dump($x);
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  &array(2) {
    [0]=>
    int(2)
    [1]=>
    *RECURSION*
  }
}
array(2) {
  [0]=>
  int(3)
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    *RECURSION*
  }
}
array(2) {
  [0]=>
  int(4)
  [1]=>
  &array(2) {
    [0]=>
    int(4)
    [1]=>
    *RECURSION*
  }
}
ok
