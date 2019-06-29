--TEST--
Test 74: Chaining scalar keywords. 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('$a = 1;');
var_dump($perl->a);
var_dump($perl->scalar->a);
var_dump($perl->scalar->scalar->a);
echo "ok\n";
?>
--EXPECTF--
int(1)
int(1)

Notice: [perl] Undefined variable: '$scalar' in %sperl074.php on line %d

Notice: Trying to get property 'a' of non-object in %sperl074.php on line %d
NULL
ok
