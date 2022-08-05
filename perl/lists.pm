package lists;
use strict;
use warnings;
use Exporter 'import';

our @EXPORTS = qw( content_lists );

use template_helpers 'to_template';

sub content_lists
{
    my ($ssn, $data, $lists) = @_;

    my %vars = ();

    to_template(\%vars, \$data->{'content_lists.tt'});
}
