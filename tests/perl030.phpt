--TEST--
Test 30: setting hash object's property
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
    \$self->{'a1'} = {"one"=>1,"two"=>2};
    bless \$self, \$type;
    return \$self;
  }
  sub dump() {
    my \$this = shift;
    use Data::Dumper;
    \$Data::Dumper::Sortkeys = 1;
    print Dumper(\$this);
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->a1 = array("one"=>2,"two"=>1);
$foo->a2 = array("one"=>1,"two"=>2);
# var_dump does not sort hashes
#var_dump($foo);
$foo->dump();
echo "ok\n";
?>
--EXPECT--
$VAR1 = bless( {
                 'a1' => {
                           'one' => 2,
                           'two' => 1
                         },
                 'a2' => {
                           'one' => 1,
                           'two' => 2
                         }
               }, 'Foo' );
ok
