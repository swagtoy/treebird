# Treebird

![Treebird logo](./meta/treebird.png)

A very lightweight Pleroma frontend.

The goal is to create a frontend that's lightweight enough to be viewed without JS, but
usable enough to improve the experience with JS.

Treebird uses C with FCGI, mastodont-c (library designed for Treebird, but can be used
for other applications as well), and plain JavaScript for the frontend (100% optional).

## Why?

PleromaFE, pleroma's default frontend, uses way too much Javascript to be usable (and doesn't even support
all of it's own API features...). BloatFE is great, but designed only around Mastodon's api, and isn't as
modern or as lightweight as it could be. Soapbox is soapbox and does soapbox things.

This led me to one choice, to develop my own frontend.

## Compatibility

Treebird respects compatibility with old browsers, and thus uses HTML table layouts, which are
supported even by most modern terminal web browsers. The core browser we aim to at least maintain compatibility
with is Netsurf, but most other browsers like GNU Emacs EWW, elinks, render Treebird wonderfully.

## Credits

Please view the [`CREDITS`](./CREDITS) file.

## Installing

See [INSTALL.md](./docs/INSTALL.md) for instructions on Apache/Nginx.
