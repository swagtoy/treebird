# Installing Treebird

This is a stub and isn't complete, please update it!

## Compiling

For the following GNU/Linux distributions, you will need the following libraries/packages:

###### Debian

`# apt install libcurl4-gnutls-dev libpcre2-dev libfcgi-dev build-essential perl libperl-dev libtemplate-perl`

###### Void GNU/Linux

`# xbps-install libcurl libcurl-devel base-devel pcre2 pcre2-devel fcgi fcgi-devel perl-Template-Toolkit`

###### Arch

`# pacman -S curl base-devel perl perl-template-toolkit`

###### Gentoo

TODO

Create a copy of `config.def.h` at `config.h`, edit the file with your information

Run `make`. (**hint:** Pass -j3 to speed up compilation). This will also clone mastodont-c, and compile both it and Treebird.

If you `fossil update` any changes, `make update` should be run after updating

## Perl dependencies manual install

**Note:** You **WONT** need to do this if your distribution above included all the deps (Template Toolkit)

At the moment, all of them listed above do, but if your distro is nonstandard, keep reading:

---

Treebird renders most of the content that you see in Perl using the Template Toolkit.

You can install it by running `make install_deps`

If that doesn't work, you can open a CPAN shell

```
perl -MCPAN -e shell
install Template::Toolkit
```

## Installation

Run `# make install`

If this succeeds (assuming you used default variables), you can now find Treebird at the following

- `/usr/local/share/treebird/` - Contains CSS, images, and other meta files
- `/usr/local/bin/treebird` - Regular executable CGI file, test it by running it as is, it shouldn't spit anything out

## Development

For developing Treebird, see `DEVELOP.md`.

## Nginx

Treebird can be served over nginx by using a FastCGI daemon such as spawn-fcgi.

An example Nginx configuration is available in [treebird.nginx.conf](./sample/treebird.nginx.conf).
* Make sure to change `example.com` to your instance's domain.
* Make sure to change the `root` to wherever the static files are being stored

## Using Apache and mod_proxy_fcgi

Apache hasn't caused many troubles, and is in fact, what I use for development. You can see how to start
spawn-fcgi in `scripts/fcgistarter.sh`.

An example Apache configuration is available in [treebird.apache.conf](./sample/treebird.apache.conf).

#### spawn-fcgi

`spawn-fcgi` can be used for both Apache and Nginx. Read the manual for it to see how to work it, or view
the testing script at `scripts/fcgistarter.sh`

- Please, at all costs, avoid FCGIWrap. It's caused nothing but headaches and has proved no real use other than
  spitting `Cannot get script name, are DOCUMENT_ROOT and SCRIPT_NAME (or SCRIPT_FILENAME) set and is the script executable?`
  at you (even if those variables are set and the script is executable)
