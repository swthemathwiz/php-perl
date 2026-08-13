--TEST--
Test 94: garbage collection of cyclic perl proxies
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '<')) echo('skip PHP < 8.0 garbage collection of cyclic perl proxies unsupported'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('package Node; sub new { my $self = bless {}, "Node"; $self->{next} = $self; return $self; }');
$n   = new Perl("Node");
var_dump($n->next === $n);
get_object_vars($n);
$wr  = WeakReference::create($n);
unset($n);
gc_collect_cycles();
var_dump($wr->get());
?>
--EXPECT--
bool(true)
NULL
