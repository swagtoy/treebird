package status;
use strict;
use warnings;
use icons qw( get_icon visibility_to_icon );
use Exporter 'import';

our @EXPORT = qw( status );

use template_helpers 'to_template';

sub status
{
    my ($ssn, $data, $status) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        status => $status,
        icon => \&get_icon,
        vis_to_icon => \&visibility_to_icon,
        );

    to_template(\%vars, \$data->{'status.tt'});
}

sub content_status
{
    my ($ssn, $data, $status, $statuses_before, $statuses_after) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        data => $data,
        create_status => \&status,
        status => $status,
        statuses_before => $statuses_before,
        statuses_after => $statuses_after,
        );


    to_template(\%vars, \$data->{'content_status.tt'});
}
