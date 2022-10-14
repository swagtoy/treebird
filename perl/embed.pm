package embed;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_embedded_page );

use template_helpers 'to_template';

sub generate_embedded_page
{
    my ($ssn, $data, $content) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        content => $content,
        );
    
    to_template(\%vars, \$data->{'embed.tt'});

}

1;

