package status;

use Exporter 'import';

our @EXPORT = qw( status );

use template_helpers 'to_template';

sub status
{
    my ($ssn, $data, $status) = @_;

    my %vars = (
        prefix => $ssn,
        status => $status
        );

    to_template(\%vars, \$data->{'status.tt'});
}

sub content_status
{
    my ($ssn, $data, $statuses) = @_;

    to_template(\%name, \$data->{'content_status.tt'});
}
