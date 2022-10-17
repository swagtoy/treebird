#!/bin/perl
BEGIN { push @INC, '.' }
use strict;
use warnings;
use helpers;

my ($url) = helpers::prompt_instance_info();
my $num = helpers::prompt('# of requests per each page');

helpers::stress_test('About page', $url, $num, 'about');
helpers::stress_test('License page', $url, $num, 'about/license');
helpers::stress_test('Login page', $url, $num, 'login');
helpers::stress_test('Search page', $url, $num, 'search');
helpers::stress_test('Emoji picker page', $url, $num, 'emoji_picker');
helpers::stress_test('Config page', $url, $num, 'config');
helpers::stress_test('General page for Config', $url, $num, 'config/general');
helpers::stress_test('Appearance page for Config', $url, $num, 'config/appearance');

