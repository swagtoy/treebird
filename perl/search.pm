package search;
use strict;
use warnings;
use Exporter 'import';

our @EXPORTS = qw( content_search content_search_tags content_search_accounts content_search_statuses search_tags search_accounts search_statuses );

use template_helpers 'to_template';
use status 'generate_status';
use account 'generate_account_item';

use constant
{
    SEARCH_CAT_STATUSES => 0,
    SEARCH_CAT_ACCOUNTS => 1,
    SEARCH_CAT_TAGS     => 2
};

sub search_page
{
    my ($ssn, $data, $tab, $content) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        tab => $tab,
        content => $content,
        );

    to_template(\%vars, \$data->{'search.tt'});
}

# CONTENT
sub search_accounts
{
    my ($ssn, $data, $search) = @_;
    
    my %vars = (
        prefix => '',
        ssn => $ssn,
        search => $search,
        create_account => sub { generate_account_item($ssn, $data, shift); },
        );

    to_template(\%vars, \$data->{'search_accounts.tt'})
}

sub search_statuses
{
    my ($ssn, $data, $search) = @_;
    
    my %vars = (
        prefix => '',
        ssn => $ssn,
        search => $search,
        create_status => sub { generate_status($ssn, $data, shift); },
        );

    to_template(\%vars, \$data->{'search_statuses.tt'})
}

sub search_tags
{
    my ($ssn, $data, $search) = @_;
    
    my %vars = (
        prefix => '',
        ssn => $ssn,
        search => $search,
        );

    to_template(\%vars, \$data->{'search_tags.tt'})
}

sub content_search_accounts
{
    search_page($_[0], $_[1], SEARCH_CAT_ACCOUNTS, search_accounts(@_)); 
}

sub content_search_statuses
{
    search_page($_[0], $_[1], SEARCH_CAT_STATUSES, search_statuses(@_)); 
}

sub content_search_tags
{
    search_page($_[0], $_[1], SEARCH_CAT_TAGS, search_tags(@_)); 
}

sub content_search
{
    my ($ssn, $data, $search) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        search => $search,
        
        statuses => search_statuses,
        accounts => search_accounts,
        hashtags => search_tags,
        );

    to_template(\%vars, \$data->{'content_search.tt'});
}

