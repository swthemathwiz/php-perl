--TEST--
Test 85: getting object variables and casting to array
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = { a => 1, b => [ 2, 3 ] };
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
$foo = new Perl("Foo");
$vars = get_object_vars($foo);
$cast = (array)$foo;
ksort($vars);
ksort($cast);
var_dump($vars);
var_dump($cast);
echo "ok\n";
?>
--EXPECT--
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
}
ok