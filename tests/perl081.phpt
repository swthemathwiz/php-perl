--TEST--
Test 81: accessing string offset on array-based Perl object
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
    my \$self = [10, 20, 30];
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
$foo = new Perl("Foo");
var_dump($foo['foo']);        // string offset on array -> E_WARNING
$foo['bar'] = 99;             // string offset on array -> E_WARNING
var_dump(isset($foo['foo'])); // string offset on array -> E_WARNING
unset($foo['foo']);           // string offset on array -> E_WARNING
$i = 1;
$alias =& $i;
var_dump($foo[$alias]);       // PHP reference as index -> int(20)
var_dump($foo[0]);            // untouched by the failed write/unset -> int(10)
var_dump($foo[-1]);           // untouched by the failed write/unset -> int(30)
echo "ok\n";
?>
--EXPECTF--
Warning: [perl] Array index must be an integer in %sperl081.php on line %d
NULL

Warning: [perl] Array index must be an integer in %sperl081.php on line %d

Warning: [perl] Array index must be an integer in %sperl081.php on line %d
bool(false)

Warning: [perl] Array index must be an integer in %sperl081.php on line %d
int(20)
int(10)
int(30)
ok
