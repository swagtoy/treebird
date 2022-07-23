use strict;
use warnings;
# Modules
use Template;
use l10n qw( %L10N );

my $template = Template->new(
    {
        INTERPOLATE => 1,
        POST_CHOMP => 1,
        EVAL_PERL => 1,
        TRIM => 1
    });

sub base_page
{
    my ($ssn, $data) = @_;
    my $result;

    my %vars = (
        prefix => '',
        lang => sub { $L10N{'EN_US'}->{shift(@_)} },
    );

    $template->process(\$data, \%vars, \$result) ||
        return $template->error();
    
    $result;
}

