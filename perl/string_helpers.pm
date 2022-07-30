package string_helpers;
use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw( &reltime_to_str );

sub reltime_to_str
{
    my $since = time() - $_[0];

    return $since . 's' if $since < 60;
    return int($since / 60) . 'm' if $since < 60 * 60;
    return int($since / (60 * 60)) . 'h' if $since < 60 * 60 * 24;
    return int($since / (60 * 60 * 24)) . 'd' if $since < 60 * 60 * 24 * 31;
    return int($since / (60 * 60 * 24 * 31)) . 'mon' if $since < 60 * 60 * 24 * 365;
    return int($since / (60 * 60 * 24 * 365)) . 'yr';
}
