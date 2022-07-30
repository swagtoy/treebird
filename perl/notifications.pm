package notifications;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_notification_compact );

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
