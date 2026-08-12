--TEST--
Test 93: accessing unblessed Perl references as arrays and hashes
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package ScalarRef;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$ref = \\42;
    return \$ref;
  }
package ArrayRef;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$ref = [ 1, 2 ];
    return \$ref;
  }
PERL_END
);
$s = new Perl("ScalarRef");
var_dump($s[0]);
$a = new Perl("ArrayRef");
var_dump($a->prop);
echo "ok\n";
?>
--EXPECTF--
Warning: [perl] Not an ARRAY reference in %sperl093.php on line %d
NULL

Warning: [perl] Not a HASH reference in %sperl093.php on line %d
NULL
ok