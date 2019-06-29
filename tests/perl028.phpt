--TEST--
Test 28: setting scalar object's properties
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
    \$self->{'int'} = 12;
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
$foo->int   = 5;
$foo->float = 2.5;
$foo->str   = "str";
# var_dump does not sort hashes
#var_dump($foo);
$foo->dump();
echo "ok\n";
?>
--EXPECT--
$VAR1 = bless( {
                 'float' => '2.5',
                 'int' => 5,
                 'str' => 'str'
               }, 'Foo' );
ok
