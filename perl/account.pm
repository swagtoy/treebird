package account;
use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw( account content_statuses );

use template_helpers 'to_template';
use l10n 'lang';
use status 'generate_status';

sub generate_account
{
    my ($ssn, $data, $acct, $relationship, $content) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        lang => \&lang,
        relationship => $relationship,
        content => $content,
        acct => $acct,
        );

    to_template(\%vars, \$data->{'account.tt'});
}

sub content_statuses
{
    my ($ssn, $data, $acct, $relationship, $statuses) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        acct => $acct,
        statuses => $statuses,
        create_status => sub { generate_status($ssn, $data, shift); },
        );

    generate_account($ssn, $data, $acct, $relationship, to_template(\%vars, \$data->{'account_statuses.tt'}));
}
