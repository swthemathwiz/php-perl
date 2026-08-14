# Makefile.frag fragment to create a source tarball

# File naming conventions for pie 
#
# php_{ExtensionName}-{Version}-src.tgz (e.g. php_myext-1.20.1-src.tgz)
#
ext_name = perl

# Pull the version from php_perl.h
ext_version := $(shell sed -n 's/^#define PHP_PERL_VERSION "\([^"]*\)".*/\1/p' $(top_srcdir)/php_perl.h)
ifeq ($(ext_version),)
$(error Cannot extract PHP_PERL_VERSION from $(top_srcdir)/php_perl.h)
endif

ext_file_prefix = php_$(ext_name)-$(ext_version)
tarball = $(ext_file_prefix)-src.tgz

dist:
	@echo "Creating source distribution tarball..."
	git archive --format=tar.gz --prefix=$(ext_file_prefix)/ HEAD > $(tarball)
	@echo "Tarball created: $(tarball)"

# Remove generated build/test artifacts on clean
clean: clean-artifacts

.PHONY: clean-artifacts

clean-artifacts:
	rm -f config*~ tmp-php.ini php_test_results_*.txt
