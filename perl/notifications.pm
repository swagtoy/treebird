package notifications;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_notification content_notifications );

use template_helpers 'to_template';
use status 'generate_status';
use string_helpers qw( random_error_kaomoji );
use icons 'get_icon';

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

return 1;
