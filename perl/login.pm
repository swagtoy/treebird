package login;
use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw( content_login );

use template_helpers 'to_template';

sub content_login
{
    my ($ssn, $data, $error) = @_;

    my %vars = (
        error => $error,
        );
    
    to_template(\%vars, \$data->{'login.tt'});
}

1;
