#!/bin/perl
BEGIN { push @INC, '.' }
use strict;
use warnings;
use helpers;

my ($url) = helpers::prompt_instance_info();
my $account = helpers::prompt('The Account', '?');
my $num = helpers::prompt('# of requests');

helpers::stress_test('Main account page', $url, $num, '@' . $account);
helpers::stress_test('Scrobbles', $url, $num, '@' . $account . '/scrobbles');
helpers::stress_test('Pinned statuses', $url, $num, '@' . $account . '/pinned');
helpers::stress_test('Media', $url, $num, '@' . $account . '/media');
helpers::stress_test('Following', $url, $num, '@' . $account . '/following');
helpers::stress_test('Followers', $url, $num, '@' . $account . '/followers');
helpers::stress_test('Statuses', $url, $num, '@' . $account . '/statuses');
