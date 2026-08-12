--TEST--
Test 83: serializing Perl objects is not allowed
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
    my \$self = { b => [ 2, 3 ] };
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
$foo = new Perl("Foo");
try {
  echo serialize($foo), "\n";
} catch (Exception $e) {
  echo get_class($e) . ": " . $e->getMessage() . "\n";
}
echo "ok\n";
?>
--EXPECT--
Exception: Serialization of 'Perl' is not allowed
ok