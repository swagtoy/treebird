package icons;
use strict;
use warnings;
use Scalar::Util 'looks_like_number';

use Exporter 'import';

our @EXPORT = qw( &get_icon &get_icon_svg &get_icon_png &visibility_to_icon );

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

        fileclip => '<svg xmlns="http://www.w3.org/2000/svg" width="10" height="10" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21.44 11.05l-9.19 9.19a6 6 0 0 1-8.49-8.49l9.19-9.19a4 4 0 0 1 5.66 5.66l-9.2 9.19a2 2 0 0 1-2.83-2.83l8.49-8.48"></path></svg>',

        'local' => '<svg class="visibility vis-local" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 9v11a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V9"/><path d="M9 22V12h6v10M2 10.6L12 2l10 8.6"/></svg>',

        direct => '<svg class="visibility vis-direct xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 4h16c1.1 0 2 .9 2 2v12c0 1.1-.9 2-2 2H4c-1.1 0-2-.9-2-2V6c0-1.1.9-2 2-2z"></path><polyline points="22,6 12,13 2,6"></polyline></svg>',

        private => '<svg class="visibility vis-private" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect><path d="M7 11V7a5 5 0 0 1 10 0v4"></path></svg>',

        list => '<svg class="visibility vis-list" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="8" y1="6" x2="21" y2="6"></line><line x1="8" y1="12" x2="21" y2="12"></line><line x1="8" y1="18" x2="21" y2="18"></line><line x1="3" y1="6" x2="3.01" y2="6"></line><line x1="3" y1="12" x2="3.01" y2="12"></line><line x1="3" y1="18" x2="3.01" y2="18"></line></svg>',

        unlisted => '<svg class="visibility vis-unlisted" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect><path d="M7 11V7a5 5 0 0 1 9.9-1"></path></svg>',

        public => '<svg class="visibility vis-public" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><line x1="2" y1="12" x2="22" y2="12"></line><path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"></path></svg>',

        follow => '<svg class="follow" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"></path><circle cx="8.5" cy="7" r="4"></circle><line x1="20" y1="8" x2="20" y2="14"></line><line x1="23" y1="11" x2="17" y2="11"></line></svg>',

        search => '<svg class="search" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="11" cy="11" r="8"></circle><line x1="21" y1="21" x2="16.65" y2="16.65"></line></svg>',

        'search-menu' => '<svg class="search-menu" width="20" height="20" fill="none" stroke="#000000" stroke-linecap="round" stroke-linejoin="round" version="1.1" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><circle cx="10.107" cy="8.7642" r="6.7391" stroke-width="1.6848"/><line x1="18.531" x2="14.866" y1="17.188" y2="13.524" stroke-width="1.6848"/><path d="m14.756 20.738 1.9607 1.9607 1.9091-1.9297" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.6554"/></svg>',
        );

    $res{$_[0]};
}

sub visibility_to_icon
{
    # I thought of an array, but I don't want to call get_icon UNLESS
    # we know the visibility
    return unless looks_like_number($_[0]);
    my $vis = $_[0];

    return get_icon('public') if $vis == 1;
    return get_icon('unlisted') if $vis == 2;
    return get_icon('private') if $vis == 3;
    return get_icon('list') if $vis == 4;
    return get_icon('direct') if $vis == 5;
    return get_icon('local') if $vis == 6;
    
    # Assume local for anything else, because well... I'm not sure
    get_icon('local');
}

1;
