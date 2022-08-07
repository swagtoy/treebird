package timeline;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( content_timeline );

use template_helpers 'to_template';
use icons 'get_icon';
use postbox 'generate_postbox';
use status 'generate_status';

sub content_timeline
{
    my ($ssn, $data, $statuses, $title, $show_post_box) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        data => $data,
        statuses => $statuses,
        title => $title,
        show_post_box => $show_post_box,
        postbox => \&generate_postbox,
        create_status => sub { generate_status($ssn, $data, shift); },
        );

    to_template(\%vars, \$data->{'timeline.tt'});
}
