package notifications;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_notification_compact content_notifications );

use template_helpers 'to_template';

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
        ssn=> $ssn,
        notifs => $notifs
        );
    
    to_template(\%vars, \$data->{'content_notifs.tt'});
}
