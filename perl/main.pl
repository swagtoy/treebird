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
    my ($ssn, $data, $main) = @_;
    my $result;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        title => $L10N{'EN_US'}->{'APP_NAME'},
        lang => sub { $L10N{'EN_US'}->{shift(@_)} },
        main => $main,
        sidebar_opacity => $ssn->{config}->{sidebar_opacity} / 255,
        acct => $ssn->{account}
        );
    

    $template->process(\$data, \%vars, \$result) ||
        return $template->error();
    
    $result;
}

