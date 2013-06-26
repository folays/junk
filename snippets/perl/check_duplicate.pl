#!/usr/bin/perl -w

use strict;
use warnings FATAL => qw(uninitialized);

sub check_duplicate
{
    (my $cmdline = qx(cat /proc/$$/cmdline)) =~ s/(?:\0(?!$))\0?/ /go;
    open my $fh, "-|", "pgrep", "-x", "-f", $cmdline or die;
    do { print "duplicate instance\n" if -t STDIN; exit } if scalar @{[<$fh>]} >= 2;
    close $fh;
}
&check_duplicate;

sleep 60;
