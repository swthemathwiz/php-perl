--TEST--
Test 47: foreach() on Perl object
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
    my \$self = {};
    \$self->{x} = 1;
    \$self->{y} = 2;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$i = 0;
foreach($x as $var => $val) {
  $name[$var] = $val;
#  echo "$var = ";
#  var_dump($val);
  if (++$i > 5) exit( 'went past end' );
}
ksort($name);
foreach($name as $var => $val) {
  echo "$var = ";
  var_dump($val);
}
echo "ok\n";
?>
--EXPECT--
x = int(1)
y = int(2)
ok
