package meta;
use strict;
use warnings;

our @EXPORT = qw(  );

use Exporter 'import';

use template_helpers 'simple_page';

sub about
{
    simple_page @_, 'about.tt';
}

sub license
{
    simple_page @_, 'license.tt';
}

1;
