--TEST--
Test 72: Unknown class conversion attempt
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  my \$x = shift(@_);
}
PERL_END
);
class Dummy extends stdClass{
};
$a = new Dummy;
$perl->f($a);
# Should not reach here
echo "error\n";
?>
--EXPECTF--
Fatal error: [perl] Can't convert class 'Dummy' object to Perl in %sperl072.php on line 12
