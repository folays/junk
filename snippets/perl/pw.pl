#!/usr/bin/perl -w

use strict;
use warnings FATAL => qw(uninitialized);

use Digest::SHA qw(sha256_hex);

die unless scalar @ARGV;

my $pw = shift;

my $salt = join("", ("A".."Z", "a".."z", "0".."9")[map { rand 62 } 1 .. 8]);

$salt = pack("H*", "637241416243384c");
printf "salt : %s\n", $salt;


my $salt_hex_formatted = unpack("H*", $salt);

printf "salt_hex_formatted : %s\n", $salt_hex_formatted;

my $crypt = "{".($salt ? "S" : "")."SHA256.HEX}".sha256_hex($pw.$salt).$salt_hex_formatted;

print $crypt."\n";
