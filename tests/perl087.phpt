--TEST--
Test 87: accessing out-of-range and negative indices on array-based Perl objects
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Bar;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = [ 10, 20 ];
    bless \$self, \$type;
    return \$self;
  }
PERL_END
);
$bar = new Perl("Bar");
var_dump($bar[0]);
var_dump($bar[5]);
var_dump($bar[-1]);
var_dump(isset($bar[0]));
var_dump(isset($bar[5]));
var_dump(isset($bar[-1]));
var_dump(empty($bar[5]));
echo "ok\n";
?>
--EXPECT--
int(10)
NULL
int(20)
bool(true)
bool(false)
bool(true)
bool(true)
ok