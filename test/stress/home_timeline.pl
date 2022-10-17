#!/bin/perl
BEGIN { push @INC, '.' }
use strict;
use warnings;
use helpers;

my ($url) = helpers::prompt_instance_info();
my $num = helpers::prompt('# of requests');

helpers::stress_test('Home timeline', $url, $num, '');
