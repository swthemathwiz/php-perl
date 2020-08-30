use strict;
use warnings;

use PadWalker qw/peek_our peek_my/;
use Data::Dumper;

our $foo = 1;
our $bar = 2;

#{
#    my $foo = 3;
#    print Dumper in_scope_variables();
#}

print STDERR Dumper in_scope_variables();

sub in_scope_variables {
    my %in_scope = %{peek_our(1)};
    my $lexical  = peek_my(1);
    for my $name (keys %main::) {
        my $glob = $main::{$name};
        if (defined ${$glob}) {
            $in_scope{'$' . $name} = ${$glob};
        }

        if (@{$glob}) {
            $in_scope{'@' . $name} = [@{$glob}];
        }

        if (%{$glob}) {
            $in_scope{'%' . $name} = {%{$glob}};
        }
    }

    #lexicals hide package variables
    while (my ($var, $ref) = each %$lexical) {
        $in_scope{$var} = $ref;
    }
    return \%in_scope;
}
