--TEST--
Test 90: accessing scalar Perl objects as arrays
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Scalar;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$x = 42;
    my \$ref = \\\$x;
    bless \$ref, \$type;
    return \$ref;
  }
PERL_END
);
$s = new Perl("Scalar");
var_dump($s[0]);
$s[0] = 1;
var_dump(isset($s[0]));
var_dump(empty($s[0]));
unset($s[0]);
echo "ok\n";
?>
--EXPECTF--
Warning: [perl] Not an ARRAY reference in %sperl090.php on line %d
NULL

Warning: [perl] Not an ARRAY reference in %sperl090.php on line %d

Warning: [perl] Not an ARRAY reference in %sperl090.php on line %d
bool(false)

Warning: [perl] Not an ARRAY reference in %sperl090.php on line %d
bool(true)

Warning: [perl] Not an ARRAY reference in %sperl090.php on line %d
ok