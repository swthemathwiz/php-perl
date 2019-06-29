--TEST--
Test 65: Modifying hash Perl variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->hash->x = array("a"=>1,"b"=>2);
$perl->eval('my @temp = %x; @temp = sort(@temp); print "@temp\n"');
var_dump(isset($perl->hash->x));
var_dump(empty($perl->hash->x));
$a=$perl->hash->x;
ksort($a);
var_dump($a);
unset($perl->hash->x);
var_dump(isset($perl->hash->x));
var_dump(empty($perl->array->x));
echo "ok\n";
?>
--EXPECT--
1 2 a b
bool(true)
bool(false)
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
bool(false)
bool(true)
ok
