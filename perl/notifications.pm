package notifications;

use Exporter 'import';

our @EXPORT = qw( notification_compact );

use template_helpers 'to_template';

sub notification_compact
{
    my ($ssn, $data, $notif) = @_;

    my %vars = (
        prefix => $ssn,
        notif => $notif
        );

    to_template(\%vars, \$data->{'notif_compact.tt'});
}
