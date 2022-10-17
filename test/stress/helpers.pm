package helpers;
use strict;
use warnings;
use threads;

use Exporter 'import';

our @EXPORT_OK = qw( prompt );

use LWP::UserAgent;
my $ua = LWP::UserAgent->new;
$ua->agent("TreebirdTest/1.0 ");

sub prompt
{
    my ($msg, $suffix) = @_;
    print $msg . ($suffix || ':') . ' ';
    my $answer = <STDIN>;
    chomp($answer);
    $answer;
}

sub prompt_instance_info
{
    my $instance = prompt('Instance domain (http://localhost [blank], treebird.social)');

    unless ($instance)
    {
        $instance = 'http://localhost';
    }
    elsif (rindex($instance, 'http://', 0) + rindex($instance, 'https://', 0) == -2)
    {
        $instance = 'https://' . $instance;
    }

    return (
        $instance . '/'
        );
}

sub stress_test
{
    my ($name, $url, $num, $path) = @_;
    my $fullurl = $url . $path;
    my @threads;
    
    print "Sending requests to \"$fullurl\" for case \"$name\": ";
    foreach (0..$num)
    {
        print(".");
        push @threads, async {
            my $req = HTTP::Request->new(GET => $fullurl);
            $ua->request($req);
            
        };
    }

    foreach my $thread (@threads) {
        my $res = $thread->join;
        unless ($res->is_success)
        {
            print "\n========================\n";
            print "Test stress case \"$name\" failed!";
            print "\n========================\n";
            exit(1);
        }
    }
}

1;
