package notifications;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_notification_compact content_notifications );

use template_helpers 'to_template';
use status 'generate_status';
use string_helpers qw( random_error_kaomoji );

sub generate_notification_compact
{
    my ($ssn, $data, $notif) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        notif => $notif
        );

    to_template(\%vars, \$data->{'notif_compact.tt'});
}

sub content_notifications
{
    my ($ssn, $data, $notifs) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        notifs => $notifs,
        create_status => sub { generate_status($ssn, $data, shift, shift); },
        random_error_kaomoji => \&random_error_kaomoji,
        );
    
    to_template(\%vars, \$data->{'content_notifs.tt'});
}
