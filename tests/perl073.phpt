--TEST--
Test 73: Cloning special object
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
var_dump($perl);
$perl2 = clone $perl;
var_dump($perl2);
$perl3 = $perl->array;
var_dump($perl3);
$perl4 = clone $perl3;
var_dump($perl4);
echo "ok\n";
?>
--EXPECTF--
object(Perl)#1 (0) {
}
object(Perl)#2 (0) {
}
object(Perl::<array operator>)#3 (0) {
}
object(Perl::<array operator>)#4 (0) {
}
ok
