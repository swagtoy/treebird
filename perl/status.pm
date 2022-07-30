package status;
use strict;
use warnings;
use string_helpers qw( reltime_to_str );
use icons qw( get_icon visibility_to_icon );
use attachments 'generate_attachment';
use emojis 'generate_emoji';
use Exporter 'import';

our @EXPORT = qw( content_status generate_status );

use template_helpers 'to_template';

sub generate_status
{
    my ($ssn, $data, $status) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        status => $status,
        icon => \&get_icon,
        rel_to_str => \&reltime_to_str,
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
        status => $status,
        statuses_before => $statuses_before,
        statuses_after => $statuses_after,
        # Functions
        create_status => \&generate_status,
        make_att => \&generate_attachment,
        make_emoji => \&generate_emoji,
        );


    to_template(\%vars, \$data->{'content_status.tt'});
}
