package chat;
use strict;
use warnings;
use Exporter 'import';

our @EXPORTS = qw( content_chats construct_chat );

use template_helpers 'to_template';
use string_helpers qw( format_username emojify reltime_to_str );

sub construct_chat
{
    my ($ssn, $data, $chat, $messages) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        chat => $chat,
        messages => $messages,
        format_username => \&format_username,
        emojify => \&emojify,
        reltime => \&reltime_to_str,
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
