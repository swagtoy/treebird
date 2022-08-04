package timeline;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( generate_attachment );

use template_helpers 'to_template';
use icons 'get_icon';

sub content_timeline
{
    my ($ssn, $data, $statuses, $title, $show_post_box) = @_;

    my %vars = (
        prefix => '',
        ssn => $ssn,
        statuses => $statuses,
        title => $title,
        show_post_box => $show_post_box,
        );

    to_template(\&vars, \&data->{'timeline.tt'});
}
