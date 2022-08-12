package status;
use strict;
use warnings;
use string_helpers qw( reltime_to_str greentextify emojify format_username localize_mentions simple_escape );
use icons qw( get_icon visibility_to_icon );
use attachments 'generate_attachment';
use postbox 'generate_postbox';
use emojis 'generate_emoji';
use Exporter 'import';
use l10n 'lang';

our @EXPORT = qw( content_status generate_status );

use template_helpers 'to_template';

sub generate_status
{
    my ($ssn, $data, $status, $notif) = @_;
    my $boost_acct;

    # Move status reference for boosts and keep account
    # I hate this design but blame MastoAPI, not me.
    if ($status->{reblog})
    {
        $boost_acct = $status->{account};
        $status = $status->{reblog};
    }

    my %vars = (
        prefix => '',
        ssn => $ssn,
        status => $status,
        boost => $boost_acct, # May be undef
        data => $data,
        notif => $notif, # May be undef
        is_statusey_notif => ($notif && ($notif->{type} eq 'mention' || $notif->{type} eq 'status')),
        # Functions
        icon => \&get_icon,
        lang => \&lang,
        rel_to_str => \&reltime_to_str,
        vis_to_icon => \&visibility_to_icon,
        make_att => \&generate_attachment,
        make_emoji => \&generate_emoji,
        greentextify => \&greentextify,
        emojify => \&emojify,
        escape => \&simple_escape,
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
