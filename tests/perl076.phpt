--TEST--
Test 76: Repeated require
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
for( $i = 0; $i < 4; $i++ ) {
  # N.B. this file really only tests starting
  # and stoping the perl extension, which only
  # works with debug turned on
  if( function_exists('perl_stop') ) {
    $perl = new Perl();
    $perl->require(dirname(__FILE__)."/perl076.pl");
    $perl = null;
    perl_stop();
  }
  echo "ok\n";
}
?>
--EXPECT--
ok
ok
ok
ok
