package template_helpers;
use strict;
use warnings;
use Exporter 'import';

our @EXPORT = qw( to_template simple_page );

use string_helpers 'simple_escape';

my $template = Template->new(
    {
        INTERPOLATE => 1,
        POST_CHOMP => 1,
        EVAL_PERL => 1,
        TRIM => 1
    });

sub pretty_error($)
{
    my $error = simple_escape(shift);
    << "END_ERROR";
<span class="e-error error-pad">
      $error
</span>
END_ERROR
}

sub to_template
{
    my ($vars, $data) = @_;
    my $result;

    return 0 unless ref $data;
    return 0 unless ref $vars;

    # TODO HTML error formatting
    $template->process($data, $vars, \$result) ||
        return pretty_error($template->error());

    $result;
}

# Generic simple page with only session data and pages.
# Pretty commonly done, so useful function.
sub simple_page
{
    my ($ssn, $data, $page) = @_;
    
    my %vars = (
        prefix => '',
        ssn => $ssn,
        );

    to_template(\%vars, \$data->{$page});
}
