use strict;
use warnings;
# Modules
use Template;
use l10n qw( &lang %L10N );
use notifications qw( generate_notification content_notifications );
use template_helpers qw( &to_template );
use timeline;
use icons 'get_icon';
use status;
use account;
use lists;
use search;
use chat;
use config;
use embed;
use meta;
use login;
# use Devel::Leak;

# my $handle;
# Devel::Leak::NoteSV($handle);

sub base_page
{
    my ($ssn,
        $data,
        $main,
        $notifs) = @_;
    my $result;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        title => $L10N{'EN_US'}->{'APP_NAME'},
        lang => \&lang,
        main => $main,
        icon => \&get_icon,
        sidebar_opacity => $ssn->{config}->{sidebar_opacity} / 255,
        acct => $ssn->{account},
        data => $data,
        notifs => $notifs,
        notification => \&generate_notification,
        );

    my $ret = to_template(\%vars, \$data->{'main.tt'});

#    leaky_friend();

    return $ret;
}

# sub leaky_friend
# {
#     $count = Devel::Leak::CheckSV($handle);
#     my $leakstr = "Memory: $count SVs\n";
#     print STDERR $leakstr;
# }
