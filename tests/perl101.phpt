--TEST--
Test 101: mirroring write-back via proxy reference
--SKIPIF--
<?php require_once('skipif.inc'); if (version_compare(phpversion(), '8.0.0', '<')) echo('PHP 8.x only'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<'PERL_END'
package Foo;
  sub new {
    my $this = shift;
    my $type = ref($this) || $this;
    my $self = {};
    $self->{'a1'} = 10;
    $self->{'a2'} = 20;
    bless $self, $type;
    return $self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');

// Write-back via reference
$x = & $foo->a1;
$x = 42;
echo "a1: " . $foo->a1 . "\n";
echo "a2: " . $foo->a2 . "\n";

// isset works after mirror sync
echo "isset a1: " . (isset($foo->a1) ? "true" : "false") . "\n";

// Second property write-back
$y = & $foo->a2;
$y = 99;
echo "a2 after write: " . $foo->a2 . "\n";

// unset clears mirror
unset($foo->a1);
echo "isset a1 after unset: " . (isset($foo->a1) ? "true" : "false") . "\n";
echo "ok\n";
?>
--EXPECT--
a1: 42
a2: 20
isset a1: true
a2 after write: 99
isset a1 after unset: false
ok
