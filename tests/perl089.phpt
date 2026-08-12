--TEST--
Test 89: checking that PerlException extends Exception
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo is_subclass_of("PerlException", "Exception") ? "subclass\n" : "not a subclass\n";
$e = new PerlException("boom");
echo get_class($e), "\n";
echo $e->getMessage(), "\n";
try {
  throw $e;
} catch (PerlException $caught) {
  echo "caught: " . ($caught === $e ? "same object" : "different") . "\n";
}
echo "ok\n";
?>
--EXPECT--
subclass
PerlException
boom
caught: same object
ok