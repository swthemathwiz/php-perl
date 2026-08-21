--TEST--
Test 52: reading Perl cyclic structures
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
--XFAIL--
var_dump *RECURSION* output differs across PHP versions (see matrix run)
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  *RECURSION*
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  *RECURSION*
}
ok
