# Makefile.frag fragment for extra targets

.PHONY: clean-artifacts dist memory-test

# make dist:
#
# Make a distribution tarball (that conforms to pie standards)
#

# File naming conventions for pie 
#
# php_{ExtensionName}-v{Version}-src.tgz (e.g. php_myext-v1.20.1-src.tgz)
#
ext_name = perl

# Pull the version from php_perl.h
ext_version := $(shell sed -n 's/^#define PHP_PERL_VERSION "\([^"]*\)".*/\1/p' $(top_srcdir)/php_perl.h)
ifeq ($(ext_version),)
$(error Cannot extract PHP_PERL_VERSION from $(top_srcdir)/php_perl.h)
endif

ext_archive_prefix = php_$(ext_name)-v$(ext_version)
tarball = $(ext_archive_prefix)-src.tgz

dist:
	@echo "Creating source distribution tarball..."
	git archive --format=tar.gz --prefix=$(ext_archive_prefix)/ HEAD > $(tarball)
	@echo "Tarball created: $(tarball)"

# make memory-test:
#
# Run make test with valgrind memory checker
#

memory-test:
	TEST_PHP_ARGS='-m -q' $(MAKE) test TESTS="$(TESTS)"

# make clean-artifacts:
#
# Remove additional generated build/test artifacts on clean
#

clean: clean-artifacts

clean-artifacts:
	rm -f config*~ tmp-php.ini php_test_results_*.txt
