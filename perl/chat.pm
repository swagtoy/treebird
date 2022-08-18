package chat;
use strict;
use warnings;
use Exporter 'import';

our @EXPORTS = qw( content_chats construct_chat );

use template_helpers 'to_template';
use string_helpers 'format_username';

sub construct_chat
{
    my ($ssn, $data, $chat, $messages) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        chat => $chat,
        messages => $messages,
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
        format_username => \&format_username,
        );

    to_template(\%vars, \$data->{'content_chats.tt'});
}

1;
