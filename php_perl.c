/*
 +----------------------------------------------------------------------+
 | PHP Version 7                                                        |
 +----------------------------------------------------------------------+
 | Copyright (c) 1997-2003 The PHP Group                                |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.0 of the PHP license,       |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_0.txt.                                  |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author: Dmitry Stogov <dmitry@zend.com>                              |
 +----------------------------------------------------------------------+
 */

#ifdef COMPILE_DL_PERL
#define HAVE_PERL 1
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if 0
#define TRACE_SUB( S )           int _trace_rc ZEND_ATTRIBUTE_UNUSED = fprintf( stderr, "%s\n", ( S ) ) | fflush( stderr );
#define TRACE_MSG( S )           do { fprintf( stderr, "%s\n", ( S ) ); fflush( stderr ); } while( 0 )
#define TRACE_MSG2( F, V )       do { fprintf( stderr, F "\n", ( V ) ); fflush( stderr ); } while( 0 )
#define TRACE_MSG3( F, V1, V2 )  do { fprintf( stderr, F "\n", ( V1 ), ( V2 ) ); fflush( stderr ); } while( 0 )
#define TRACE_SV_DUMP( SV )      do { sv_dump( ( SV ) ); } while( 0 )
#define TRACE_ASSERT( C )        do { if( !( C ) ) abort(); } while( 0 )
#define TEST_START_STOP
#else
#define TRACE_SUB( S )
#define TRACE_MSG( S )
#define TRACE_MSG2( F, V )
#define TRACE_MSG3( F, V1, V2 )
#define TRACE_SV_DUMP( SV )
#define TRACE_ASSERT( C )
#undef  TEST_START_STOP
#endif /* if 0 */

#ifndef ZEND_EXTENSION_API_NO_8_0_X
#define ZEND_EXTENSION_API_NO_8_0_X 420200930
#endif
#ifndef ZEND_EXTENSION_API_NO_7_4_X
#define ZEND_EXTENSION_API_NO_7_4_X 320190902
#endif

#if HAVE_PERL

#include <EXTERN.h>     /* from the Perl distribution */
#include <perl.h>       /* from the Perl distribution */
#include <perliol.h>    /* from the Perl distribution */
#include <perlapi.h>    /* from the Perl distribution */
#include <XSUB.h>       /* from the Perl distribution */

#if !defined(PERL_VERSION_LT) || !defined(PERL_VERSION_GE)
/* Shamelessly taken from perl/core/vutil.h */
#define PERL_VERSION_DECIMAL(r,v,s) (r*1000000 + v*1000 + s)
#define PERL_DECIMAL_VERSION \
        PERL_VERSION_DECIMAL(PERL_REVISION,PERL_VERSION,PERL_SUBVERSION)
#define PERL_VERSION_LT(r,v,s) \
        (PERL_DECIMAL_VERSION < PERL_VERSION_DECIMAL(r,v,s))
#define PERL_VERSION_GE(r,v,s) \
        (PERL_DECIMAL_VERSION >= PERL_VERSION_DECIMAL(r,v,s))
#endif

#undef END_EXTERN_C     /* bypass macros redeclaration warning (defined in Perl and PHP) */

#include "php.h"
#include "zend_objects_API.h"
#include "zend_exceptions.h"
#include "zend_extensions.h"
#include "ext/standard/info.h"
#include "SAPI.h"

#include "php_perl.h"

#ifndef ZEND_EXTENSION_API_NO_7_4_X
#  define ZEND_EXTENSION_API_NO_7_4_X     320190902
#endif
#ifndef ZEND_EXTENSION_API_NO_8_0_X
#  define ZEND_EXTENSION_API_NO_8_0_X     420200930
#endif

ZEND_BEGIN_MODULE_GLOBALS( perl )
  PerlInterpreter *perl;
  HashTable        perl_objects; /* this hash is used to make one to one mapping between Perl and PHP objects */
  zend_bool        perl_sys_inited; /* if this module has been sys inited */
ZEND_END_MODULE_GLOBALS( perl )

ZEND_DECLARE_MODULE_GLOBALS( perl );
static PHP_GINIT_FUNCTION( perl );
static PHP_GSHUTDOWN_FUNCTION( perl );

#define PERLG(v) ZEND_MODULE_GLOBALS_ACCESSOR( perl, v )

#ifdef TEST_START_STOP
PHP_FUNCTION( perl_start );
PHP_FUNCTION( perl_stop );

ZEND_BEGIN_ARG_INFO(arginfo_perl_start_stop, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry perl_static_functions[] = {
  PHP_FE( perl_start, arginfo_perl_start_stop )
  PHP_FE( perl_stop, arginfo_perl_start_stop )
  PHP_FE_END
};
#endif

PHP_METHOD( Perl, eval );
PHP_METHOD( Perl, require );

ZEND_BEGIN_ARG_INFO(arginfo_perl_eval, 0)
        ZEND_ARG_INFO(0, perl_code)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(arginfo_perl_require, 0)
        ZEND_ARG_INFO(0, perl_filename)
ZEND_END_ARG_INFO()

static const zend_function_entry perl_methods[] = {
  PHP_ME( Perl, eval,    arginfo_perl_eval,    ZEND_ACC_PUBLIC )
  PHP_ME( Perl, require, arginfo_perl_require, ZEND_ACC_PUBLIC )
  PHP_FE_END
};

/****************************************************************************/
/* This code was produced by `perl -MExtUtils::Embed -e xsinit`             */

EXTERN_C void xs_init( pTHX );

EXTERN_C void boot_DynaLoader( pTHX_ CV *cv );

EXTERN_C void
xs_init( pTHX )
{
  TRACE_SUB( "xs_init" );

  static const char file[] = __FILE__;

  dXSUB_SYS;
  PERL_UNUSED_CONTEXT;

  /* DynaLoader is a special case */
  newXS( "DynaLoader::boot_DynaLoader", boot_DynaLoader, file );
} /* xs_init */

/****************************************************************************/

typedef enum _perl_context {
  PERL_SCALAR,
  PERL_ARRAY,
  PERL_HASH
} perl_context;

typedef enum _perl_kind {
  PERL_NORMAL,  /* Normal variable */
  PERL_SPECIAL, /* Special keyword applied */
  PERL_PROXY    /* Proxy used for get/set */
} perl_kind;

typedef struct php_perl_object {
  SV          *sv;             /* Perl's representation of object */
  HashTable   *properties;     /* Temporary collection of object properties */
  perl_context context;        /* flag for next perl call, property */
  perl_kind    kind;           /* normal, proxy, or cleaner */
  zend_bool    remembered;     /* remembered or not */
  void        *remembered_sv;  /* the one we entered in the table */

  /* N.B.: PHP7 requires at end of data (additional data may be allocated beyond end) */
  zend_object  std;
} php_perl_object;

/* Three basic types used by Zend to access Perl objects (with members or offsets) */
#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_8_0_X)

typedef zend_object *php_perl_zop;              /* Zend object pointer */
#define php_perl_from_zop(V)                    (php_perl_from_zend((V)))
#define php_perl_zop_to_ce(V)                   ((V)->ce)

typedef zend_string *php_perl_mp;               /* Zend member pointer */
#define php_perl_get_member_string(V)           (V)
#define php_perl_release_member_string(V)

typedef zval *php_perl_offp;                    /* Zend offset pointer */
#define php_perl_offset_to_member(V)            (zval_get_string((V)))
#define php_perl_release_offset_member(V)       (zend_string_release((V)))

#else

typedef zval *php_perl_zop;                     /* Zend object pointer */
#define php_perl_from_zop(V)                    (php_perl_from_zend(Z_OBJ_P((V))))
#define php_perl_zop_to_ce(V)                   (Z_OBJCE_P(V))

typedef zval *php_perl_mp;                      /* Zend member pointer */
#define php_perl_get_member_string(V)           (zval_get_string((V)))
#define php_perl_release_member_string(V)       (zend_string_release((V)))

typedef zval *php_perl_offp;                    /* Zend offset pointer */
#define php_perl_offset_to_member(V)            (V)
#define php_perl_release_offset_member(V)

#endif

/* Function to move from zend object to perl object */
static inline php_perl_object *
php_perl_from_zend( zend_object *object )
{
  return (php_perl_object *)( (char *)object - offsetof( php_perl_object, std ) );
} /* php_perl_from_zend */

/* Function to move from perl object to zend object */
static inline zend_object *
php_perl_to_zend( php_perl_object *object )
{
  return (zend_object *)( (char *)object + offsetof( php_perl_object, std ) );
} /* php_perl_to_zend */

static zend_class_entry    *php_perl_ce;
static zend_class_entry    *php_perl_exception_ce;
static zend_object_handlers php_perl_object_handlers;
static zend_object_handlers php_perl_proxy_handlers;

/* PHP <-> Perl data conversion routines */
static SV   *php_perl_zval_to_sv_noref( zval *zv, HashTable *var_hash );
static SV   *php_perl_zval_to_sv_ref( zval *zv, HashTable *var_hash );
static SV   *php_perl_zval_to_sv( zval *zv );
static zval *php_perl_sv_to_zval_noref( SV *sv, zval *zv, HashTable *var_hash );
static zval *php_perl_sv_to_zval_ref( SV *sv, zval *zv, HashTable *var_hash );
static zval *php_perl_sv_to_zval( SV *sv, zval *zv );

/* Handlers for Perl objects overloading */
static zval *php_perl_read_property( php_perl_zop object, php_perl_mp member, int type, void * *cache_slot, zval *rv );
#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_7_4_X)
static zval *php_perl_write_property( php_perl_zop object, php_perl_mp member, zval *value, void * *cache_slot );
#else
static void  php_perl_write_property( php_perl_zop object, php_perl_mp member, zval *value, void * *cache_slot );
#endif
static int   php_perl_has_property( php_perl_zop object, php_perl_mp member, int has_set_exists, void * *cache_slot );
static void  php_perl_unset_property( php_perl_zop object, php_perl_mp member, void * *cache_slot );
static zval *php_perl_read_dimension( php_perl_zop object, php_perl_offp offset, int type, zval *rv );
static void  php_perl_write_dimension( php_perl_zop object, php_perl_offp offset, zval *value );
static int   php_perl_has_dimension( php_perl_zop object, php_perl_offp offset, int check_empty );
static void  php_perl_unset_dimension( php_perl_zop object, php_perl_offp offset );
static zval *php_perl_get_property_ptr_ptr( php_perl_zop object, php_perl_mp member, int type, void * *cache_slot );
#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_8_0_X)
static int   php_perl_do_operation( zend_uchar opcode, zval *result, zval *op1, zval *op2 );
#endif

static SV *
PerlIOPHP_getarg( pTHX_ PerlIO *f, CLONE_PARAMS *param, int flags )
{
  TRACE_SUB( "PerlIOPHP_getarg" );

  Perl_croak( aTHX_ "an attempt to getarg from a stale io handle" );
  return NULL;
} /* PerlIOPHP_getarg */

static SSize_t
PerlIOPHP_write( pTHX_ PerlIO *f, const void *vbuf, Size_t count )
{
  TRACE_SUB( "PerlIOPHP_write" );

  return zend_write( vbuf, count );
} /* PerlIOPHP_write */

static IV
PerlIOPHP_flush( pTHX_ PerlIO *f )
{
  TRACE_SUB( "PerlIOPHP_flush" );

  sapi_flush();
  return 0;
} /* PerlIOPHP_flush */

static IV
PerlIOPHP_noop_fail( pTHX_ PerlIO *f )
{
  TRACE_SUB( "PerlIOPHP_noop_fail" );

  return -1;
} /* PerlIOPHP_noop_fail */

static PerlIO_funcs PerlIO_PHP = {
  sizeof( PerlIO_funcs ),
  "PHP",
  sizeof( struct _PerlIO ),
  PERLIO_K_MULTIARG | PERLIO_K_RAW,
  PerlIOBase_pushed,
  PerlIOBase_popped,
  NULL,
  PerlIOBase_binmode,
  PerlIOPHP_getarg,
  PerlIOBase_fileno,
  PerlIOBase_dup,
  PerlIOBase_read,
  NULL,
  PerlIOPHP_write,
  NULL,                         /* can't seek on STD{IN|OUT}, fail on call*/
  NULL,                         /* can't tell on STD{IN|OUT}, fail on call*/
  PerlIOBase_close,
  PerlIOPHP_flush,
  PerlIOPHP_noop_fail,          /* fill */
  PerlIOBase_eof,
  PerlIOBase_error,
  PerlIOBase_clearerr,
  PerlIOBase_setlinebuf,
  NULL,                         /* get_base */
  NULL,                         /* get_bufsiz */
  NULL,                         /* get_ptr */
  NULL,                         /* get_cnt */
  NULL,                         /* set_ptrcnt */
};

/* Creates Perl interpreter if it was not created before */
static PerlInterpreter *
php_perl_init( void )
{
  PerlInterpreter *my_perl = PERLG( perl );

  if( my_perl == NULL ) {
    TRACE_SUB( "php_perl_init" );

    char *embedding[] = { "", "-e", "0" };

    if( !PERLG( perl_sys_inited ) ) {
      PERL_SYS_INIT3((int *)NULL,(char ***)NULL,(char ***)NULL);
      PERLG( perl_sys_inited ) = TRUE;
    }
    my_perl = perl_alloc();

    PL_perl_destruct_level = 1;
    perl_construct( my_perl );

    PL_origalen = 1; /* don't let $0 assignment update the proctitle or embedding[0] */
    perl_parse( my_perl, xs_init, sizeof(embedding)/sizeof(embedding[0]), embedding, (char * *)NULL );
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;

    PerlIO_define_layer( aTHX_ & PerlIO_PHP );
    PerlIO_push(aTHX_ PerlIO_stdout(), &PerlIO_PHP, "w", NULL);

    TRACE_MSG2( "  perl interpreter " ZEND_ADDR_FMT " created", (zend_ulong)my_perl );
    PERLG( perl )  = my_perl;
    zend_hash_init( &PERLG( perl_objects ), 0, NULL, NULL, 0 );
  }

  return my_perl;
} /* php_perl_init */

/* Destroys Perl interpreter if it was created before */
static void
php_perl_destroy( void )
{
  TRACE_SUB( "php_perl_destroy" );

  PerlInterpreter *my_perl = PERLG( perl );

  if( my_perl != NULL ) {
    PerlIO_flush( PerlIO_stdout() );
    PerlIO_pop(aTHX_ PerlIO_stdout());

    PL_perl_destruct_level = 1;
    perl_destruct( my_perl );
    perl_free( my_perl );

    TRACE_MSG2( "  perl interpreter " ZEND_ADDR_FMT " freed", (zend_ulong)my_perl );
    zend_hash_destroy( &PERLG( perl_objects ) );
    PERLG( perl ) = NULL;

#if PERL_VERSION_LT(5,32,0) && PERL_VERSION_GE(5,28,0)
    /* Refer to discussion here: https://github.com/Perl/perl5/issues/17154 */
    PL_InBitmap = NULL;
#endif
  }
} /* php_perl_destroy */

static void
binary_hash_add( HashTable *hash, const void *key, void *data )
{
  TRACE_SUB( "binary_hash_add" );

  if( sizeof( zend_ulong ) >= sizeof( void * ) ) {
    TRACE_MSG3( "  key = " ZEND_ADDR_FMT " <-> " ZEND_ADDR_FMT, (size_t)key, (size_t)data );
    zend_hash_index_add_ptr( hash, (zend_ulong)key, data );
  }
  else {
    char key_str[ZEND_LTOA_BUF_LEN];
    zend_sprintf( key_str, ZEND_ADDR_FMT, (size_t)key );
    TRACE_MSG3( "  key = %s <-> " ZEND_ADDR_FMT, key_str, (size_t)data );

    zend_hash_str_add_ptr( hash, key_str, strlen( key_str ), data );
  }
} /* binary_hash_add */

/* Remembers mapping between Perl (SV) and PHP object (zend_object) */
static void
binary_hash_del( HashTable *hash, const void *key )
{
  TRACE_SUB( "binary_hash_del" );

  if( sizeof( zend_ulong ) >= sizeof( void * ) ) {
    TRACE_MSG2( "  key = " ZEND_ADDR_FMT " deleted", (size_t)key );
    zend_hash_index_del( hash, (zend_ulong)key );
  }
  else {
    char key_str[ZEND_LTOA_BUF_LEN];
    zend_sprintf( key_str, ZEND_ADDR_FMT, (size_t)key );
    TRACE_MSG2( "  key = %s deleted", key_str );
    zend_hash_str_del( hash, key_str, strlen( key_str ) );
  }
} /* binary_hash_del */

static inline int
binary_hash_find( HashTable *hash, const void *key, void * *data )
{
  TRACE_SUB( "binary_hash_find" );

  if( sizeof( zend_ulong ) >= sizeof( void * ) ) {
    *data = zend_hash_index_find_ptr( hash, (zend_ulong)key );
    TRACE_MSG3( "  key = " ZEND_ADDR_FMT " %s", (size_t)key, ( *data == NULL ) ? "not found" : "found" );
  }
  else {
    char key_str[ZEND_LTOA_BUF_LEN];
    zend_sprintf( key_str, ZEND_ADDR_FMT, (size_t)key );
    *data = zend_hash_str_find_ptr( hash, key_str, strlen( key_str ) );
    TRACE_MSG3( "  key = %s %s", key_str, ( *data == NULL ) ? "not found" : "found" );
  }

  return ( *data == NULL ) ? FAILURE : SUCCESS;
} /* binary_hash_find */

static void
php_perl_remember_object( zend_object *object )
{
  TRACE_SUB( "php_perl_remember_object" );

  php_perl_object *pobj = php_perl_from_zend( object );
  if( pobj->sv != NULL ) {
    SV *remembered_sv = SvRV( pobj->sv );
    binary_hash_add( &PERLG( perl_objects ), remembered_sv, (void *)object );
    pobj->remembered_sv = (void *)remembered_sv;
    pobj->remembered    = TRUE;
  }
  else {
    pobj->remembered = FALSE;
  }
} /* php_perl_remember_object */

/* Recalls mapping between Perl (SV) and PHP object (zend_object) */
static inline int
php_perl_recall_object( const SV *sv, zend_object * *object )
{
  TRACE_SUB( "php_perl_recall_object" );

  return binary_hash_find( &PERLG( perl_objects ), SvRV( sv ), (void * *)object );
} /* php_perl_recall_object */

/* Forgets mapping between Perl (SV) and PHP object (zend_object) */
static void
php_perl_forget_object( zend_object *object )
{
  TRACE_SUB( "php_perl_forget_object" );

  php_perl_object *pobj = php_perl_from_zend( object );
  if( pobj->remembered ) {
    binary_hash_del( &PERLG( perl_objects ), (SV *)pobj->remembered_sv );
    pobj->remembered = FALSE;
  }
} /* php_perl_forget_object */

/* Determine if a zval is a php-perl object */
static inline zend_bool
php_perl_is_our_zval( zval *zv )
{
  TRACE_SUB( "php_perl_is_our_zval" );

  return zv != NULL && Z_TYPE_P( zv ) == IS_OBJECT && Z_OBJCE_P( zv ) == php_perl_ce;
} /* php_perl_is_our_zval */

/* Initialize a new zval with the given Perl SV */
static zval *
php_perl_create_new_object( zval *zv, SV *sv, perl_kind kind )
{
  TRACE_SUB( "php_perl_create_new_object" );

  TRACE_ASSERT( zv != NULL );

  /* This call should call create_object, which initializes the object */
  object_init_ex( zv, php_perl_ce );

  /* Now the set the parameters */
  {
    php_perl_object *pobj = php_perl_from_zend( Z_OBJ_P( zv ) );

    TRACE_ASSERT( pobj->sv == NULL );
    TRACE_ASSERT( pobj->kind == PERL_NORMAL );
    TRACE_ASSERT( pobj->properties == NULL );
    TRACE_ASSERT( pobj->remembered == FALSE );
    TRACE_ASSERT( pobj->context == PERL_SCALAR );
    TRACE_ASSERT( Z_OBJ_HT_P( zv ) == &php_perl_object_handlers );

    pobj->kind = kind;

    if( kind == PERL_SPECIAL || kind == PERL_PROXY )
      SvREFCNT_inc( sv );
    pobj->sv = sv;

    TRACE_MSG2( "create perl object with sv => 0x%lx", (zend_ulong)pobj->sv );
    TRACE_SV_DUMP( pobj->sv );
  }

  /* Use the proxy handlers if requested */
  if( kind == PERL_PROXY )
    Z_OBJ_HT_P( zv ) = &php_perl_proxy_handlers;

  /* If not a proxy remember the object, so that it can be cross-referenced */
  if( kind != PERL_PROXY )
    php_perl_remember_object( Z_OBJ_P( zv ) );

  return zv;
} /* php_perl_create_new_object */

/* Get the zval storage area for the index */
static inline zval *
php_perl_hash_index_get_zval( HashTable *ht, zend_ulong index )
{
  return zend_hash_index_update( ht, index, &EG( uninitialized_zval ) );
} /* php_perl_hash_index_get_zval */

/* Get the zval storage area for the given key */
static inline zval *
php_perl_hash_str_get_zval( HashTable *ht, const char *key, size_t key_len )
{
  return zend_hash_str_add( ht, key, key_len, &EG( uninitialized_zval ) );
} /* php_perl_hash_str_get_zval */

/* Get the zval storage area from the array zval for the index */
static inline zval *
php_perl_array_get_zval( zval *zv, zend_ulong index )
{
  return zend_hash_index_update( Z_ARRVAL_P( zv ), index, &EG( uninitialized_zval ) );
} /* php_perl_array_get_zval */

/* Get the zval storage area from the array zval for the given key */
static inline zval *
php_perl_hash_get_zval( zval *zv, const char *key, size_t key_len )
{
  return zend_hash_str_add( Z_ARRVAL_P( zv ), key, key_len, &EG( uninitialized_zval ) );
} /* php_perl_hash_get_zval */

/* Dereference a Perl SV */
static inline SV *
php_perl_deref( SV *sv )
{
  while( SvROK( sv ) )
    sv = SvRV( sv );
  return sv;
} /* php_perl_deref */

/* Converts PHP's value to Perl's equivalent */
static SV *
php_perl_zval_to_sv( zval *zv )
{
  TRACE_SUB( "php_perl_zval_to_sv" );

  HashTable var_hash;
  SV       *sv;

  zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
  sv = php_perl_zval_to_sv_ref( zv, &var_hash );
  zend_hash_destroy( &var_hash );
  return sv;
} /* php_perl_zval_to_sv */

static SV *
php_perl_zval_to_sv_ref( zval *zv,
                         HashTable *var_hash )
{
  TRACE_SUB( "php_perl_zval_to_sv_ref" );

  SV *sv;
  if( ( Z_ISREF_P( zv ) || Z_TYPE_P( zv ) == IS_OBJECT || Z_TYPE_P( zv ) == IS_ARRAY ) &&
      binary_hash_find( var_hash, zv, (void * *)&sv ) == SUCCESS ) {
    SvREFCNT_inc( sv );
    return sv;
  }

  sv = php_perl_zval_to_sv_noref( zv, var_hash );

  if( ( Z_ISREF_P( zv ) || Z_TYPE_P( zv ) == IS_OBJECT || Z_TYPE_P( zv ) == IS_ARRAY ) )
    binary_hash_add( var_hash, zv, sv );
  return sv;
} /* php_perl_zval_to_sv_ref */

static SV *
php_perl_zval_to_sv_noref( zval *zv,
                           HashTable *var_hash )
{
  TRACE_SUB( "php_perl_zval_to_sv_noref" );

  TRACE_MSG2( "  zend %s", zend_get_type_by_const( Z_TYPE_P( zv ) ) );
  switch( Z_TYPE_P( zv ) ) {
    case IS_NULL:
    case IS_UNDEF:
      return &PL_sv_undef;

    case IS_TRUE:
      return &PL_sv_yes;

    case IS_FALSE:
      return &PL_sv_no;

    case IS_LONG:
      return newSViv( Z_LVAL_P( zv ) );

    case IS_DOUBLE:
      return newSVnv( Z_DVAL_P( zv ) );

    case IS_STRING:
      return newSVpv( Z_STRVAL_P( zv ), Z_STRLEN_P( zv ) );

    case IS_ARRAY:
      {
        SV        *sv;
        HashTable *ht      = Z_ARRVAL_P( zv );
        zend_bool  is_hash = FALSE;

        /* checking if 'hv' is array or hash */
        for( zend_hash_internal_pointer_reset( ht );
             zend_hash_get_current_data( ht ) != NULL;
             zend_hash_move_forward( ht ) ) {
          if( zend_hash_get_current_key_type( ht ) == HASH_KEY_IS_STRING ) {
            is_hash = TRUE;
            break;
          }
        }

        TRACE_MSG2( "  zend array ishash=%d", (int)is_hash );

        /* converting to Perl's hash */
        if( is_hash ) {
          HV *hv = newHV();

          sv = (SV *)newRV( (SV *)hv );

          /* do this before return in case a hash element references itself */
          binary_hash_add( var_hash, zv, sv );

          for( zend_hash_internal_pointer_reset( ht );
               zend_hash_get_current_data( ht ) != NULL;
               zend_hash_move_forward( ht ) ) {
            zend_string *key;
            zend_ulong   index;

            if( zend_hash_get_current_key( ht, &key, &index ) != HASH_KEY_IS_STRING ) {
              char xkey[ZEND_LTOA_BUF_LEN];
              zend_sprintf( xkey, ZEND_ULONG_FMT, index );
              hv_store( hv, xkey, strlen( xkey ), php_perl_zval_to_sv_ref( zend_hash_get_current_data( ht ), var_hash ), 0 );
            }
            else {
              hv_store( hv, ZSTR_VAL( key ), ZSTR_LEN( key ), php_perl_zval_to_sv_ref( zend_hash_get_current_data( ht ), var_hash ), 0 );
            }
          }
        }
        /* converting to Perl's array */
        else {
          AV *av = newAV();

          sv = (SV *)newRV( (SV *)av );

          /* do this before return in case an array element references itself */
          binary_hash_add( var_hash, zv, sv );

          for( zend_hash_internal_pointer_reset( ht );
               zend_hash_get_current_data( ht ) != NULL;
               zend_hash_move_forward( ht ) ) {
            zend_string *key;
            zend_ulong   index;

            if( zend_hash_get_current_key( ht, &key, &index ) == HASH_KEY_IS_LONG )
              av_store( av, index, php_perl_zval_to_sv_ref( zend_hash_get_current_data( ht ), var_hash ) );
          }
        }
        return sv;
      }

    case IS_OBJECT:
      if( php_perl_is_our_zval( zv ) ) {
        php_perl_object *obj = php_perl_from_zend( Z_OBJ_P( zv ) );
        TRACE_MSG( "  zend perl object" );
        return newSVsv( obj->sv );
      }

      /* We got some other type of object */
      {
        zend_string *name = zend_std_get_class_name( Z_OBJ_P( zv ) );
        TRACE_MSG2( "zend '%s' object", ZSTR_VAL( name ) );
        zend_error( E_ERROR, "[perl] Can't convert class '%s' object to Perl", ZSTR_VAL( name ) );
        zend_string_release( name );
      }
      return &PL_sv_undef;

    case IS_REFERENCE:
      return (SV *)newRV( php_perl_zval_to_sv_ref( Z_REFVAL_P( zv ), var_hash ) );

    default:
      break;
  } /* switch */

  zend_error( E_ERROR, "[perl] Can't convert PHP type '%s' (%d) to Perl", zend_get_type_by_const( Z_TYPE_P( zv ) ), (int)Z_TYPE_P( zv ) );
  return &PL_sv_undef;
}   /* php_perl_zval_to_sv_noref */

/* Converts Perl's value to PHP's equivalent */
static zval *
php_perl_sv_to_zval( SV *sv, zval *zv )
{
  TRACE_SUB( "php_perl_sv_to_zval" );

  HashTable var_hash;

  zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
  zv = php_perl_sv_to_zval_ref( sv, zv, &var_hash );
  zend_hash_destroy( &var_hash );
  return zv;
}   /* php_perl_sv_to_zval */

static zval *
php_perl_sv_to_zval_ref( SV *sv,
                         zval *zv,
                         HashTable *var_hash )
{
  TRACE_SUB( "php_perl_sv_to_zval_ref" );

  TRACE_ASSERT( zv != NULL );

  {
    zval *z;
    if( SvREFCNT( sv ) > 1 && binary_hash_find( var_hash, sv, (void * *)&z ) == SUCCESS ) {
      TRACE_MSG2( "  linking 0x%lx", (zend_ulong)zv );
      TRACE_MSG2( "       to 0x%lx", (zend_ulong)z );
      ZVAL_NEW_REF( zv, z );
      return zv;
    }
  }

  /* N.B.: Sv<->ZV connected before call means that zv must be filled */
  if( SvREFCNT( sv ) > 1 )
    binary_hash_add( var_hash, sv, zv );

  return php_perl_sv_to_zval_noref( sv, zv, var_hash );
} /* php_perl_sv_to_zval_ref */

static zval *
php_perl_sv_to_zval_noref( SV *sv,
                           zval *zv,
                           HashTable *var_hash )
{
  TRACE_SUB( "php_perl_sv_to_zval_noref" );

  if( sv ) {
    TRACE_SV_DUMP( sv );

    if( SvTYPE( sv ) == SVt_NULL ) { /* null */
      ZVAL_NULL( zv );
      TRACE_MSG( "  zval null" );
    }
    else if( SvIOK( sv ) ) {              /* integer */
      ZVAL_LONG( zv, SvIV( sv ) );
      TRACE_MSG2( "  zval long (%ld)", (long)Z_LVAL_P( zv ) );
    }
    else if( SvNOK( sv ) ) {              /* double */
      ZVAL_DOUBLE( zv, SvNV( sv ) );
      TRACE_MSG2( "  zval double (%lf)", (double)Z_DVAL_P( zv ) );
    }
    else if( SvPOK( sv ) ) {              /* string */
      STRLEN len;
      char  *str = SvPV( sv, len );
      ZVAL_STRINGL( zv, str, len );
      TRACE_MSG2( "  zval string (\"%s\")", Z_STRVAL_P( zv ) );
    }
    else if( sv_isobject( sv ) ) {        /* object */
      zend_object *recalled_object;
      TRACE_MSG2( "  zval object (sv=%lx)", (long)sv );

      /* Check for recursion here */
      if( php_perl_recall_object( sv, &recalled_object ) == SUCCESS ) {
        TRACE_MSG2( "  creating recalled object for (sv=%lx)", (long)sv );
        ZVAL_NEW_REF( zv, &EG( uninitialized_zval ) );
        ZVAL_OBJ( Z_REFVAL_P( zv ), recalled_object );
        GC_ADDREF( recalled_object );
      }
      else {
        php_perl_create_new_object( zv, sv, PERL_NORMAL );
        SvREFCNT_inc( sv );
      }
    }
    else if( SvROK( sv ) ) {              /* reference */
      TRACE_MSG( "  zval reference" );
      ZVAL_NEW_REF( zv, &EG( uninitialized_zval ) );
      php_perl_sv_to_zval_ref( SvRV( sv ), Z_REFVAL_P( zv ), var_hash );
    }
    else if( SvTYPE( sv ) == SVt_PVAV ) { /* array */
      I32 i   = 0;
      I32 len = av_len( (AV *)sv );

      TRACE_MSG2( "  zval array size[%d]", (int)len + 1 );
      array_init_size( zv, len + 1 );
      for( i = 0; i <= len; i++ ) {
        SV * *el_sv = av_fetch( (AV *)sv, i, 0 );
        if( el_sv != NULL && *el_sv != NULL ) {
          TRACE_MSG2( "zval array [%d]", (int)i );
          php_perl_sv_to_zval_ref( *el_sv, php_perl_array_get_zval( zv, i ), var_hash );
        }
      }
    }
    else if( SvTYPE( sv ) == SVt_PVHV ) { /* hash */
      SV   *el_sv;
      char *key;
      I32   key_len;

      TRACE_MSG( "  zval hash" );
      array_init( zv );
      hv_iterinit( (HV *)sv );
      while( ( el_sv = hv_iternextsv( (HV *)sv, &key, &key_len ) ) != NULL ) {
        TRACE_MSG2( "  zval key (%s)", key );
        php_perl_sv_to_zval_ref( el_sv, php_perl_hash_get_zval( zv, key, key_len ), var_hash );
      }
    }
    else {
      zend_error( E_ERROR, "[perl] Can't convert Perl type (%ld) to PHP", (long)SvTYPE( sv ) );
    }
  }
  TRACE_MSG( "  zval done" );
  return zv;
} /* php_perl_sv_to_zval_noref */

/****************************************************************************/

/* Calls constructor of Perl's class and returns created object */
static SV *
php_perl_call_constructor( const char *class_name,
                           size_t class_name_len,
                           const char *constructor,
                           size_t constructor_len,
                           int argc,
                           zval *argv )
{
  TRACE_SUB( "php_perl_call_constructor" );

  SV    *ret;
  size_t func_len = class_name_len + constructor_len + 3;
  char  *func     = (char *)emalloc( func_len );

  dSP;                                                           /* initialize stack pointer         */

  int i;

  strcpy( func, class_name );
  strcpy( func + class_name_len, "::" );
  strcpy( func + class_name_len + 2, constructor );

  TRACE_MSG2( "call constructor '%s'", func );

  ENTER;                                                         /* everything created after here    */
  SAVETMPS;                                                      /* ...is a temporary variable.      */
  PUSHMARK( SP );                                                /* remember the stack pointer       */

  XPUSHs( sv_2mortal( newSVpv( class_name, class_name_len ) ) );
  for( i = 0; i < argc; i++ ) {
    XPUSHs( sv_2mortal( php_perl_zval_to_sv( argv + i ) ) );
  }

  PUTBACK;                                                       /* make local stack pointer global  */
  call_pv( func, G_SCALAR | G_EVAL );                            /* call the function                */
  SPAGAIN;                                                       /* refresh stack pointer            */

  ret = POPs;
  SvREFCNT_inc( ret );

  PUTBACK;
  FREETMPS;                                                      /* free that return value           */
  LEAVE;                                                         /* ...and the XPUSHed "mortal" args.*/

  efree( func );
  return ret;
} /* php_perl_call_constructor */

/* Calls method of Perl's object */
static void
php_perl_call_method( SV *obj,
                      const char *func, int argc, zval *argv,
                      perl_context context,
                      zval *return_value )
{
  TRACE_SUB( "php_perl_call_method" );

  dSP;                                                           /* initialize stack pointer         */

  int i;

  TRACE_MSG2( "call method '%s'", func );

  ENTER;                                                         /* everything created after here    */
  SAVETMPS;                                                      /* ...is a temporary variable.      */
  PUSHMARK( SP );                                                /* remember the stack pointer       */

  XPUSHs( obj );

  for( i = 0; i < argc; i++ ) {
    XPUSHs( sv_2mortal( php_perl_zval_to_sv( argv + i ) ) );
  }

  PUTBACK;                                                       /* make local stack pointer global  */
  if( return_value != NULL ) {
    if( context != PERL_SCALAR ) {
      int       count, i;
      I32       ax;
      HashTable var_hash;

      count = call_method( func, G_ARRAY | G_EVAL );             /* call the function                */
      SPAGAIN;                                                   /* refresh stack pointer            */
      sp   -= count;
      ax    = ( sp - PL_stack_base ) + 1;
      zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
      array_init_size( return_value, count );
      if( context == PERL_ARRAY ) {
        for( i = 0; i < count; i++ ) {
          php_perl_sv_to_zval_ref( (SV *)ST( i ), php_perl_array_get_zval( return_value, i ), &var_hash );
        }
      }
      else {
        for( i = 0; i < count; i++ ) {
          STRLEN key_len;
          char  *key = SvPV( ST( i ), key_len );
          if( i != count - 1 )
            php_perl_sv_to_zval_ref( (SV *)ST( ++i ), php_perl_hash_get_zval( return_value, key, key_len ), &var_hash );
          else
            add_assoc_null_ex( return_value, key, key_len );
        }
      }
      zend_hash_destroy( &var_hash );
    }
    else {
      call_method( func, G_SCALAR | G_EVAL );                /* call the function                */
      SPAGAIN;                                               /* refresh stack pointer            */
      php_perl_sv_to_zval( POPs, return_value );
    }
  }
  else {
    call_method( func, G_DISCARD | G_EVAL );                 /* call the function                */
    SPAGAIN;                                                 /* refresh stack pointer            */
  }
  PUTBACK;
  FREETMPS;                                                  /* free that return value           */
  LEAVE;                                                     /* ...and the XPUSHed "mortal" args.*/
} /* php_perl_call_method */

/* Calls Perl's function */
static void
php_perl_call( const char *func, int argc, zval *argv,
               perl_context context,
               zval *return_value )
{
  TRACE_SUB( "php_perl_call" );

  dSP;                                                       /* initialize stack pointer         */

  int i;

  TRACE_MSG2( "call '%s'", func );

  ENTER;                                                     /* everything created after here    */
  SAVETMPS;                                                  /* ...is a temporary variable.      */
  PUSHMARK( SP );                                            /* remember the stack pointer       */

  for( i = 0; i < argc; i++ ) {
    XPUSHs( sv_2mortal( php_perl_zval_to_sv( argv + i ) ) );
  }
  PUTBACK;                                                   /* make local stack pointer global  */
  if( return_value != NULL ) {
    if( context != PERL_SCALAR ) {
      int       count, i;
      I32       ax;
      HashTable var_hash;

      count = call_pv( func, G_ARRAY | G_EVAL );             /* call the function                */
      SPAGAIN;                                               /* refresh stack pointer            */
      sp   -= count;
      ax    = ( sp - PL_stack_base ) + 1;
      zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
      array_init_size( return_value, count );
      if( context == PERL_ARRAY ) {
        for( i = 0; i < count; i++ ) {
          php_perl_sv_to_zval_ref( (SV *)ST( i ), php_perl_array_get_zval( return_value, i ), &var_hash );
        }
      }
      else {
        for( i = 0; i < count; i++ ) {
          STRLEN key_len;
          char  *key = SvPV( ST( i ), key_len );
          if( i != count - 1 )
            php_perl_sv_to_zval_ref( (SV *)ST( ++i ), php_perl_hash_get_zval( return_value, key, key_len ), &var_hash );
          else
            add_assoc_null_ex( return_value, key, key_len );
        }
      }
      zend_hash_destroy( &var_hash );
    }
    else {
      call_pv( func, G_SCALAR | G_EVAL );             /* call the function                */
      SPAGAIN;                                        /* refresh stack pointer            */
      php_perl_sv_to_zval( POPs, return_value );
    }
  }
  else {
    call_pv( func, G_DISCARD | G_EVAL );              /* call the function                */
    SPAGAIN;                                          /* refresh stack pointer            */
  }
  PUTBACK;
  FREETMPS;                                           /* free that return value           */
  LEAVE;                                              /* ...and the XPUSHed "mortal" args.*/
} /* php_perl_call */

/****************************************************************************/

#if (ZEND_EXTENSION_API_NO < ZEND_EXTENSION_API_NO_8_0_X)
static zval *
php_perl_get( zval *object, zval *retval )
{
  TRACE_SUB( "php_perl_get" );

  php_perl_object         *obj       = php_perl_from_zend( Z_OBJ_P( object ) );
  SV                      *sv        = obj->sv;

  if( sv == NULL ) {
    zend_error( E_ERROR, "[perl] Cannot get value" );
    return NULL;
  }
  return php_perl_sv_to_zval( sv, retval );
} /* php_perl_get */

static void
php_perl_set( zval *object, zval *value )
{
  TRACE_SUB( "php_perl_set" );

  php_perl_object         *obj       = php_perl_from_zend( Z_OBJ_P( object ) );
  SV                      *sv        = obj->sv;

  if( sv == NULL ) {
    zend_error( E_ERROR, "[perl] Cannot set value" );
    return;
  }
  sv_setsv( sv, php_perl_zval_to_sv( value ) );
} /* php_perl_set */
#endif

/* Returns element of array based Perl's object */
static zval *
php_perl_read_dimension( php_perl_zop object, php_perl_offp offset_val, int type, zval *rv )
{
  TRACE_SUB( "php_perl_read_dimension" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;
  zend_bool                write     = ZEND_TRUTH( type != BP_VAR_R && type != BP_VAR_IS );
  zval                    *retval    = NULL;

  TRACE_ASSERT( rv != NULL );

  ZVAL_UNDEF( rv );

  if( sv == NULL ) {
    zend_error( E_ERROR, "[perl] Cannot get dimension" );
    return NULL;
  }

  TRACE_MSG2( "  read_dim write = %d", (int)write );

  sv = php_perl_deref( sv );

  if( SvTYPE( sv ) == SVt_PVAV ) {
    AV        *av = (AV *)sv;
    SV      * *prop_val;
    zend_ulong offset = zval_get_long(offset_val);

    TRACE_MSG2( "  read_dim offset = %ld", (long)offset );

    prop_val = av_fetch( av, offset, write );
    if( prop_val != NULL ) {
      /* Create a proxy variable for writing an object */
      if( write && !sv_isobject( *prop_val ) )
        retval = php_perl_create_new_object( rv, *prop_val, PERL_PROXY );
      else
        retval = php_perl_sv_to_zval( *prop_val, rv );
    }
  }
  else if( SvTYPE( sv ) == SVt_PVHV ) {
    php_perl_mp mp = php_perl_offset_to_member( offset_val );
    retval = php_perl_read_property( object, mp, type, NULL, rv );
    php_perl_release_offset_member( mp );
  }
  else {
    zend_error( E_WARNING, "[perl] Object is not an array" );
  }

  if( retval == NULL )
    return &EG( uninitialized_zval );
  else
    return retval;
} /* php_perl_read_dimension */

/* Sets element of array based Perl's object */
static void
php_perl_write_dimension( php_perl_zop object, php_perl_offp offset_val, zval *value )
{
  TRACE_SUB( "php_perl_write_dimension" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;

  if( sv == NULL ) {
    zend_error( E_ERROR, "[perl] Cannot set dimension" );
    return;
  }

  sv = php_perl_deref( sv );
  if( SvTYPE( sv ) == SVt_PVAV ) {
    AV *av = (AV *)sv;
    av_store( av, zval_get_long(offset_val), php_perl_zval_to_sv( value ) );
  }
  else if( SvTYPE( sv ) == SVt_PVHV ) {
    php_perl_mp mp = php_perl_offset_to_member( offset_val );
    php_perl_write_property( object, mp, value, NULL );
    php_perl_release_offset_member( mp );
  }
  else {
    zend_error( E_WARNING, "[perl] Object is not an array" );
  }
} /* php_perl_write_dimension */

/* Checks if element of array based Perl's object isset or empty */
static int
php_perl_has_dimension( php_perl_zop object, php_perl_offp offset_val, int check_empty )
{
  TRACE_SUB( "php_perl_has_dimension" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;
  int                      ret       = 0;

  if( sv == NULL ) {
    zend_error( E_ERROR, "[perl] Cannot check dimension" );
    return 0;
  }

  sv = php_perl_deref( sv );
  if( SvTYPE( sv ) == SVt_PVAV ) {
    AV        *av     = (AV *)sv;
    zend_ulong offset = zval_get_long(offset_val);

    /* empty() */
    if( check_empty ) {
      SV * *prop_val = av_fetch( av, offset, 0 );
      if( prop_val != NULL ) {
        zval zv;
        ZVAL_UNDEF( &zv );
        ret = zval_is_true( php_perl_sv_to_zval( *prop_val, &zv ) );
        zval_ptr_dtor( &zv );
      }
    }
    /* isset() */
    else if( av_exists( av, offset ) ) {
      ret = 1;
    }
  }
  else if( SvTYPE( sv ) == SVt_PVHV ) {
    php_perl_mp mp = php_perl_offset_to_member( offset_val );
    ret = php_perl_has_property( object, mp, check_empty, NULL );
    php_perl_release_offset_member( mp );
  }
  else {
    zend_error( E_WARNING, "[perl] Object is not an array" );
  }
  return ret;
} /* php_perl_has_dimension */

/* Deletes element of array based Perl's object */
static void
php_perl_unset_dimension( php_perl_zop object, php_perl_offp offset_val )
{
  TRACE_SUB( "php_perl_unset_dimension" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;

  if( sv == NULL ) {
    zend_error( E_ERROR, "[perl] Cannot unset dimension" );
    return;
  }

  sv = php_perl_deref( sv );
  if( SvTYPE( sv ) == SVt_PVAV ) {
    AV *av = (AV *)sv;
    av_delete( av, zval_get_long(offset_val), G_DISCARD );
  }
  else if( SvTYPE( sv ) == SVt_PVHV ) {
    php_perl_mp mp = php_perl_offset_to_member( offset_val );
    php_perl_unset_property( object, mp, NULL );
    php_perl_release_offset_member( mp );
  }
  else {
    zend_error( E_WARNING, "[perl] Object is not an array" );
  }
} /* php_perl_unset_dimension */

static zval *
php_perl_get_property_ptr_ptr( php_perl_zop object, php_perl_mp member_val, int type, void **cache_slot )
{
  TRACE_SUB( "php_perl_get_property_ptr_ptr" );

  /* Fallback to read_property. */
  return NULL;
} /* php_perl_get_property_ptr_ptr */

/* Returns property of hash based Perl's object */
static zval *
php_perl_read_property( php_perl_zop object, php_perl_mp member_val, int type, void * *key_, zval *rv )
{
  TRACE_SUB( "php_perl_read_property" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  zval                    *retval    = NULL;
  SV                      *sv        = NULL;
  zend_bool                write     = ZEND_TRUTH( type != BP_VAR_R && type != BP_VAR_IS );
  zend_string             *member    = php_perl_get_member_string(member_val);

  ZVAL_UNDEF( rv );

  /* All perl 'member' properties are variables, except for a few predefined specials */

  TRACE_MSG2( "read property member '%s'", ZSTR_VAL( member ) ? ZSTR_VAL( member ) : "<undefined>" );

  /* Handle the intermediate special operators */
  if( obj->kind != PERL_SPECIAL ) {
    if( zend_string_equals_literal( member, "array" ) ) {
      retval = php_perl_create_new_object( rv, obj->sv, PERL_SPECIAL );
      php_perl_from_zend( Z_OBJ_P( rv ) )->context = PERL_ARRAY;
      goto php_perl_read_property_cleanup;
    }
    else if( zend_string_equals_literal( member, "hash" ) ) {
      retval = php_perl_create_new_object( rv, obj->sv, PERL_SPECIAL );
      php_perl_from_zend( Z_OBJ_P( rv ) )->context = PERL_HASH;
      goto php_perl_read_property_cleanup;
    }
    else if( zend_string_equals_literal( member, "scalar" ) ) {
      retval = php_perl_create_new_object( rv, obj->sv, PERL_SPECIAL );
      php_perl_from_zend( Z_OBJ_P( rv ) )->context = PERL_SCALAR;
      goto php_perl_read_property_cleanup;
    }
  }

  TRACE_MSG( "rp1" );

  /* Trying the pull an array, hash, or scalar from Perl() object space */
  if( obj->sv == NULL ) {
    if( obj->context == PERL_ARRAY ) {
      sv = (SV *)get_av( ZSTR_VAL( member ), write );
      if( sv && !AvARRAY( sv ) ) {
        if( write )
          av_clear( (AV *)sv );
        else
          sv = NULL;
      }
    }
    else if( obj->context == PERL_HASH ) {
      sv = (SV *)get_hv( ZSTR_VAL( member ), write );
      if( sv && !HvARRAY( sv ) ) {
        if( write )
          hv_clear( (HV *)sv );
        else
          sv = NULL;
      }
    }
    else {
      TRACE_MSG( "rp2" );
      sv = get_sv( ZSTR_VAL( member ), FALSE );
      if( sv && !SvOK( sv ) )
        sv = NULL;
    }

    if( sv == NULL ) {
      if( obj->context == PERL_ARRAY )
        zend_error( E_NOTICE, "[perl] Undefined variable: '@%s'", ZSTR_VAL( member ) );
      else if( obj->context == PERL_HASH )
        zend_error( E_NOTICE, "[perl] Undefined variable: '%%%s'", ZSTR_VAL( member ) );
      else
        zend_error( E_NOTICE, "[perl] Undefined variable: '$%s'", ZSTR_VAL( member ) );
      goto php_perl_read_property_cleanup;
    }
  }
  /* Normal processing... interpret the variable */
  else {
    TRACE_MSG( "rp3" );
    sv = php_perl_deref( obj->sv );
    if( SvTYPE( sv ) == SVt_PVHV ) {
      HV   *hv = (HV *)sv;
      SV * *prop_val;

      prop_val = hv_fetch( hv, ZSTR_VAL( member ), ZSTR_LEN( member ), write );
      if( prop_val != NULL ) {
        if( write && !sv_isobject( *prop_val ) )
          retval = php_perl_create_new_object( rv, *prop_val, PERL_PROXY );
        else
          retval = php_perl_sv_to_zval( *prop_val, rv );
      }
      goto php_perl_read_property_cleanup;
    }
    else {
      zend_error( E_WARNING, "[perl] Object is not a hash" );
      goto php_perl_read_property_cleanup;
    }
  }

  if( sv != NULL ) {
    TRACE_MSG( "rp6" );
    retval = php_perl_sv_to_zval( sv, rv );
  }

php_perl_read_property_cleanup:
  php_perl_release_member_string(member);

  if( retval == NULL )
    return &EG( uninitialized_zval );
  else
    return retval;
} /* php_perl_read_property */

/* Sets property of hash based Perl's object */
#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_7_4_X)
static zval *
php_perl_write_property( php_perl_zop object, php_perl_mp member_val, zval *value, void * *cache_slot )
#else
static void
php_perl_write_property( php_perl_zop object, php_perl_mp member_val, zval *value, void * *cache_slot )
#endif
{
  TRACE_SUB( "php_perl_write_property" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;
  zend_string             *member    = php_perl_get_member_string(member_val);
  zval                    *result    = NULL;

  if( sv == NULL ) {
    if( obj->context == PERL_ARRAY ) {
      AV *av = get_av( ZSTR_VAL( member ), TRUE );
      if( Z_TYPE_P( value ) == IS_ARRAY ) {
        HashTable *ht = Z_ARRVAL_P( value );
        HashTable  var_hash;

        zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
        for( zend_hash_internal_pointer_reset( ht ); zend_hash_get_current_data( ht ) != NULL; zend_hash_move_forward( ht ) ) {
          zend_string *key;
          zend_ulong   index;

          if( zend_hash_get_current_key( ht, &key, &index ) != HASH_KEY_IS_STRING ) {
            result = zend_hash_get_current_data( ht );
            av_store( av, index, php_perl_zval_to_sv_ref( result, &var_hash ) );
          }
        }
        zend_hash_destroy( &var_hash );
      }
      else {
        zend_error( E_NOTICE, "[perl] array required" );
      }
    }
    else if( obj->context == PERL_HASH ) {
      HV *hv = get_hv( ZSTR_VAL( member ), TRUE );
      if( Z_TYPE_P( value ) == IS_ARRAY ) {
        HashTable *ht = Z_ARRVAL_P( value );
        HashTable  var_hash;

        zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
        for( zend_hash_internal_pointer_reset( ht ); zend_hash_get_current_data( ht ) != NULL; zend_hash_move_forward( ht ) ) {
          zend_string *key;
          zend_ulong   index;

          if( zend_hash_get_current_key( ht, &key, &index ) != HASH_KEY_IS_STRING ) {
            char xkey[ZEND_LTOA_BUF_LEN];
            zend_sprintf( xkey, ZEND_ULONG_FMT, index );
            result = zend_hash_get_current_data( ht );
            hv_store( hv, xkey, strlen( xkey ), php_perl_zval_to_sv_ref( result, &var_hash ), 0 );
          }
          else {
            result = zend_hash_get_current_data( ht );
            hv_store( hv, ZSTR_VAL( key ), ZSTR_LEN( key ), php_perl_zval_to_sv_ref( result, &var_hash ), 0 );
          }
        }
        zend_hash_destroy( &var_hash );
      }
      else {
        zend_error( E_NOTICE, "[perl] array required" );
      }
    }
    else {
      SV *sv = get_sv( ZSTR_VAL( member ), TRUE );
      result = value;
      sv_setsv( sv, php_perl_zval_to_sv( result ) );
    }
  }
  else {
    sv = php_perl_deref( sv );
    if( SvTYPE( sv ) == SVt_PVHV ) {
      HV *hv = (HV *)sv;
      result = value;
      hv_store( hv, ZSTR_VAL( member ), ZSTR_LEN( member ), php_perl_zval_to_sv( result ), 0 );
    }
    else {
      zend_error( E_WARNING, "[perl] Object is not a hash" );
    }
  }

  php_perl_release_member_string(member);
#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_7_4_X)
  return result;
#endif
} /* php_perl_write_property */

/* Returns true or false based on whether a zval is a null */
static inline int
zval_is_null(zval *op)
{
again:
  switch( Z_TYPE_P(op) ) {
    case IS_NULL:
    case IS_UNDEF:
      return 1;
    case IS_REFERENCE:
      op = Z_REFVAL_P(op);
      goto again;
    default:
      return 0;
  }
} /* zval_is_null */

/* Checks if property of hash based Perl's object isset or empty */
static int
php_perl_has_property( php_perl_zop object, php_perl_mp member_val, int has_set_exists, void * *cache_slot )
{
  TRACE_SUB( "php_perl_has_property" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;
  int                      ret       = 0;
  zend_string             *member    = php_perl_get_member_string(member_val);

  TRACE_MSG2( "  has_property '%s'", ZSTR_VAL( member ) );
  TRACE_MSG2( "  has_property - has_set_exists=%d", (int)has_set_exists );
  TRACE_MSG2( "  has_property - " ZEND_ADDR_FMT " object", (zend_ulong)object );
  TRACE_MSG2( "  has_property - " ZEND_ADDR_FMT " sv", (zend_ulong)sv );

  if( sv == NULL ) {
    if( obj->context == PERL_ARRAY ) {
      sv = (SV *)get_av( ZSTR_VAL( member ), FALSE );
      if( sv && !AvARRAY( sv ) )
        sv = NULL;
    }
    else if( obj->context == PERL_HASH ) {
      sv = (SV *)get_hv( ZSTR_VAL( member ), FALSE );
      if( sv && !HvARRAY( sv ) )
        sv = NULL;
    }
    else {
      sv = get_sv( ZSTR_VAL( member ), FALSE );
      if( sv && !SvOK( sv ) )
        sv = NULL;
    }

    /* property_exists() - true if sv != NULL */
    if( has_set_exists == 2 && sv != NULL )
      ret = 1;
  }
  else {
    sv = php_perl_deref( sv );
    if( SvTYPE( sv ) == SVt_PVHV ) {
      HV *hv = (HV *)sv;

      if( has_set_exists < 2 ) {
        SV * *prop_val = hv_fetch( hv, ZSTR_VAL( member ), ZSTR_LEN( member ), 0 );

        sv = (prop_val == NULL) ? NULL : *prop_val;
      }

      /* property_exists() - true if member exists */
      if( has_set_exists == 2 && hv_exists( hv, ZSTR_VAL( member ), ZSTR_LEN( member ) ) )
        ret = 1;
    }
    else {
      zend_error( E_WARNING, "[perl] Object is not a hash" );
      sv = NULL;
    }
  }

  /* If we want isset() or has(), then sv has the element to examine */
  if( sv != NULL && has_set_exists < 2 ) {
    zval zv;
    ZVAL_UNDEF( &zv );
    php_perl_sv_to_zval( sv, &zv );
    /* (isset) whether property exists and is true */
    if( has_set_exists == 1 )
      ret = zval_is_true( &zv );
    /* (has) whether property exists and is not NULL */
    else
      ret = !zval_is_null( &zv );
    zval_ptr_dtor( &zv );
  }

  php_perl_release_member_string(member);
  return ret;
} /* php_perl_has_property */

/* Deletes property of hash based Perl's object */
static void
php_perl_unset_property( php_perl_zop object, php_perl_mp member_val, void * *cache_slot )
{
  TRACE_SUB( "php_perl_unset_property" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;
  zend_string             *member    = php_perl_get_member_string(member_val);

  if( sv == NULL ) {
    if( obj->context == PERL_ARRAY ) {
      AV *av = get_av( ZSTR_VAL( member ), FALSE );
      av_undef( av );
    }
    else if( obj->context == PERL_HASH ) {
      HV *hv = get_hv( ZSTR_VAL( member ), FALSE );
      hv_undef( hv );
    }
    else {
      SV *sv = get_sv( ZSTR_VAL( member ), FALSE );
      sv_setsv( sv, &PL_sv_undef );
    }
  }
  else {
    sv = php_perl_deref( sv );
    if( SvTYPE( sv ) == SVt_PVHV ) {
      HV *hv = (HV *)sv;
      hv_delete( hv, ZSTR_VAL( member ), ZSTR_LEN( member ), G_DISCARD );
    }
    else {
      zend_error( E_WARNING, "[perl] Object is not a hash" );
    }
  }
  php_perl_release_member_string(member);
} /* php_perl_unset_property */

#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_8_0_X)
static int
php_perl_do_operation( zend_uchar opcode, zval *result, zval *op1, zval *op2 )
{
  TRACE_SUB( "php_perl_do_operation" );

  TRACE_MSG2( "  opcode = %d", (int)opcode );
  /* Is is an increment (++) or decrement (--) */
  if( (opcode == ZEND_ADD || opcode == ZEND_SUB) &&
      result == op1 &&
      Z_TYPE_P(op2) == IS_LONG && Z_LVAL_P(op2) == 1 ) {
    php_perl_object *obj = php_perl_from_zend( Z_OBJ_P( result ) );
    SV              *sv  = obj->sv;

    if( sv == NULL ) {
      zend_error( E_ERROR, "[perl] Cannot access value" );
      return FAILURE;
    }
    else if( SvREADONLY(sv) ) {
      zend_error( E_ERROR, "[perl] Cannot increment/decrement read-only value" );
      return FAILURE;
    }
    else if( SvTYPE(sv) >= SVt_PVAV || (isGV_with_GP(sv) && !SvFAKE(sv)) ) {
      zend_error( E_ERROR, "[perl] Cannot increment/decrement non-scalar" );
      return FAILURE;
    }

    if( opcode == ZEND_ADD )
      sv_inc( sv );
    else
      sv_dec( sv );
    return SUCCESS;
  }

  return FAILURE;
} /* php_perl_do_operation */
#endif

/* Constructs Perl object by calling constructor */
static void
php_perl_constructor_handler( INTERNAL_FUNCTION_PARAMETERS )
{
  TRACE_SUB( "php_perl_constructor_handler" );

  char  *perl_class_name;
  size_t perl_class_name_len;
  char  *constructor     = "new";
  size_t constructor_len = 3;
  int    argc            = EX_NUM_ARGS();
  zval  *this_ptr        = &EX( This );

  /* <this_ptr> should be a freshly initialized php_perl object */
  TRACE_ASSERT( php_perl_is_our_zval( this_ptr ) );
  TRACE_ASSERT( php_perl_from_zend( Z_OBJ_P( this_ptr ) )->sv == NULL );
  TRACE_ASSERT( php_perl_from_zend( Z_OBJ_P( this_ptr ) )->properties == NULL );
  TRACE_ASSERT( php_perl_from_zend( Z_OBJ_P( this_ptr ) )->remembered == FALSE );
  TRACE_ASSERT( php_perl_from_zend( Z_OBJ_P( this_ptr ) )->context == PERL_SCALAR );
  TRACE_ASSERT( php_perl_from_zend( Z_OBJ_P( this_ptr ) )->kind == PERL_NORMAL );
  TRACE_ASSERT( Z_OBJ_HT_P( this_ptr ) == &php_perl_object_handlers );

  TRACE_MSG2( "  argc = %d", (int)argc );
  if( argc == 0 ) {
    /* No arguments ... this is a call to new Perl() */
  }
  else if( zend_parse_parameters( ( argc > 2 ? 2 : argc ), "s|s",
                                  &perl_class_name, &perl_class_name_len,
                                  &constructor, &constructor_len ) != FAILURE ) {
    zval *argv = NULL;
    SV   *sv;

    if( argc > 2 ) {
      argv = (zval *)safe_emalloc( sizeof( zval ), argc, 0 );
      if( zend_get_parameters_array_ex( argc, argv ) == FAILURE ) {
        efree( argv );
        RETURN_FALSE;
      }
    }

    sv = php_perl_call_constructor( perl_class_name, perl_class_name_len,
                                    constructor, constructor_len,
                                    argc - 2, argv + 2 );

    if( argv != NULL ) {
      efree( argv );
      argv = NULL;
    }

    if( SvTRUE( ERRSV ) ) {
      STRLEN na;
      zend_throw_exception_ex( php_perl_exception_ce, 0, "[perl] constructor error: %s", SvPV( ERRSV, na ) );
      RETURN_FALSE;
    }

    /* Assign the perl expression to the this pointer */
    {
      php_perl_object *pobj = php_perl_from_zend( Z_OBJ_P( this_ptr ) );

      pobj->sv = sv;
      ; /* Incremented in call (do nothing) SvREFCNT_inc( pobj->sv ); */
    }

    /* Remember it */
    php_perl_remember_object( Z_OBJ_P( this_ptr ) );
  }
  RETURN_TRUE;
} /* php_perl_constructor_handler */

/* get_constructor handler for overloaded Perl objects */
static zend_function *
php_perl_get_constructor( zend_object *object )
{
  TRACE_SUB( "php_perl_get_constructor" );

  zend_function *f = zend_get_std_object_handlers()->get_constructor( object );
  if( f )
    return f;

  f                            = ecalloc( 1, sizeof( zend_internal_function ) );
  f->type                      = ZEND_INTERNAL_FUNCTION;
  f->common.num_args           = 2;
  f->common.required_num_args  = 0;
  f->common.function_name      = zend_string_init( "Perl", sizeof( "Perl" ) - 1, 1 );
  f->internal_function.handler = php_perl_constructor_handler;

  return f;
} /* php_perl_get_constructor */

/* Calls method of overloaded Perl's object */
static int
php_perl_call_function( zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS )
{
  TRACE_SUB( "php_perl_call_function" );

  int                      argc      = EX_NUM_ARGS();
  php_perl_object         *obj       = php_perl_from_zend( object );
  zval                    *argv      = NULL;
  const char              *func      = ZSTR_VAL( method );

  if( argc > 0 ) {
    argv = (zval *)safe_emalloc( sizeof( zval ), argc, 0 );
    if( zend_get_parameters_array_ex( argc, argv ) == FAILURE )
      return FAILURE;
  }

  if( obj->sv == NULL )
    php_perl_call( func, argc, argv, obj->context, return_value );
  else
    php_perl_call_method( obj->sv, func, argc, argv, obj->context, return_value );

  if( argv != NULL ) {
    efree( argv );
    argv = NULL;
  }

  if( SvTRUE( ERRSV ) ) {
    STRLEN na;
    zend_throw_exception_ex( php_perl_exception_ce, 0, "[perl] call error: %s", SvPV( ERRSV, na ) );
    return FAILURE;
  }

  return SUCCESS;
} /* php_perl_call_function */

static
ZEND_FUNCTION(php_perl_method_handler)
{
  TRACE_SUB( "php_perl_method_handler" );

  /* Pass to old call function */
  php_perl_call_function( EX(func)->common.function_name, Z_OBJ(EX(This)), INTERNAL_FUNCTION_PARAM_PASSTHRU );

  /* Cleanup trampoline */
  ZEND_ASSERT(EX(func)->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE);
  zend_string_release(EX(func)->common.function_name);
  zend_free_trampoline(EX(func));
  EX(func) = NULL;
} /* php_perl_method_handler */

/* Get method handler for overloaded Perl objects */
static zend_function *
php_perl_get_method( zend_object * *object_ptr, zend_string *method, const zval *key )
{
  TRACE_SUB( "php_perl_get_method" );

  php_perl_object *obj = php_perl_from_zend( *object_ptr );

  if( obj->sv == NULL ) {
    zend_function *f = zend_get_std_object_handlers()->get_method( object_ptr, method, key );
    if( f )
      return f;
  }

  /* Normal call - pass function name and any arguments to handler, along with This pointer (i.e. not static) */
  {
    zend_internal_function *f;

    if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
      f = (zend_internal_function *) &EG(trampoline);
    } else {
      f = emalloc(sizeof(zend_internal_function));
    }
    memset(f, 0, sizeof(zend_internal_function));

    f->type          = ZEND_INTERNAL_FUNCTION;
    f->function_name = zend_string_copy(method);
    f->scope         = (*object_ptr)->ce;
    f->fn_flags      = ZEND_ACC_CALL_VIA_HANDLER;
    f->handler       = ZEND_FN(php_perl_method_handler);
    return (zend_function *)f;
  }
} /* php_perl_get_method */

/* Returns all properties of Perl's object */
static HashTable *
php_perl_get_properties( php_perl_zop object )
{
  TRACE_SUB( "php_perl_get_properties" );

  php_perl_object         *obj       = php_perl_from_zop( object );
  SV                      *sv        = obj->sv;
  HashTable               *ht;

  /* Default Perl() object (no properties) */
  if( sv == NULL )
    return NULL;

  /* Create the hash table */
  if( obj->properties != NULL ) {
    ht = obj->properties;
    /* rebuild existing HashTable for properties */
    zend_hash_clean( ht );
  }
  else {
    ALLOC_HASHTABLE( obj->properties );
    ht = obj->properties;
    zend_hash_init( ht, 0, NULL, ZVAL_PTR_DTOR, 0 );
  }

  sv = php_perl_deref( sv );
  if( SvTYPE( sv ) == SVt_PVHV ) {
    HV       *hv = (HV *)sv;
    SV       *el_sv;
    char     *key;
    I32       key_len;
    HashTable var_hash;

    zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
    hv_iterinit( hv );
    while( ( el_sv = hv_iternextsv( hv, &key, &key_len ) ) != NULL ) {
      php_perl_sv_to_zval_ref( el_sv, php_perl_hash_str_get_zval( ht, key, key_len ), &var_hash );
    }
    zend_hash_destroy( &var_hash );
  }
  else if( SvTYPE( sv ) == SVt_PVAV ) {
    AV       *av  = (AV *)sv;
    I32       len = av_len( av );
    I32       i;
    HashTable var_hash;

    zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
    for( i = 0; i <= len; i++ ) {
      SV * *el_sv = av_fetch( av, i, 0 );
      if( el_sv != NULL && *el_sv != NULL )
        php_perl_sv_to_zval_ref( *el_sv, php_perl_hash_index_get_zval( ht, (zend_ulong)i ), &var_hash );
    }
    zend_hash_destroy( &var_hash );
  }

  return ht;
} /* php_perl_get_properties */

/* Returns class name of overloaded Perl's object */
static zend_string *
php_perl_get_class_name( const zend_object *object )
{
  TRACE_SUB( "php_perl_get_class_name" );

  const php_perl_object *pobj      = php_perl_from_zend( (zend_object *)object );
  SV                    *sv        = pobj->sv;
  const char            *name      = NULL;
  char                  *allocated = NULL;

  TRACE_MSG2( "get_class_name " ZEND_ADDR_FMT " (perl)", (zend_ulong)pobj );
  TRACE_MSG2( "get_class_name " ZEND_ADDR_FMT " (zend)", (zend_ulong)object );

  if( sv == NULL ) {
    if( pobj->kind == PERL_SPECIAL ) {
      switch( pobj->context ) {
        case PERL_ARRAY:
          name = "Perl::<array operator>";
          break;
        case PERL_HASH:
          name = "Perl::<hash operator>";
          break;
        case PERL_SCALAR:
        default:
          name = "Perl::<scalar operator>";
          break;
      }
    }
    else 
      name = "Perl";
  }
  else {
    HV *stash;
    if( SvROK(sv) &&
        SvTYPE(SvRV(sv)) >= SVt_PVMG &&
        ( stash = SvSTASH( SvRV(sv) ) ) != NULL ) {
      const char *tmp     = HvNAME( stash );
      const int   tmp_len = strlen( tmp );

      name = allocated = emalloc( tmp_len + sizeof( "Perl::" ) );
      strcpy( allocated, "Perl::" );
      strcpy( allocated + sizeof( "Perl::" ) - 1, tmp );
    }
  }

  if( name == NULL ) { 
    if( SvTYPE( sv ) == SVt_PVAV )
      name = "Perl::array";
    else if( SvTYPE( sv ) == SVt_PVHV )
      name = "Perl::hash";
    else
      name = "Perl::scalar";
  }

  zend_string *ret = zend_string_init( name, strlen( name ), 0 );
  if( allocated )
    efree( allocated );
  return ret;
} /* php_perl_get_class_name */

/* Remove an object from the system */
static void
php_perl_dtor_obj( zend_object *object )
{
  TRACE_SUB( "php_perl_dtor_obj" );

  php_perl_object         *obj       = php_perl_from_zend( object );

  TRACE_ASSERT( object != NULL );
  TRACE_MSG2( "dtor " ZEND_ADDR_FMT " (zend)", (zend_ulong)object );

  /* Call the userland destructor (if it exists) */
  zend_objects_destroy_object( object );

  /* Removing properties */
  if( obj->properties ) {
    TRACE_MSG2( "free " ZEND_ADDR_FMT " properties", (zend_ulong)obj );
    zend_hash_destroy( obj->properties );
    FREE_HASHTABLE( obj->properties );
    obj->properties = NULL;
  }

  /* Forget the object */
  if( obj->remembered )
    php_perl_forget_object( object );

  /* Remove perl object if not a special operator */
  if( obj->sv != NULL ) {
    TRACE_MSG2( "sv ref count is %d", (int)SvREFCNT( obj->sv ) );
    if( SvREFCNT( obj->sv ) >= 1 ) {
      TRACE_MSG2( "sv free ref count " ZEND_ADDR_FMT " sv", (zend_ulong)( obj->sv ) );
      SvREFCNT_dec( obj->sv );
    }
    obj->sv = NULL;
  }
} /* php_perl_dtor_obj */

/* Deallocate storage */
static void
php_perl_free_obj( zend_object *object )
{
  TRACE_SUB( "php_perl_free_obj" );

  TRACE_ASSERT( object != NULL );

  TRACE_MSG2( "free " ZEND_ADDR_FMT " (zend)", (zend_ulong)object );

  zend_object_std_dtor( object );
} /* php_perl_free_obj */

/* Makes a copy of overloaded perl object.
   It copies only properties or elements of object, but doesn't copy nested
   arrays, hashes or objects */
static zend_object *
php_perl_clone( php_perl_zop object )
{
  TRACE_SUB( "php_perl_clone" );

  php_perl_object         *old       = php_perl_from_zop( object );
  SV                      *new_sv    = NULL;

  if( old->kind == PERL_PROXY ) {
    zend_error( E_ERROR, "[perl] Can't clone perl proxy operator" );
    return NULL;
  }

  if( old->sv != NULL ) {
    SV *old_sv = SvRV( old->sv );

    if( SvTYPE( old_sv ) == SVt_PVAV ) {
      /* array */
      I32 len = av_len( (AV *)old_sv );
      I32 i;

      new_sv = (SV *)newAV();
      for( i = 0; i <= len; i++ ) {
        SV * *el_sv = av_fetch( (AV *)old_sv, i, 0 );
        if( el_sv != NULL && *el_sv != NULL )
          av_push( (AV *)new_sv, newSVsv( *el_sv ) );
      }
    }
    else if( SvTYPE( old_sv ) == SVt_PVHV ) {
      /* hash */
      new_sv = (SV *)newHVhv( (HV *)old_sv );
    }
    else if( SvOK( old_sv ) ) {
      /* scalar */
      new_sv = newSVsv( old_sv );
    }
    else {
      /* unknown */
      zend_error( E_ERROR, "[perl] Can't clone perl object (type [%ld])", (long)SvTYPE( old_sv ) );
      return NULL;
    }
    new_sv = sv_bless( newRV_noinc( new_sv ), SvSTASH( old_sv ) );
  }

  {
    php_perl_object *pobj   = (php_perl_object *)zend_object_alloc( sizeof( php_perl_object ), php_perl_zop_to_ce( object ) );
    zend_object     *new_zo = php_perl_to_zend( pobj );

    zend_object_std_init( new_zo, php_perl_zop_to_ce( object ) );

    /* Normal initialization of object */
    pobj->sv            = new_sv;
    pobj->properties    = NULL;
    pobj->context       = old->context;
    pobj->kind          = old->kind;
    pobj->remembered    = FALSE;
    pobj->remembered_sv = NULL;

    SvREFCNT_inc( pobj->sv );

    new_zo->handlers    = &php_perl_object_handlers;

    php_perl_remember_object( new_zo );
    return new_zo;
  }
} /* php_perl_clone */

/* Creates overloaded Perl's object */
static zend_object *
php_perl_create_object( zend_class_entry *class_type )
{
  TRACE_SUB( "php_perl_create_object" );

  php_perl_object *pobj   = (php_perl_object *)zend_object_alloc( sizeof( php_perl_object ), class_type );
  zend_object     *new_zo = php_perl_to_zend( pobj );

  /* Always initialize perl first */
  (void)php_perl_init();

  TRACE_MSG2( "create " ZEND_ADDR_FMT " (perl)", (zend_ulong)pobj );
  TRACE_MSG2( "create " ZEND_ADDR_FMT " (zend)", (zend_ulong)new_zo );

  zend_object_std_init( new_zo, class_type );
  object_properties_init( new_zo, class_type );

  /* Normal initialization of object */
  pobj->sv            = NULL;
  pobj->context       = PERL_SCALAR;
  pobj->properties    = NULL;
  pobj->kind          = PERL_NORMAL;
  pobj->remembered    = FALSE;
  pobj->remembered_sv = NULL;

  new_zo->handlers    = &php_perl_object_handlers;

  return new_zo;
} /* php_perl_create_object */

/****************************************************************************/

static void
php_perl_iterator_dtor( zend_object_iterator *iterator )
{
  TRACE_SUB( "php_perl_iterator_dtor" );

  zval_ptr_dtor(&iterator->data);
} /* php_perl_iterator_dtor */

static int
php_perl_iterator_valid( zend_object_iterator *iterator )
{
  TRACE_SUB( "php_perl_iterator_valid" );

  php_perl_object *obj = php_perl_from_zend( Z_OBJ( iterator->data ) );

  return ( obj->properties != NULL &&
           zend_hash_get_current_data( obj->properties ) != NULL ) ? SUCCESS : FAILURE;
} /* php_perl_iterator_valid */

static zval *
php_perl_iterator_current_data( zend_object_iterator *iterator )
{
  TRACE_SUB( "php_perl_iterator_current_data" );

  php_perl_object *obj = php_perl_from_zend( Z_OBJ( iterator->data ) );

  return obj->properties ? zend_hash_get_current_data( obj->properties ) : NULL;
} /* php_perl_iterator_current_data */

static void
php_perl_iterator_current_key( zend_object_iterator *iterator, zval *key )
{
  TRACE_SUB( "php_perl_iterator_current_key" );

  php_perl_object *obj = php_perl_from_zend( Z_OBJ( iterator->data ) );

  if( obj->properties )
    zend_hash_get_current_key_zval( obj->properties, key );
} /* php_perl_iterator_current_key */

static void
php_perl_iterator_move_forward( zend_object_iterator *iterator )
{
  TRACE_SUB( "php_perl_iterator_move_forward" );

  php_perl_object *obj = php_perl_from_zend( Z_OBJ( iterator->data ) );

  if( obj->properties )
    zend_hash_move_forward( obj->properties );
} /* php_perl_iterator_move_forward */

static void
php_perl_iterator_rewind( zend_object_iterator *iterator )
{
  TRACE_SUB( "php_perl_iterator_rewind" );

  php_perl_object *obj = php_perl_from_zend( Z_OBJ( iterator->data ) );

  if( obj->properties ) {
    /* removing properties */
    zend_hash_destroy( obj->properties );
    FREE_HASHTABLE( obj->properties );
    obj->properties = NULL;
  }

#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_8_0_X)
  php_perl_get_properties( Z_OBJ(iterator->data) );
#else
  php_perl_get_properties( &iterator->data );
#endif

  if( obj->properties )
    zend_hash_internal_pointer_reset( obj->properties );
} /* php_perl_iterator_rewind */

static zend_object_iterator_funcs php_perl_iterator_funcs = {
  php_perl_iterator_dtor,
  php_perl_iterator_valid,
  php_perl_iterator_current_data,
  php_perl_iterator_current_key,
  php_perl_iterator_move_forward,
  php_perl_iterator_rewind,
  NULL
};

static zend_object_iterator *
php_perl_get_iterator( zend_class_entry *ce, zval *object, int by_ref )
{
  TRACE_SUB( "php_perl_get_iterator" );

  zend_object_iterator *iterator = emalloc( sizeof( zend_object_iterator ) );
  zend_iterator_init( iterator );

  Z_ADDREF_P( object );
  ZVAL_OBJ( &iterator->data, Z_OBJ_P(object) );
  iterator->funcs = &php_perl_iterator_funcs;

  return iterator;
} /* php_perl_get_iterator */

/****************************************************************************/

static
PHP_GINIT_FUNCTION( perl ) {
  TRACE_SUB( "PHP_GINIT_FUNCTION" );
#if defined(COMPILE_DL_PERL) && defined(ZTS)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  ZEND_SECURE_ZERO(perl_globals, sizeof(*perl_globals));
} /* PHP_GINIT_FUNCTION */

static
PHP_GSHUTDOWN_FUNCTION( perl ) {
  TRACE_SUB( "PHP_GSHUTDOWN_FUNCTION" );
  if( perl_globals->perl_sys_inited ) {
    perl_globals->perl_sys_inited = FALSE;
    PERL_SYS_TERM();
  }
} /* PHP_GSHUTDOWN_FUNCTION */

PHP_MINIT_FUNCTION( perl ){
  TRACE_SUB( "PHP_MINIT_FUNCTION" );

  zend_class_entry ce;

  /* Exception raised. */
  INIT_CLASS_ENTRY( ce, "PerlException", NULL );
  php_perl_exception_ce                         = zend_register_internal_class_ex( &ce, zend_exception_get_default() );
  php_perl_exception_ce->ce_flags              |= ZEND_ACC_FINAL;

  /* Class of perl objects. */
  INIT_CLASS_ENTRY( ce, "Perl", perl_methods );
  php_perl_ce                                   = zend_register_internal_class( &ce );
  php_perl_ce->create_object                    = php_perl_create_object;
  php_perl_ce->get_iterator                     = php_perl_get_iterator;

  /* Setup normal object handler functions. */
  memcpy( &php_perl_object_handlers, zend_get_std_object_handlers(), sizeof( zend_object_handlers ) );
  php_perl_object_handlers.offset               = offsetof( php_perl_object, std );
  php_perl_object_handlers.get_constructor      = php_perl_get_constructor;
  php_perl_object_handlers.dtor_obj             = php_perl_dtor_obj;
  php_perl_object_handlers.free_obj             = php_perl_free_obj;
  php_perl_object_handlers.clone_obj            = php_perl_clone;
  php_perl_object_handlers.read_property        = php_perl_read_property;
  php_perl_object_handlers.write_property       = php_perl_write_property;
  php_perl_object_handlers.read_dimension       = php_perl_read_dimension;
  php_perl_object_handlers.write_dimension      = php_perl_write_dimension;
  php_perl_object_handlers.get_property_ptr_ptr = php_perl_get_property_ptr_ptr;
  php_perl_object_handlers.has_property         = php_perl_has_property;
  php_perl_object_handlers.unset_property       = php_perl_unset_property;
  php_perl_object_handlers.has_dimension        = php_perl_has_dimension;
  php_perl_object_handlers.unset_dimension      = php_perl_unset_dimension;
  php_perl_object_handlers.get_properties       = php_perl_get_properties;
  php_perl_object_handlers.get_method           = php_perl_get_method;
  php_perl_object_handlers.get_class_name       = php_perl_get_class_name;
#if (ZEND_EXTENSION_API_NO >= ZEND_EXTENSION_API_NO_8_0_X)
  php_perl_object_handlers.do_operation         = php_perl_do_operation;
#endif

  /* Setup proxy handlers. Copied from normal class above and adjusted */
  memcpy( &php_perl_proxy_handlers, &php_perl_object_handlers, sizeof( zend_object_handlers ) );
#if (ZEND_EXTENSION_API_NO < ZEND_EXTENSION_API_NO_8_0_X)
  php_perl_proxy_handlers.get                   = php_perl_get;
  php_perl_proxy_handlers.set                   = php_perl_set;
#else
  0; /* FIXME: fix proxy handling in PHP 8.x */
#endif

  return SUCCESS;
} /* PHP_MINIT_FUNCTION */

PHP_MSHUTDOWN_FUNCTION( perl ){
  TRACE_SUB( "PHP_MSHUTDOWN_FUNCTION" );

  return SUCCESS;
} /* PHP_MSHUTDOWN_FUNCTION */

PHP_RSHUTDOWN_FUNCTION( perl ){
  TRACE_SUB( "PHP_RSHUTDOWN_FUNCTION" );

  php_perl_destroy();
  return SUCCESS;
} /* PHP_RSHUTDOWN_FUNCTION */

PHP_MINFO_FUNCTION( perl ){
  TRACE_SUB( "PHP_MINFO_FUNCTION" );

  php_info_print_table_start();
  php_info_print_table_header( 2, "Perl support", "enabled" );
  php_info_print_table_row( 2, "Extension version", PHP_PERL_VERSION );
  {
    char rt_perl_version[64];
    snprintf( rt_perl_version, sizeof(rt_perl_version), "%d.%d.%d", PL_revision, PL_version, PL_subversion );
    php_info_print_table_row( 2, "Perl version", rt_perl_version );
  }
  php_info_print_table_row( 2, "Perl compile version", PERL_VERSION_STRING );
  php_info_print_table_end();
} /* PHP_MINFO_FUNCTION */

/* perl_require($perl_file_name)
   Loads and executes Perl file. Produces a error if file does not exist or
   has Perl's errors */
PHP_METHOD( Perl, require ){
  TRACE_SUB( "PHP_METHOD Perl::require" );

  char  *perl_filename;
  size_t perl_filename_len;

  if( zend_parse_parameters( EX_NUM_ARGS(), "s", &perl_filename, &perl_filename_len ) != FAILURE ) {
    TRACE_MSG2( "  requiring '%s'", perl_filename );

    (void) php_perl_init();

    require_pv( perl_filename );
    if( SvTRUE( ERRSV ) ) {
      STRLEN na;
      zend_throw_exception_ex( php_perl_exception_ce, 0, "[perl] require error: %s", SvPV( ERRSV, na ) );
    }
  }
  TRACE_MSG( "Perl::require done" );
} /* Perl::require */

/* perl_eval($perl_code)
   Evaluates Perl code and returns result.
   Produces a error if code has Perl's errors */
PHP_METHOD( Perl, eval ){
  TRACE_SUB( "PHP_METHOD: Perl::eval" );

  char                    *perl_code;
  size_t                   perl_code_len;

  if( zend_parse_parameters( EX_NUM_ARGS(), "s", &perl_code, &perl_code_len ) != FAILURE ) {
    SV *sv;
    (void) php_perl_init();

    TRACE_MSG2( "  evaluating '%s'", perl_code );

    dSP;
    sv = newSVpv( perl_code, perl_code_len );
    if( USED_RET() ) {
      zval            *this_ptr = &EX( This );
      php_perl_object *obj      = php_perl_from_zend( Z_OBJ_P( this_ptr ) );

      TRACE_MSG2( "this_ptr is perl = %d", (int)php_perl_is_our_zval( this_ptr ) );
      TRACE_MSG2( "this_ptr context = %d", (int)obj->context );
      TRACE_MSG2( "return value is perl = %d", (int)php_perl_is_our_zval( return_value ) );

      if( obj->context != PERL_SCALAR ) {
        int       count, i;
        I32       ax;
        HashTable var_hash;

        count = eval_sv( sv, G_ARRAY );
        SPAGAIN;
        sp   -= count;
        ax    = ( sp - PL_stack_base ) + 1;
        zend_hash_init( &var_hash, 0, NULL, NULL, 0 );
        array_init_size( return_value, count );
        if( obj->context == PERL_ARRAY ) {
          for( i = 0; i < count; i++ ) {
            php_perl_sv_to_zval_ref( (SV *)ST( i ), php_perl_array_get_zval( return_value, i ), &var_hash );
          }
        }
        else {
          for( i = 0; i < count; i++ ) {
            STRLEN key_len;
            char  *key = SvPV( ST( i ), key_len );
            if( i != count - 1 )
              php_perl_sv_to_zval_ref( (SV *)ST( ++i ), php_perl_hash_get_zval( return_value, key, key_len ), &var_hash );
            else
              add_assoc_null_ex( return_value, key, key_len );
          }
        }
        zend_hash_destroy( &var_hash );
      }
      else {
        eval_sv( sv, G_SCALAR );
        SPAGAIN;
        TRACE_MSG2( "scalar (no discard) sv = 0x%lx", (zend_ulong)sv );
        php_perl_sv_to_zval( POPs, return_value );
        TRACE_MSG( "back" );
      }
      PUTBACK;
    }
    else {
      TRACE_MSG2( "scalar (discard) sv = 0x%lx", (zend_ulong)sv );
      eval_sv( sv, G_DISCARD );
      TRACE_MSG( "back" );
    }
    SvREFCNT_dec( sv );

    if( SvTRUE( ERRSV ) ) {
      STRLEN na;
      zend_throw_exception_ex( php_perl_exception_ce, 0, "[perl] eval error: %s", SvPV( ERRSV, na ) );
    }
  }
  TRACE_MSG( "Perl::eval done" );
} /* Perl::eval */

#ifdef TEST_START_STOP
PHP_FUNCTION( perl_start ){
  TRACE_SUB( "PHP_FUNCTION: perl_start" )
  (void) php_perl_init();
} /* perl_start */

PHP_FUNCTION( perl_stop ){
  TRACE_SUB( "PHP_FUNCTION: perl_stop" )
  (void) php_perl_destroy();
} /* perl_stop */
#endif

zend_module_entry perl_module_entry = {
  STANDARD_MODULE_HEADER,
  "perl",
#ifdef TEST_START_STOP
  perl_static_functions,
#else
  NULL,
#endif
  PHP_MINIT( perl ),
  PHP_MSHUTDOWN( perl ),
  NULL,
  PHP_RSHUTDOWN( perl ),
  PHP_MINFO( perl ),
  PHP_PERL_VERSION,
  PHP_MODULE_GLOBALS(perl),
  PHP_GINIT(perl),
  PHP_GSHUTDOWN(perl),
  NULL,
  STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_PERL
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE( perl )
#endif

#endif /* if HAVE_PERL */
