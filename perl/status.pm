package status;
use strict;
use warnings;
use HTML::Escape 'escape_html';
use string_helpers qw( reltime_to_str greentextify emojify format_username localize_mentions );
use icons qw( get_icon visibility_to_icon );
use attachments 'generate_attachment';
use postbox 'generate_postbox';
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
        data => $data,
        # Functions
        icon => \&get_icon,
        rel_to_str => \&reltime_to_str,
        vis_to_icon => \&visibility_to_icon,
        make_att => \&generate_attachment,
        make_emoji => \&generate_emoji,
        greentextify => \&greentextify,
        emojify => \&emojify,
        escape => \&escape_html,
        fix_mentions => \&localize_mentions,
        format_username => \&format_username,
        make_postbox => \&generate_postbox,
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
        );


    to_template(\%vars, \$data->{'content_status.tt'});
}
