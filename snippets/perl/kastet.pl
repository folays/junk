#!/usr/bin/perl -w

use strict;
use warnings qw(uninitialized);

use Data::Dumper;

my @cube = map { 0 } (1 .. 5 ** 3);

my @piece = ([0, 0, 0],
	     [0, 1, 0],
	     [0, 2, 0],
	     [0, 3, 0],
	     [1, 1, 0]);

my @placed;

&try;

sub success()
{
    print "got solution\n";
    foreach (@placed)
    {
	printf "-> %d %d %d %d %d %d\n", @$_;
    }
    exit 0;
}

sub try()
{
    foreach my $pos_x (0 .. 4)
    {
	foreach my $pos_y (0 .. 4)
	{
	    foreach my $pos_z (0 .. 4)
	    {
		next if &test_pt($pos_x, $pos_y, $pos_z);
		&try_pos($pos_x, $pos_y, $pos_z);
	    }
	}
    }
}

sub try_pos($$$)
{
    my ($pos_x, $pos_y, $pos_z) = @_;

    foreach my $rot_x (0 .. 3)
    {
	foreach my $rot_y (0 .. 3)
	{
	    foreach my $rot_z (0 .. 3)
	    {
		&try_pos_rot($pos_x, $pos_y, $pos_z,
			     $rot_x, $rot_y, $rot_z);
	    }
	}
    }
}

sub try_pos_rot($$$$$$)
{
    my ($pos_x, $pos_y, $pos_z,
	$rot_x, $rot_y, $rot_z) = @_;

    my @old_cube = @cube;
    foreach my $pt (@piece)
    {
	my @new_pt = ($pos_x + $pt->[0], $pos_y + $pt->[1], $pos_z + $pt->[2]);
	&rot_pt(\@new_pt, $rot_x, $rot_y, $rot_z);
	&flag_pt(@new_pt) or goto fail;
    }
    printf "cube %d : ".join(" ", ("%d") x 5 ** 3)."\n", scalar @placed, @cube;
    push @placed, [$pos_x, $pos_y, $pos_z, $rot_x, $rot_y, $rot_z];
    if (scalar @placed == 25)
    {
	&success;
    }
    else
    {
	&try;
    }
    pop @placed;
  fail:
    @cube = @old_cube;
}

sub rot_pt($$$$)
{
    my ($pt, $rot_x, $rot_y, $rot_z) = @_;

    while ($rot_x--)
    {
	@$pt = ($pt->[1], -$pt->[0], $pt->[2]);
    }
    while ($rot_y--)
    {
	@$pt = ($pt->[2], $pt->[1], -$pt->[0]);
    }
    while ($rot_z--)
    {
	@$pt = ($pt->[0], -$pt->[2], $pt->[1]);
    }
}

sub ref_pt($$$)
{
    my ($pos_x, $pos_y, $pos_z) = @_;

    return \$cube[$pos_x + $pos_y * 5 + $pos_z * (5 ** 2)];
}

sub check_pt($$$)
{
    my ($pos_x, $pos_y, $pos_z) = @_;

    foreach (\$pos_x, \$pos_y, \$pos_z)
    {
	return unless $$_ >= 0 && $$_ <= 4;
    }
    return 1;
}

sub test_pt($$$)
{
    my ($pos_x, $pos_y, $pos_z) = @_;

    my $ref = &ref_pt($pos_x, $pos_y, $pos_z);
    return $$ref;
}

sub flag_pt($$$)
{
    my ($pos_x, $pos_y, $pos_z) = @_;

    return unless &check_pt;
    my $ref = &ref_pt($pos_x, $pos_y, $pos_z);
    return if $$ref;
    $$ref = 1;
}
