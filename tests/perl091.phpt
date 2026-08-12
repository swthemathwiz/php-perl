--TEST--
Test 91: accessing array Perl objects as hashes
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = [ 10, 20 ];
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
$a = new Perl("Foo");
var_dump($a->prop);
$a->prop = 1;
var_dump(isset($a->prop));
var_dump(empty($a->prop));
unset($a->prop);
echo "ok\n";
?>
--EXPECTF--
Warning: [perl] Not a HASH reference in %sperl091.php on line %d
NULL

Warning: [perl] Not a HASH reference in %sperl091.php on line %d

Warning: [perl] Not a HASH reference in %sperl091.php on line %d
bool(false)

Warning: [perl] Not a HASH reference in %sperl091.php on line %d
bool(true)

Warning: [perl] Not a HASH reference in %sperl091.php on line %d
ok