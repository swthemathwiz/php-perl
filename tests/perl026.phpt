--TEST--
Test 26: reading hash object's property
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
    my \$self = {};
    \$self->{'x'} = {"int"=>1, "float"=>2.5, "str"=>"str"};
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$a = $foo->x;
ksort($a);
var_dump($a);
echo "ok\n";
?>
--EXPECT--
array(3) {
  ["float"]=>
  float(2.5)
  ["int"]=>
  int(1)
  ["str"]=>
  string(3) "str"
}
ok
