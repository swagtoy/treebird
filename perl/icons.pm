package icons;
use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw( &get_icon &get_icon_svg &get_icon_png );

sub get_icon
{
    my ($ico, $is_png) = @_;
    $is_png ||= 0;

    $is_png ? get_icon_png($ico) : get_icon_svg($ico);
}

sub get_icon_svg
{
    my %res = (
        repeat => '<svg class="repeat" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 2.1l4 4-4 4"/><path d="M3 12.2v-2a4 4 0 0 1 4-4h12.8M7 21.9l-4-4 4-4"/><path d="M21 11.8v2a4 4 0 0 1-4 4H4.2"/></svg>',
        like => '<svg class="like" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon></svg>',
        expand => '<svg class="expand" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M15 3h6v6M14 10l6.1-6.1M9 21H3v-6M10 14l-6.1 6.1"/></svg>',
        reply => '<svg class="reply" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 9l6 6-6 6"/><path d="M4 4v7a4 4 0 0 0 4 4h11"/></svg>',
        emoji => '<svg class="emoji-btn" width="20" height="20" fill="none" stroke="#000000" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" version="1.1" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><circle cx="12" cy="12" r="10"/><g><line x1="9" x2="9" y1="6.9367" y2="11.755" stroke-width="1.7916"/><line x1="15" x2="15" y1="6.9367" y2="11.755" stroke-width="1.7916"/><path d="m7.0891 15.099s4.7206 4.7543 9.7109 0" stroke-linecap="round" stroke-linejoin="miter" stroke-width="1.9764"/></g></svg>',
        likeboost => '<svg class="one-click-software" width="20" height="20" fill="none" stroke="#000000" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" version="1.1" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><g><g stroke-width="1.98"><path d="m19.15 8.5061 2.7598 2.7598-2.7598 2.7598"/><path d="m14.756 11.325s2.5484-0.05032 6.3258 0.01026m-15.639 10.807-2.7598-2.7598 2.7598-2.7598"/><path d="m22.4 15.327v1.2259c0 1.156-1.2356 2.7598-2.7598 2.7598h-16.664"/></g><polygon transform="matrix(.60736 0 0 .60736 .60106 .63577)" points="18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2 15.09 8.26 22 9.27 17 14.14" stroke-width="2.9656"/></g></svg>',
        );

    $res{$_[0]};
}
