package attachments;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_attachment );

use template_helpers 'to_template';

sub generate_attachment
{
    my ($ssn, $data, $att, $sensitive) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        attachment => $att,
        sensitive => $sensitive
        );

    to_template(\%vars, \$data->{'attachment.tt'});
}

