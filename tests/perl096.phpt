--TEST--
Test 96: firing every in-place do_operation opcode via a bound reference
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
    \$self->{inc} = 1;
    \$self->{dec} = 1;
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
    \$self->{yes} = (1 == 1);
    \$self->{no} = (1 == 0);
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$o = new Perl('Foo');
$i = &$o->inc; $i++;  var_dump($o->inc);
$d = &$o->dec; $d--;  var_dump($o->dec);
$a = &$o->add; $a += 5; var_dump($o->add);
$s = &$o->sub; $s -= 4; var_dump($o->sub);
$m = &$o->mul; $m *= 3; var_dump($o->mul);
$v = &$o->div; $v /= 2; var_dump($o->div);
$m2 = &$o->mod; $m2 %= 4; var_dump($o->mod);
$l = &$o->sl; $l <<= 2; var_dump($o->sl);
$r = &$o->sr; $r >>= 1; var_dump($o->sr);
$c = &$o->cat; $c .= "x"; var_dump($o->cat);
$o2 = &$o->or; $o2 |= 1; var_dump($o->or);
$b = &$o->and; $b &= 7; var_dump($o->and);
$x2 = &$o->xor; $x2 ^= 3; var_dump($o->xor);
$p = &$o->pow; $p **= 2; var_dump($o->pow);
$u = &$o->undef; $u += 5; var_dump($o->undef);
$y = &$o->yes; $y += 2; var_dump($o->yes);
$n = &$o->no; $n .= "!"; var_dump($o->no);
echo "ok\n";
?>
--EXPECT--
int(2)
int(0)
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
int(5)
int(3)
string(2) "0!"
ok
