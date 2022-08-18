package config;
use strict;
use warnings;

our @EXPORT = qw( general appearance );
use Exporter 'import';

use template_helpers 'to_template';

sub general
{
    my ($ssn, $data) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        );

    to_template(\%vars, \$data->{'config_general.tt'});
}

sub appearance
{
    my ($ssn, $data) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        );

    to_template(\%vars, \$data->{'config_appearance.tt'});    
}

1;
