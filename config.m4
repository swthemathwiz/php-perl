dnl $Id: config.m4 174724 2004-12-14 18:04:36Z dmitry $
dnl config.m4 for extension perl

PHP_ARG_WITH([perl],
    [whether to enable Perl support],
    [AS_HELP_STRING([--with-perl[=PATH]],
        [path of Perl directory prefix; omit -> auto-detect])],
    [yes])

AC_ARG_ENABLE([perl-trace],
    [AS_HELP_STRING([--enable-perl-trace],
        [enable Perl extension tracing and start/stop functions])],
    [AC_DEFINE(PHP_PERL_TRACE, 1, [Whether to enable Perl extension tracing])])

if test "$PHP_PERL" != "no"; then
  if test "$PHP_PERL" = "yes"; then
    AC_PATH_PROG([PERL], [perl])
    if test -n "$PERL"; then
      PERL_PREFIX=`$PERL -V:prefix | sed -n "s/^prefix='\(.*\)';\$/\1/p"`
      if test -n "$PERL_PREFIX" && test -x "$PERL_PREFIX/bin/perl"; then
        PERL_DIR=$PERL_PREFIX
      else
        PERL_DIR=${PERL%/bin/perl}
      fi
    fi
  else
    PERL_DIR=$PHP_PERL
  fi

  AC_MSG_CHECKING([for Perl prefix directory])
  if test -z "$PERL_DIR" || test ! -x "$PERL_DIR/bin/perl"; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find a Perl executable. Use --with-perl=PATH to specify the Perl prefix directory.])
  fi
  AC_MSG_RESULT([$PERL_DIR])

  $PERL_DIR/bin/perl -MExtUtils::Embed -e 'exit 0' || AC_MSG_ERROR([Cannot use perl ExtUtils::Embed package])

  EXTRA_CFLAGS=`$PERL_DIR/bin/perl -MExtUtils::Embed -e ccopts`
  EXTRA_LDFLAGS=`$PERL_DIR/bin/perl -MExtUtils::Embed -e ldopts`

  PHP_SUBST(EXTRA_CFLAGS)
  PHP_SUBST(EXTRA_LDFLAGS)

  dnl Standard extension setup
  PHP_NEW_EXTENSION(perl, php_perl.c, $ext_shared)

  dnl Pull in Makefile.frag
  PHP_ADD_MAKEFILE_FRAGMENT
fi
