package meta;
use strict;
use warnings;

our @EXPORT = qw(  );

use Exporter 'import';

use template_helpers qw( simple_page to_template );

sub about
{
    simple_page @_, 'about.tt';
}

sub license
{
    simple_page @_, 'license.tt';
}

sub memory
{
    my ($ssn, $data, $mem_alloc_est, $mem_alloc, $mem_free, $mem_time_alloc, $mem_time_freed) = @_;

    my %vars = (
        ssn => $ssn,
        mem_alloc_est => $mem_alloc_est,
        mem_alloc => $mem_alloc,
        mem_free => $mem_free,
        mem_time_alloc => $mem_time_alloc,
        mem_time_freed => $mem_time_freed,
        );
    
    to_template(\%vars, \$data->{'memory.tt'});
}

1;
