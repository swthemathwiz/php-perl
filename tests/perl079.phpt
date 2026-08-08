--TEST--
Test 79: Accessing packages with nested packages
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('
package Foo::Bar::Baz;
sub new {
    my $class = shift;
    return bless { name => "nested" }, $class;
}
package Foo::Bar;
sub new {
    my $class = shift;
    return bless { name => "mid" }, $class;
}
1;
');

$obj1 = $perl->eval('Foo::Bar::Baz->new()');
var_dump($obj1);

$obj2 = $perl->eval('Foo::Bar->new()');
var_dump($obj2);
?>
--EXPECTF--
object(Perl::Foo::Bar::Baz)#%d (1) {
  ["name"]=>
  string(6) "nested"
}
object(Perl::Foo::Bar)#%d (1) {
  ["name"]=>
  string(3) "mid"
}
