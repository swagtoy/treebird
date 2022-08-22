package emojis;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_emoji );

use template_helpers 'to_template';

sub generate_emoji
{
    my ($ssn, $data, $status_id, $emoji) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        status_id => $status_id,
        emoji => $emoji
        );

    to_template(\%vars, \$data->{'emoji.tt'});
}

sub emoji_picker
{
    my ($data, $emojis) = @_;

    my %vars = (
        e => $emojis
        );

    to_template(\%vars, \$data->{'emoji_picker.tt'});
}

1;
