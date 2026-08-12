--TEST--
Test 80: calling die() in Perl constructor
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  our \$calls = 0;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    \$calls++;
    if( \$calls == 1 ) {
      die("bad constructor");
    }
    my \$self = { a => 42 };
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
try {
  $foo = new Perl("Foo");
  echo "no exception\n";
} catch (PerlException $e) {
  echo "exception: " . trim($e->getMessage()) . "\n";
}
$foo = new Perl("Foo");
var_dump(get_class($foo));
var_dump($foo->a);
echo "ok\n";
?>
--EXPECTF--
exception: [perl] constructor error: bad constructor at (eval %d) line %d.
string(4) "Perl"
int(42)
ok
