package account;
use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw( generate_attachment );

use template_helpers 'to_template';

sub account
{
    my ($ssn, $data, $acct, $content) = @_;

    my %vars = (
        prefix = '',
        ssn => $ssn,
        content => $content,
        );

    to_template(\%vars, \$data->{'account.tt'});
}

sub content_account_statuses
{
    my ($ssn, $data, $acct, $statuses) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        acct => $acct,
        );

    account($ssn, $data, $acct, to_template(\%vars, \$data->{'account_statuses.tt'}));
}
