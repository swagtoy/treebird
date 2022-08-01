package postbox;
use strict;
use warnings;
use template_helpers 'to_template';
use string_helpers qw( get_mentions_from_content );
use Exporter 'import';

our @EXPORT = qw( generate_postbox );

sub generate_postbox
{
    my ($ssn, $data, $status) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        data => $data,
        status => $status,
        mentionify => \&get_mentions_from_content,
        );

    to_template(\%vars, \$data->{'postbox.tt'});
}
