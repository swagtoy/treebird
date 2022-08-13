package account;
use strict;
use warnings;
use Data::Dumper;

use Exporter 'import';

our @EXPORT = qw( account content_statuses );

use template_helpers 'to_template';
use l10n 'lang';
use status 'generate_status';
use string_helpers qw( simple_escape emojify random_error_kaomoji format_username );
use navigation 'generate_navigation';

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
        escape => \&simple_escape,
        emojify => \&emojify,
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
        # Make subroutine so Perl doesn't autovivify
        nav => sub { generate_navigation($ssn, $data, $statuses->[0]->{id}, $statuses->[-1]->{id}) },
        random_error_kaomoji => \&random_error_kaomoji,
        );

    generate_account($ssn, $data, $acct, $relationship, to_template(\%vars, \$data->{'account_statuses.tt'}));
}

sub generate_account_item
{
    my ($ssn, $data, $account) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        account => $account,
        # Functions
        icon => \&get_icon,
        lang => \&lang,
        format_username => \&format_username,
        );

    to_template(\%vars, \$data->{'account_item.tt'});
}

sub generate_account_list
{
    my ($ssn, $data, $accounts, $title) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        accounts => $accounts,
        title => $title,
        create_account => sub { generate_account_item($ssn, $data, shift); },
        nav => sub { generate_navigation($ssn, $data, $accounts->[0]->{id}, $accounts->[-1]->{id}) },
        );

    to_template(\%vars, \$data->{'accounts.tt'});
}

sub content_accounts
{
    my ($ssn, $data, $acct, $relationship, $accounts, $title) = @_;
    
    my $acct_list_page = generate_account_list($ssn, $data, $accounts, $title);

    # Should we create a full accounts view?
    if ($acct)
    {
        generate_account($ssn, $data, $acct, $relationship, $acct_list_page);
    }
    else {
        return $acct_list_page;
    }
}
