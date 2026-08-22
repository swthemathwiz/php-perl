--TEST--
Test 104: testing reference interactions with properties and dimensions
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '<')) echo( 'skip PHP 8.x only' ); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('package Foo; sub new { my $this = shift; my $type = ref($this) || $this; my $self = { s => 2 }; bless $self, $type; return $self; } package main;');
$perl->eval('@arr = (7);');
$f = new Perl('Foo');

function incr(&$v) {
  $v++;
}

/* By-ref function argument on a property */
incr($f->s);
var_dump($f->s);

/* Bound reference stays connected across a by-ref argument call */
$y = &$f->s;
incr($f->s);
var_dump($y);

/* Direct assignment writes through the bound reference */
$f->s = 9;
var_dump($y);

/* unset detaches the property but the alias keeps its last value */
unset($f->s);
var_dump($y);
var_dump(isset($f->s));

/* By-ref function argument on an array element */
var_dump($perl->array->arr[0]);
incr($perl->array->arr[0]);
var_dump($perl->array->arr[0]);
var_dump($perl->array->arr[0]);
?>
--EXPECT--
int(3)
int(4)
int(9)
int(9)
bool(false)
int(7)
int(8)
int(8)
