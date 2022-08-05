package lists;
use strict;
use warnings;
use Exporter 'import';

our @EXPORTS = qw(  );

use template_helpers 'to_template';

sub content_lists
{
    my ($ssn, $data, $lists) = @_;

    to_template(\%vars, $data->{'content_lists.tt'});
}
