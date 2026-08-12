--TEST--
Test 84: rejecting unserialization of Perl objects
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
    my \$self = { a => 1 };
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
try {
  echo serialize(new Perl("Foo")), "\n";
} catch (Exception $e) {
  echo "serialize: " . get_class($e) . ": " . $e->getMessage() . "\n";
}
try {
  unserialize('O:4:"Perl":1:{s:1:"a";i:1;}');
  echo "unserialize ok\n";
} catch (Throwable $e) {
  echo "unserialize: " . get_class($e) . ": " . $e->getMessage() . "\n";
}
echo "ok\n";
?>
--EXPECT--
serialize: Exception: Serialization of 'Perl' is not allowed
unserialize: Exception: Unserialization of 'Perl' is not allowed
ok