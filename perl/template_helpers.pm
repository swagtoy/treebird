package template_helpers;

use Exporter 'import';

our @EXPORT = qw( &to_template );

my $template = Template->new(
    {
        INTERPOLATE => 1,
        POST_CHOMP => 1,
        EVAL_PERL => 1,
        TRIM => 1
    });

sub to_template
{
    my ($vars, $data) = @_;
    my $result;

    0 unless ref $data;
    0 unless ref $vars;

    # TODO HTML error formatting
    $template->process($data, $vars, \$result) ||
        return $template->error();

    $result;
}
