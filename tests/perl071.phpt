--TEST--
Test 71: Basic type check
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  my \$x = shift(@_);
  use Data::Dumper;
  print Dumper(\$x);
}
PERL_END
);
$perl->f(1);
$perl->f('a');
$perl->f(true);
$perl->f(false);
$perl->f(null);
$a=1;
$b=& $a;
$c=null;
$perl->f($a);
$perl->f($b);
$perl->f($c);
echo "ok\n";
?>
--EXPECTF--
$VAR1 = 1;
$VAR1 = 'a';
$VAR1 = 1;
$VAR1 = '';
$VAR1 = undef;
$VAR1 = 1;
$VAR1 = 1;
$VAR1 = undef;
ok
