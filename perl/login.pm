package login;
use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw( content_login );

use l10n 'lang';
use template_helpers 'to_template';

sub content_login
{
    my ($ssn, $data, $error) = @_;

    my %vars = (
        error => $error,
        lang => \&lang,
        );
    
    to_template(\%vars, \$data->{'login.tt'});
}

1;
