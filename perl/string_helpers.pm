package string_helpers;
use strict;
use warnings;
use HTML::Escape 'escape_html';
use Exporter 'import';

our @EXPORT = qw( reltime_to_str greentextify emojify format_username get_mentions_from_content );

sub reltime_to_str
{
    my $since = time() - $_[0];

    return $since . 's' if $since < 60;
    return int($since / 60) . 'm' if $since < 60 * 60;
    return int($since / (60 * 60)) . 'h' if $since < 60 * 60 * 24;
    return int($since / (60 * 60 * 24)) . 'd' if $since < 60 * 60 * 24 * 31;
    return int($since / (60 * 60 * 24 * 31)) . 'mon' if $since < 60 * 60 * 24 * 365;
    return int($since / (60 * 60 * 24 * 365)) . 'yr';
}

sub greentextify
{
    my $text = shift;
    $text =~ s/((?:^|<br\/?>| )&gt;.*?)(?:<br\/?>|$)/<span class="greentext">$1<\/span><br>/gs;
    $text =~ s/((?:^|<br\/?>| )&lt;.*?)(?:<br\/?>|$)/<span class="bluetext">$1<\/span><br>/gs;
    $text =~ s/((?:^|<br\/?>| )\^.*?)(?:<br\/?>|$)/<span class="yellowtext">$1<\/span><br>/gs;
    $text;
}

sub emojify
{
    my ($text, $emojis) = @_;
    if ($emojis)
    {
        foreach my $emoji (@{$emojis})
        {
            my $emo = $emoji->{shortcode};
            my $url = $emoji->{url};
            $text =~ s/:$emo:/<img class="emoji" src="$url" loading="lazy">/gsi;
        }
    }
    $text;
}

sub format_username
{
    my $account = shift;
    emojify(escape_html($account->{display_name}), $account->{emojis});
}

sub get_mentions_from_content
{
    my ($ssn, $status) = @_;
    my $result = '';
    my $acct;
    # todo
    while ($status->{'content'} =~
           /<a .*?href=\"https?:\/\/(.*?)\/(?:@|users\/|u\/)?(.*?)?\".*?>@(?:<span>)?.*?(?:<\/span>)?/gs)
    {
        $acct = $2 . '@' . $1;
        $result .= '@' . $acct . ' ' if $ssn->{account}->{acct} eq $acct;
    }
    ($status->{account}->{acct} eq $ssn->{account}->{acct})
        ? $result : '@' . $status->{account}->{acct} . ' ' . $result;
}
