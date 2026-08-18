--TEST--
Test 98: firing binary value-context operators via a bound reference
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '<')) echo( 'skip value-context binary operators on objects unsupported' ); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{add} = 10;
    \$self->{sub} = 10;
    \$self->{mul} = 10;
    \$self->{div} = 10;
    \$self->{mod} = 10;
    \$self->{pow} = 10;
    \$self->{cat} = '10';
    \$self->{sl} = 8;
    \$self->{sr} = 8;
    \$self->{and} = 10;
    \$self->{or} = 10;
    \$self->{xor} = 10;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$o = new Perl('Foo');

/* Proxy on the left of the operator */
$a = &$o->add;  $z = $a + 5;   var_dump($z);
$s = &$o->sub;  $z = $s - 4;   var_dump($z);
$m = &$o->mul;  $z = $m * 3;   var_dump($z);
$v = &$o->div;  $z = $v / 2;   var_dump($z);
$m2 = &$o->mod; $z = $m2 % 4;  var_dump($z);
$l = &$o->sl;   $z = $l << 2;  var_dump($z);
$r = &$o->sr;   $z = $r >> 1;  var_dump($z);
$c = &$o->cat;  $z = $c . "x"; var_dump($z);
$o2 = &$o->or;  $z = $o2 | 1;  var_dump($z);
$b = &$o->and;  $z = $b & 7;   var_dump($z);
$x2 = &$o->xor; $z = $x2 ^ 3;  var_dump($z);
$p = &$o->pow;  $z = $p ** 2;  var_dump($z);

/* Proxy on the right of the operator */
$z = 5 + $a;    var_dump($z);
$z = 14 - $s;   var_dump($z);
$z = 2 * $m;    var_dump($z);
$z = 20 / $v;   var_dump($z);
$z = 22 % $m2;  var_dump($z);
$z = 1 << $l;   var_dump($z);
$z = 1024 >> $r; var_dump($z);
$z = "x" . $c;  var_dump($z);
$z = 4 | $o2;   var_dump($z);
$z = 14 & $b;   var_dump($z);
$z = 9 ^ $x2;   var_dump($z);
$z = 2 ** $p;   var_dump($z);

/* Compound assignment, plain left hand side, proxy on the right */
$z = 5;    $z += $a;  var_dump($z);
$z = 14;   $z -= $s;  var_dump($z);
$z = 2;    $z *= $m;  var_dump($z);
$z = 20;   $z /= $v;  var_dump($z);
$z = 22;   $z %= $m2; var_dump($z);
$z = 1;    $z <<= $l; var_dump($z);
$z = 1024; $z >>= $r; var_dump($z);
$z = "x";  $z .= $c;  var_dump($z);
$z = 4;    $z |= $o2; var_dump($z);
$z = 14;   $z &= $b;  var_dump($z);
$z = 9;    $z ^= $x2; var_dump($z);
$z = 2;    $z **= $p; var_dump($z);

/* Unary minus */
$z = -$a; var_dump($z);

/* Value-context is read-only: the Perl variables must be unchanged */
var_dump($o->add, $o->sub, $o->mul, $o->div, $o->mod, $o->pow);
var_dump($o->cat, $o->sl, $o->sr, $o->and, $o->or, $o->xor);
?>
--EXPECT--
int(15)
int(6)
int(30)
int(5)
int(2)
int(32)
int(4)
string(3) "10x"
int(11)
int(2)
int(9)
int(100)
int(15)
int(4)
int(20)
int(2)
int(2)
int(256)
int(4)
string(3) "x10"
int(14)
int(10)
int(3)
int(1024)
int(15)
int(4)
int(20)
int(2)
int(2)
int(256)
int(4)
string(3) "x10"
int(14)
int(10)
int(3)
int(1024)
int(-10)
int(10)
int(10)
int(10)
int(10)
int(10)
int(10)
string(2) "10"
int(8)
int(8)
int(10)
int(10)
int(10)
