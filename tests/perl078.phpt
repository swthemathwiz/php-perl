--TEST--
Test 78: Modifying objects with assign/inc/dec operators
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
    \$self->{x} = 0x2049;
    \$self->{y} = 0x3859;
    \$self->{z} = 0x0911;
    \$self->{s} = 'string';
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$x->x |= 0x4040;
var_dump($x->x);
$x->y ^= 0x3040;
var_dump($x->y);
$x->z &= 0x0099;
var_dump($x->z);
// N.B.: strings increment but not decrement
$x->s++;
var_dump($x->s);
$x->s--;
var_dump($x->s);
$x->s++;
var_dump($x->s);
echo "ok\n";
?>
--EXPECTF--
int(24649)
int(2073)
int(17)
string(6) "strinh"%w%r(Deprecated: Decrement on non-numeric string has no effect.*)?%r
string(6) "strinh"
string(6) "strini"
ok

