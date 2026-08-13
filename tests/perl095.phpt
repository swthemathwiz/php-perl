--TEST--
Test 95: decoding JSON of Perl objects with json_decode()
--SKIPIF--
<?php require_once('skipif.inc'); if (!function_exists('json_decode')) echo('skip json_decode not available'); ?>
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
$j = json_decode(json_encode($foo), true);
ksort($j);
var_dump($j);
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
ok