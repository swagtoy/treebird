package notifications;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_notification content_notifications embed_notifications );

use template_helpers 'to_template';
use status 'generate_status';
use string_helpers qw( random_error_kaomoji );
use icons 'get_icon';
use embed 'generate_embedded_page';
use navigation 'generate_navigation';

sub generate_notification
{
    my ($ssn, $data, $notif, $is_compact) = @_;

    $is_compact ||= 0;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        notif => $notif,
        compact => $is_compact,
        create_status => sub { generate_status($ssn, $data, shift, shift, $is_compact); },
        icon => \&get_icon,
        );

    to_template(\%vars, \$data->{'notification.tt'});
}

sub content_notifications
{
    my ($ssn, $data, $notifs) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        notifs => $notifs,
        notification => sub { generate_notification($ssn, $data, shift); },
        random_error_kaomoji => \&random_error_kaomoji,
        );
    
    to_template(\%vars, \$data->{'content_notifs.tt'});
}

sub embed_notifications
{
    my ($ssn, $data, $notifs) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        notifs => $notifs,
        notification => sub { generate_notification($ssn, $data, shift, 1); },
        nav => sub { generate_navigation($ssn, $data, $notifs->[0]->{id}, $notifs->[-1]->{id}) },
        );
    
    generate_embedded_page($ssn, $data, to_template(\%vars, \$data->{'notifs_embed.tt'}));
}

1;
