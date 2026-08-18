--TEST--
Test 100: writing nested Perl array and hash elements
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
@arr = (1, 2, 3);
%h = (x => 1, y => 2);
@arr2 = ([5, 6], [7, 8]);
PERL_END
);
var_dump($perl->array->arr);
$perl->array->arr[0] = 42;
var_dump($perl->array->arr);
$perl->array->arr[1] += 5;
var_dump($perl->array->arr);
$perl->hash->h["x"] = 42;
var_dump($perl->hash->h);
$perl->array->arr2[0][0] = 9;
var_dump($perl->array->arr2);
$perl->array->arr = array(7, 8, 9);
var_dump($perl->array->arr);
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(42)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(42)
  [1]=>
  int(7)
  [2]=>
  int(3)
}
array(2) {
  ["x"]=>
  int(42)
  ["y"]=>
  int(2)
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(9)
    [1]=>
    int(6)
  }
  [1]=>
  array(2) {
    [0]=>
    int(7)
    [1]=>
    int(8)
  }
}
array(3) {
  [0]=>
  int(7)
  [1]=>
  int(8)
  [2]=>
  int(9)
}