package string_helpers;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( reltime_to_str greentextify emojify format_username get_mentions_from_content localize_mentions simple_escape random_error_kaomoji );

my $re_mentions = '(?=<a .*?mention.*?)<a .*?href="https?:\/\/(.*?)\/(?:@|users\/|\/u)?(.*?)?".*?>';

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

sub simple_escape
{
    my $text = shift;
    $text =~ s/&/&amp;/gs;
    $text =~ s/</&lt;/gs;
    $text =~ s/>/&gt;/gs;
    $text =~ s/"/&quot;/gs;
    $text;
}

sub greentextify
{
    my $text = shift;
    $text =~ s/(&gt;.*?)(?=<|$)/<span class="greentext">$1<\/span>/gs;
    $text =~ s/(&lt;.*?)(?=<|$)/<span class="bluetext">$1<\/span>/gs;
    $text =~ s/(?:^|>| )(\^.*?)(?=<|$)/<span class="yellowtext">$1<\/span>/gs;
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

    #TODO   ESCAPE DISPLAY NAME
    emojify(simple_escape($account->{display_name}), $account->{emojis});
}

sub localize_mentions
{
    my $text = shift;
    # idk how to work around this 
    my $at = '@';
                            
    $text =~ s/$re_mentions/<a target="_parent" class="mention" href="\/$at$2$at$1">/gs;
    $text;
}

sub get_mentions_from_content
{
    my ($ssn, $status) = @_;
    my $result = '';
    my $acct;
    while ($status->{'content'} =~
           /<a .*?href=\"https?:\/\/(.*?)\/(?:@|users\/|u\/)?(.*?)?\".*?>@(?:<span>)?.*?(?:<\/span>)?/gs)
    {
        $acct = $2 . '@' . $1;
        # TODO this does not account for the domain (alt interference)
        $result .= '@' . $acct unless $ssn->{account}->{acct} eq $2;
    }
    ($status->{account}->{acct} eq $ssn->{account}->{acct})
        ? $result : '@' . $status->{account}->{acct} . ' ' . $result;
}

sub random_error_kaomoji
{
    my @messages = (
        "(；￣Д￣）",
        "（｀Δ´）！",
        "¯\\_(ツ)_/¯",
        "(ﾉ´･ω･)ﾉ ﾐ ┸━┸",
        "(╯°□°）╯︵ ┻━┻",
        );
    @messages[rand(scalar @messages)];
}
