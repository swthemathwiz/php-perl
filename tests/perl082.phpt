--TEST--
Test 82: calling json_encode() on Perl objects
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
echo json_encode($foo), "\n";
echo "ok\n";
?>
--EXPECT--
{"a":1,"b":[2,3]}
ok