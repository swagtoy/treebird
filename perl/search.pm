package search;
use strict;
use warnings;
use Exporter 'import';

our @EXPORTS = qw( content_search search_tags search_accounts search_statuses );

use template_helpers 'to_template';

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

sub search_tags
{
    my ($ssn, $data, $tags) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        statuses => $statuses,
        );

    search_page($ssn, $data, SEARCH_CAT_STATUSES, to_template(\%vars, \$data->{'search_tags.tt'})); 
}

sub search_accounts
{
    my ($ssn, $data, $statuses) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        statuses => $statuses,
        );

    search_page($ssn, $data, SEARCH_CAT_STATUSES, to_template(\%vars, \$data->{'search_accounts.tt'})); 
}


sub search_statuses
{
    my ($ssn, $data, $statuses) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        statuses => $statuses,
        );

    search_page($ssn, $data, SEARCH_CAT_STATUSES, to_template(\%vars, \$data->{'search_statuses.tt'})); 
}

sub content_search
{
    my ($ssn, $data, $search) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        search => $search,
        );

    to_template(\%vars, \$data->{'content_search.tt'});
}

