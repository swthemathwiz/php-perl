--TEST--
Test 86: calling Perl methods with call_user_func_array()
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
  sub add {
    my \$self = shift;
    my (\$x, \$y) = @_;
    return \$x + \$y + \$self->{a};
  }
PERL_END
);
$foo = new Perl("Foo");
echo call_user_func_array(array($foo, "add"), array(10, 20)), "\n";
try {
  call_user_func_array(array($foo, "missing"), array());
  echo "no exception\n";
} catch (PerlException $e) {
  echo "exception: " . trim($e->getMessage()) . "\n";
}
echo "ok\n";
?>
--EXPECTF--
31
exception: [perl] call error: Can't locate object method "missing" via package "Foo".
ok