--TEST--
Test 54: Perl's cyclic object structures support
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$x = $perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
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
  \$x = new Foo;
  \$x->{'foo'} = \$x;
  return \$x;
PERL_END
);
# var_dump does not sort hashes
#var_dump($x);
$x->dump();
$x->x = 1;
# var_dump does not sort hashes
#var_dump($x);
$x->dump();
echo "ok\n";
?>
--EXPECT--
$VAR1 = bless( {
                 'foo' => $VAR1
               }, 'Foo' );
$VAR1 = bless( {
                 'foo' => $VAR1,
                 'x' => 1
               }, 'Foo' );
ok
