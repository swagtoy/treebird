package navigation;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_navigation );

use template_helpers 'to_template';
use l10n 'lang';

sub generate_navigation
{
    my ($ssn, $data, $first_id, $last_id) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        lang => \&lang,
        start_id => $ssn->{post}->{start_id} || $first_id,
        prev_id => $first_id,
        next_id => $last_id,
        );

    to_template(\%vars, \$data->{'navigation.tt'});
}
