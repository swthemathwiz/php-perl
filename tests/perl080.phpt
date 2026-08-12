--TEST--
Test 80: calling die() in Perl constructor
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
    die("bad constructor");
    my \$self = { a => 42 };
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
# Try with die
try {
  $foo = new Perl("Foo");
  echo "no exception\n";
} catch (PerlException $e) {
  echo "exception: " . trim($e->getMessage()) . "\n";
}

# Repeat 20 times
$times = 0;
for( $i = 0; $i < 20; $i++ ) {
  try {
    $foo = new Perl("Foo");
    echo "no exception\n";
  } catch (PerlException $e) {
    $times++;
  }
}
var_dump($times);
echo "ok\n";
?>
--EXPECTF--
exception: [perl] constructor error: bad constructor at (eval %d) line %d.
int(20)
ok
