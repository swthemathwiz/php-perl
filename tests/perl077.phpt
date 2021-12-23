--TEST--
Test 77: Dumping a variety of objects
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
\$gs = 1;
\$ga = [ 1 ];
\$gh = { a => 1 };
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{s} = 2;
    \$self->{a} = [ 2 ];
    \$self->{h} = { a => 2 };
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
// Globals
var_dump($perl->gs);
var_dump($perl->ga);
var_dump($perl->gh);
// Specials
var_dump($perl->array);
var_dump($perl->hash);
var_dump($perl->scalar);
// Package members
$f = new Perl('Foo');
var_dump($f->s);
var_dump($f->a);
var_dump($f->h);
// Reference to member
$rs = &$f->s;
var_dump( $rs );
$ra = &$f->a;
var_dump( $ra );
$rh = &$f->h;
var_dump( $rh );
// Reference to component of member
$ram = &$f->a[0];
var_dump( $ram );
$rah = &$f->h->a;
var_dump( $rah );
echo "ok\n";
?>
--EXPECT--
int(1)
array(1) {
  [0]=>
  int(1)
}
array(1) {
  ["a"]=>
  int(1)
}
object(Perl::<array operator>)#2 (0) {
}
object(Perl::<hash operator>)#2 (0) {
}
object(Perl::<scalar operator>)#2 (0) {
}
int(2)
array(1) {
  [0]=>
  int(2)
}
array(1) {
  ["a"]=>
  int(2)
}
object(Perl::scalar)#3 (0) {
}
object(Perl::scalar)#4 (1) {
  [0]=>
  int(2)
}
object(Perl::scalar)#5 (1) {
  ["a"]=>
  int(2)
}
object(Perl::scalar)#7 (0) {
}
object(Perl::scalar)#8 (0) {
}
ok
