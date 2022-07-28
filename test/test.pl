#!/usr/bin/perl
use strict;
use warnings;

my $FAIL = "\033[91m";
my $OK = "\033[96m";
my $SUCCESS = "\033[92m";
my $ENDC = "\033[0m";

sub read_file
{
    my ($filename) = @_;
    open my $fh, '<', $filename or die "Error opening $filename: $!";

    do { local $/; <$fh> };
}

my @files_glob = glob('./test/unit/t*.bin');
my $fail = 0;

foreach my $bin (@files_glob)
{
    my $content = read_file(substr($bin, 0, -4) . ".c");
    my ($name, $valgrind) = $content =~ /\/\*.*INFO name='(.*?)'.*valgrind=(.*?)\s\*\//s;

    
    if (system(($bin)))
    {
        print "$FAIL -- !! -- Test '$name' failed!$ENDC\n";
        $fail = 1;
        last;
    }
    else {
        print "$OK -- OK -- Test '$name' passed$ENDC\n";
    }
}

print "$SUCCESS -- All Tests passed!$ENDC\n" unless $fail;
