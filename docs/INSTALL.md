# Installing Treebird

## Compiling

For the following UNIX-like system, you will need the following libraries/packages:

###### Debian

`# apt install libcurl4-gnutls-dev libfcgi-dev build-essential perl libperl-dev libtemplate-perl`

###### Arch

`# pacman -S curl base-devel perl perl-template-toolkit`

###### Gentoo

`# emerge -a dev-perl/Template-Toolkit dev-libs/cJSON www-servers/spawn-fcgi`

###### Void GNU/Linux

`# xbps-install libcurl libcurl-devel base-devel fcgi fcgi-devel perl-Template-Toolkit`

###### OpenBSD

`# pkg_add curl fcgi spawn-fcgi p5-Template cjson gmake`

(**Note:** Treebird supports Pledge and Unveil!)

###### FreeBSD

`# pkg install p5-Template-Toolkit fcgi-devkit curl gmake`

---

Install mastodont-c. `fossil clone https://code.nekobit.net/fossil/mastodont-c`,
`premake4 gmake`, `make`, `make install`.

Create a copy of `config.def.h` at `config.h`, edit the file with your
information. Note that this setup will change in the
future.

Run `make` (or `gmake`). 

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

- `/usr/local/share/treebird/` - Contains CSS, images, perl, and other meta files
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

## OpenHTTPd / httpd(8)

OpenBSD's httpd config file is available. Bare in mind that `slowcgi` *WILL*
not work (for now). Please use `spawn-fcgi`.

#### spawn-fcgi

`spawn-fcgi` can be used for both Apache, OpenHTTPd (httpd(8)) and Nginx. Read the manual for it to see how to work it, or view
the testing script at `scripts/fcgistarter.sh`

- Please, at all costs, avoid FCGIWrap. It's caused nothing but headaches and has proved no real use other than
  spitting `Cannot get script name, are DOCUMENT_ROOT and SCRIPT_NAME (or SCRIPT_FILENAME) set and is the script executable?`
  at you (even if those variables are set and the script is executable)
