#!/usr/bin/perl -w

use strict;

eval {
    local $SIG{ALRM} = sub { die };

    alarm 1;

    open my $fh, "-|", "sleep", "3" or die;
    while (<$fh>)
    {
	print "fh $fh";
    }
    close $fh;

    alarm 0;
    return 1;
}
|| do
{
    die "marche pas\n";
};

print "marche\n";
