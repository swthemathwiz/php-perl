--TEST--
Test 97: firing unary bitwise-not via a bound reference
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '<')) echo( 'skip unary bitwise-not on objects unsupported' ); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{bnot} = 10;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$o = new Perl('Foo');
$n = &$o->bnot;
$r = ~$n;
var_dump($r);
/* Value-context is read-only: the Perl variable must be unchanged */
var_dump($o->bnot);
?>
--EXPECT--
int(-11)
int(10)