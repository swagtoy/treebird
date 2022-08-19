package config;
use strict;
use warnings;

our @EXPORT = qw( general appearance );
use Exporter 'import';

use template_helpers 'simple_page';

sub general
{
    simple_page @_, 'config_general.tt';
}

sub appearance
{
    simple_page @_, 'config_appearance.tt';
}

1;
