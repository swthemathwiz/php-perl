--TEST--
Test 92: accessing undef Perl objects as arrays and hashes
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Undef;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    return undef;
  }
PERL_END
);
$u = new Perl("Undef");
var_dump($u[0]);
var_dump($u->prop);
echo "ok\n";
?>
--EXPECTF--
Warning: [perl] Not an ARRAY reference in %sperl092.php on line %d
NULL

Warning: [perl] Not a HASH reference in %sperl092.php on line %d
NULL
ok