package chat;
use strict;
use warnings;
use Exporter 'import';

our @EXPORTS = qw( content_chats );

use template_helpers 'to_template';

sub construct_chat
{
    my ($ssn, $data, $chat) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        chat => $chat,
        );

    to_template(\%vars, \$data->{'chat.tt'});
}

sub content_chats
{
    my ($ssn, $data, $chats) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        chats => $chats,
        );

    to_template(\%vars, \$data->{'content_chats.tt'});
}

1;
