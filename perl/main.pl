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

# my $template = Template->new(
#     {
#         INTERPOLATE => 1,
#         POST_CHOMP => 1,
#         EVAL_PERL => 1,
#         TRIM => 1
#     });

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

    to_template(\%vars, \$data->{'main.tt'});
}

