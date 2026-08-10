PHP Perl Extension (Updated for PHP 7 and PHP 8)
================================================

What is the PHP Perl Extension?
===============================

This extension allows embedding a Perl interpreter into PHP 7+, enabling you to:

  * Execute Perl files
  * Evaluate Perl code
  * Access values of Perl variables
  * Call Perl subroutines
  * Instantiate and manipulate Perl objects

About the Updates
=================
I have updated version 1.0.1 of the php-perl extension source (built for PHP 5)
to support PHP 7+.  The source has been modified extensively. I started numbering
the new versions from 1.20.0. The updates have never been tested on any OS other
than Linux, so your mileage may vary on other operating systems.

Except as noted, the syntax and semantics have not changed, and
the limitations of the PHP 5 version are still present.

The primary changes were:

  - Adapted to the newer PHP object model.
  - Modified the use of binary hashes (no longer supported).
  - Deleted the older tasking model code.
  - Made general changes for the PHP 5-to-7 transition.
  - Updated miscellaneous code (4+ years of API changes).
  - Modified various tests, primarily because `var_dump` does not sort hashes
    consistently. Some tests use Perl's `Data::Dumper` to get around
    the lack of consistent hash sorting in `var_dump`.
  - Added a few tests.
  - Converted about 4 tests to XFAIL (known limitations).
  - N.B.: The code is no longer compatible with PHP 5.

The original source was released under the PHP v3.0 license, and my
modifications are released under the same license.

Requirements
============
  - PHP 7.3/7.4 or later; PHP 8.x with additional caveats
  - Perl 5.8.0 or later with the `ExtUtils::Embed` module

Quick Install
=============

Step 1. Compile this extension. `PHP_PREFIX` and `PERL_PREFIX` must point to valid
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
Step 3. Add the Perl extension to your `php.ini` (this step can require root privileges):
```ini
    extension=perl.so
```

Windows Installation Notes
==========================
PHP only supports the MSVC toolchain and does not generally include development
support (headers and libraries). So, prerequisites are:

  - Obtain (or build) a Perl version built with MSVC (an officially supported
    configuration). Most versions of Perl on Windows are built using the MinGW
    toolchain.
  - Install the PHP Development Pack from [PHP](https://windows.php.net/downloads/).
  - Install [MSYS2](https://www.msys2.org/) to augment
    your build toolchain to include tools such as bison, sed, and re2c.

Step 1. Compile this extension. `PHP_PREFIX` and `PERL_PREFIX` must point to valid
        PHP and Perl installation prefixes:
```cmd
    SET PERL_PREFIX=C:\perl
    SET PHP_PREFIX=C:\php
    "%PHP_PREFIX%\phpize.bat"
    .\configure.bat --with-perl="%PERL_PREFIX%" --with-prefix="%PHP_PREFIX%"
    nmake
```
Step 2. Copy `php_perl.dll` (from `Release`) to the PHP extension directory.

Step 3. Add the Perl extension to your `php.ini`:
```ini
    extension=php_perl.dll
```

PHP API
=======

`new Perl()`
------------
Creates a Perl interpreter. It allows:

  * Reading and modifying Perl variables
  * Calling Perl functions
  * Evaluating Perl code
  * Loading and executing external Perl files

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

`$perl->eval($perl_code)`
-----------------------
Evaluates Perl code and returns the result. If the Perl code is invalid, the method
will throw a PHP exception.

Examples:
```php
    $perl = new Perl();
    $perl->eval('require "test.pl";');
    echo $perl->eval($x.'+'.$y.';');
    $perl->eval('$z='.$x.'+'.$y.';');
```

By default, Perl code is evaluated in scalar context, but it can be
evaluated in array or hash contexts too.

Examples:
```php
    $perl = new Perl();
    $perl->eval('("a","b","c")');                  // eval in void context
    var_dump($perl->eval('("a","b","c")'));        // eval in scalar context
    var_dump($perl->array->eval('("a","b","c")')); // eval in array context
    var_dump($perl->hash->eval('("a","b","c")'));  // eval in hash context
```

`$perl->require($perl_file_name)`
-------------------------------
Loads and executes a Perl file. It does not return any value. If the required Perl
file does not exist or is invalid, the method will throw a PHP exception.

Examples:
```php
    $perl = new Perl();
    $perl->require('test.pl');
```

`new Perl($perl_class_name[, $constructor = "new"[, ...]])`
-----------------------------------------------------------
Creates an instance of a Perl class by calling a specified constructor
(defaulting to "new" if omitted). Additional parameters are passed
to Perl's constructor. The created object allows:

  * Reading and modifying object properties
  * Calling methods
  * Cloning

Examples:
```php
    $x = new Perl("Test");
    $y = new Perl("Test","copy",$x);
    $z = clone $y;
    echo $z->property;
    echo $z->method(1,2,3);
```

Methods can be called in array or hash contexts in the same way as Perl
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

Known Bugs and Limitations
==========================

* Perl objects are passed between Perl and PHP by reference; all other data
  types (including arrays and hashes) are passed by value. Therefore, modifying Perl's
  arrays and hashes in PHP does not change the corresponding Perl variables.
```php
        $x = $perl->array->x;
        $x[0] = 1; // Perl's array @x still unmodified

        // However, you can use PHP references to achieve this:

        $y = &$perl->array->y;
        $y[0] = 1; // Modifies Perl's array @y
```
* The extension cannot call internal Perl functions (`print`, `die`, ...).

* In PHP 8.x, references to Perl variables are not properly handled:
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
  | Ubuntu 24.04 LTS | 8.3.6       | 5.38.2       | PHP 8.x References to Perl variables not usable |
  | Ubuntu 26.04 LTS | 8.5.4       | 5.40.1       | PHP 8.x References to Perl variables not usable |
  | Fedora 30        | 7.3.18      | 5.28.2       | All Passed                                      |
  | Fedora 34        | 7.4.27      | 5.32.1       | All Passed                                      |
  | Fedora 35        | 8.0.13      | 5.34.0       | PHP 8.x References to Perl variables not usable |
  | Fedora 38        | 8.2.8       | 5.36.1       | PHP 8.x References to Perl variables not usable |
  | Fedora 39        | 8.2.13      | 5.38.2       | PHP 8.x References to Perl variables not usable |
  | Fedora 40        | 8.3.12      | 5.38.2       | PHP 8.x References to Perl variables not usable |
  | Fedora 42        | 8.4.21      | 5.40.4       | PHP 8.x References to Perl variables not usable |
  | Fedora 43        | 8.4.24      | 5.42.3       | PHP 8.x References to Perl variables not usable |
  | Fedora 44        | 8.5.9       | 5.42.3       | PHP 8.x References to Perl variables not usable |

The original extension was tested on Red Hat Linux 9.0 with PHP 5.0.0RC2-dev (non-ZTS build)
and Perl 5.8.0 (installed from RPM), and on Windows 2000 with PHP 5.0.0RC2-dev
(ZTS build) and Perl 5.8.0.
