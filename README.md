PHP Perl Extension (Updated for PHP7 and PHP8)
==============================================

What is the PHP Perl Extension?
===============================

This extension allows embedding of Perl Interpreter into PHP7 to:

  * execute Perl files
  * evaluate Perl code
  * access values of Perl variables
  * call Perl subroutines
  * instantiate and manipulate of Perl objects

About the Updates
=================
I have modified the version 1.0.1 php-perl extension source, which
ran with PHP 5, to run with PHP 7+.  The source has been modified
extensively. I started numbering the new versions from 1.20.0. The
updates have never been tested on any OS other than Linux, so
any other OS is a YMMV.

The syntax and semantics have not changed nor have the limitations.

The primary changes were:

  - Adapt to the newer PHP object model.
  - Modify the use of binary hashes (no longer supported).
  - Delete the older tasking model code.
  - General PHP 5 to 7 changes.
  - Miscellaneous updating of the code (4+ years of changes).
  - Modify various tests primarily due to var_dump
    not sorting hashes consistently. Added a few tests.
    The tests sometime use Perl's Data::Dumper to get
    around the lack of var_dump hash sorting.
  - Converted about 4 tests to XFAIL (known limitations).
  - N.B.: The code is no longer compatible with PHP 5

The original source was released under PHP v3.0 license and my
modifications are released under the same.

Requirements
============
  - PHP 7.3/7.4 or later. PHP 8.x with additional caveats
  - Perl 5.8.0 or later with module ExtUtils::Embed

Quick Install
=============

Step 1. Compile this extension. PHP_PREFIX and PERL_PREFIX must point to real
        PHP and Perl installation prefixes:
```sh
        export PHP_PREFIX="/usr"
        export PERL_PREFIX="/usr"
        $PHP_PREFIX/bin/phpize
        ./configure --with-perl=$PERL_PREFIX --with-php-config=$PHP_PREFIX/bin/php-config
        make
```
Step 2. Install the extension (this step can require root privileges):
```sh
        make install
```
Step 3. Add Perl extension into your php.ini (this step can require root privileges)
```
        extension=perl.so
```

Windows Installation Notes
==========================

Step 1. Download ActivePerl binaries for Windows from [ActiveState](http://www.activestate.com/products/perl/)
        and install them.

Step 2. Put this extension into corresponding PHP source tree (into ext/perl)

Step 3. Compile the extension
```
        SET PERL_HOME=C:\perl
        msdev perl.dsp /MAKE "perl - Win32 Release_TS"
```
Step 4. Copy php_perl.dll (from Release_TS) to PHP extension dir

Step 5. Add Perl extension into your php.ini
```
        extension=php_perl.dll
```

PHP API
=======

new Perl()
----------
Creates Perl interpreter. It allows:

  * reading and modifying of Perl variables
  * calling Perl functions
  * evaluating Perl code
  * loading and executing external Perl files

Examples:
```php
      $perl = new Perl();
      var_dump($perl->x);        // print scalar Perl variable - $x
      var_dump($perl->array->x); // print array Perl variable - @x
      var_dump($perl->hash->x);  // print hash Perl variable - %x
      $perl->func();             // call Perl function 'func' in void context
      $x = $perl->func();        // call Perl function 'func' in scalar context
      $y = $perl->array->func(); // call Perl function 'func' in array context
      $y = $perl->hash->func();  // call Perl function 'func' in hash context

      $perl->eval('use Digest::MD5');
      echo $perl->{'Digest::MD5::md5_hex'}('Hello');
```

Perl->eval($perl_code)
----------------------
Evaluates Perl code and returns result. If Perl code is invalid it will
throw PHP exception.

Examples:
```php
      $perl = new Perl();
      $perl->eval('require "test.pl";');
      echo $perl->eval($x.'+'.$y.';');
      $perl->eval('$z='.$x.'+'.$y.';');
```

By default, Perl code is evaluated in scalar context, but it can be
evaluated in array or hash context too.

Examples:
```php
      $perl = new Perl();
      $perl->eval('("a","b","c")');                  // eval in void context
      var_dump($perl->eval('("a","b","c")'));        // eval in scalar context
      var_dump($perl->array->eval('("a","b","c")')); // eval in array context
      var_dump($perl->hash->eval('("a","b","c")'));  // eval in hash context
```

Perl->require($perl_file_name)
------------------------------
Loads and executes Perl file. It doesn't return any value. If required Perl
file doesn't exist or invalid it will throw PHP exception.

Examples:
```php
      $perl = new Perl();
      $perl->require('test.pl');
```

new Perl($perl_class_name[, $constructor = "new"[, ...]])
-----------------------------------------------------
Creates an instance of Perl class through calling specified constructor
or "new" if constructor is not specified. Additional parameters passed
to Perl's constructor. The created object allows:

  * reading and modifying of object properties
  * calling methods
  * cloning

Examples:
```php
      $x = new Perl("Test");
      $y = new Perl("Test","copy",$x);
      $z = clone $y;
      echo $z->property;
      echo $z->method(1,2,3);
```

Methods can be called in array or hash context in the same way as Perl
functions, but all properties are accessible directly (without array or
hash modifiers).

Examples:
```php
      $x = new Perl("Test");
      $x->f();                  // call method "f" in void context
      var_dump($x->f());        // call method "f" in scalar context
      var_dump($x->array->f()); // call method "f" in array context
      var_dump($x->hash->f());  // call method "f" in hash context
```

Known BUGS and Limitations
==========================

* Perl objects passed between Perl and PHP by reference all other data type
   (including arrays and hashes) passed by value. So modification of Perl's
   arrays and hashes in PHP will not have effect in Perl.
```php
      $x = $perl->array->x;
      $x[0] = 1; // Perl's array @x still unmodified

      // But you can use PHP references to do this. The following code works fine.

      $y = &$perl->array->y;
      $y[0] = 1; // Modifies Perl's array @y
```
* pecl/perl can't call internal Perl functions (print, die, ...).

* In PHP 8.x, references to Perl objects are not properly manipulated:
```php
      $perl->y = 1;
      $x = &$perl->y;
      $x = 2;
      var_dump( $perl->y ); // Should be int(2), but is int(1)
```

Testing
=======

The status of the most recent testing follows:

  | OS               | PHP Version | Perl Version | Status                                          |
  | ---------------- | ----------- | ------------ | ----------------------------------------------- |
  | Ubuntu 18.04 LTS | 7.3.33      | 5.26.1       | All Passed                                      |
  | Ubuntu 20.04 LTS | 7.4.26      | 5.30.0       | All Passed                                      |
  | Ubuntu 20.04 LTS | 8.0.13      | 5.30.0       | PHP 8.x References to Perl variables not usable |
  | Ubuntu 20.04 LTS | 8.1.0       | 5.30.0       | PHP 8.x References to Perl variables not usable |
  | Ubuntu 22.04 LTS | 8.1.2       | 5.34.0       | PHP 8.x References to Perl variables not usable |
  | Fedora 30        | 7.3.18      | 5.28.2       | All Passed                                      |
  | Fedora 34        | 7.4.27      | 5.32.1       | All Passed                                      |
  | Fedora 35        | 8.0.13      | 5.34.0       | PHP 8.x References to Perl variables not usable |
  | Fedora 38        | 8.2.8       | 5.36.1       | PHP 8.x References to Perl variables not usable |

The original extension was tested on RedHat Linux 9.0 with PHP 5.0.0RC2-dev (non ZTS build)
and perl-5.8.0 (installed from RPM) and on Windows 2000 with PHP-5.0.0RC2-dev
(ZTS build) and perl-5.8.0.
