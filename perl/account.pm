package account;
use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw( account content_statuses );

use template_helpers 'to_template';

sub generate_account
{
    my ($ssn, $data, $acct, $content) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        content => $content,
        acct => $acct,
        );

    to_template(\%vars, \$data->{'account.tt'});
}

sub content_statuses
{
    my ($ssn, $data, $acct, $relationships, $statuses) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        acct => $acct,
        );

    generate_account($ssn, $data, $acct, $relationships, to_template(\%vars, \$data->{'account_statuses.tt'}));
}
