--TEST--
Test 80: calling die() in Perl constructor
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    die("bad constructor");
  }
PERL_END
);
try {
  $foo = new Perl("Foo");
  echo "no exception\n";
} catch (PerlException $e) {
  echo "exception: " . trim($e->getMessage()) . "\n";
}
echo "ok\n";
?>
--EXPECTF--
exception: [perl] constructor error: bad constructor at (eval %d) line %d.
ok
