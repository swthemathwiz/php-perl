--TEST--
Test 6: returning hash value from eval()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$a = $perl->eval('{"int"=>1,"double"=>3.5,"string"=>"string"};');
ksort($a);
var_dump($a);
echo "ok\n";
?>
--EXPECT--
array(3) {
  ["double"]=>
  float(3.5)
  ["int"]=>
  int(1)
  ["string"]=>
  string(6) "string"
}
ok
