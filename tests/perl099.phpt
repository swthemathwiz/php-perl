--TEST--
Test 99: using a non-scalar Perl value as an operand fails
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '<')) echo( 'skip non-scalar operand validation unsupported' ); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{arr} = [1, 2, 3];
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$o = new Perl('Foo');
$a = &$o->arr;
$a += 5;
# Should not reach here
echo "error\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: [perl] Cannot use += on non-scalar/non-string in %sperl099.php:%d
Stack trace:
#0 {main}
  thrown in %sperl099.php on line %d
