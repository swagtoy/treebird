# Installing Treebird

## Compiling

For the following GNU/Linux distributions, you will need the following libraries/packages:

- libcurl
  - Debian: `libcurl4-gnutls-dev`
  - Arch: `curl`
  - Void: `libcurl libcurl-devel`
- libpcre2
  - Debian: `libpcre2-dev`
  - Void: `pcre2 pcre2-devel`
- libfcgi
  - Debian: `libfcgi-dev`
  - Void: `fcgi fcgi-devel`

Create a copy of `config.def.h` at `config.h`, edit the file with your information

Run `make`. This will also clone mastodont-c, and compile both it and Treebird.

If you `git pull` any changes, `make update` should be run after updating, as it ensures
that mastodont-c is up to date.

## Installation

- TODO?

Run `make install`, this might require root.

If this succeeds (assuming you used default variables), you can now find Treebird at the following

- `/usr/local/share/treebird/` - Contains CSS, images, and other meta files
- `/usr/local/bin/treebird` - Regular executable CGI file, test it by running it as is, it'll spit HTML out!

### Using NGINX (and fcgiwrap)

Treebird can be served over nginx by using a FastCGI server such as fcgiwrap.

The example static files will be in `/usr/local/share/treebird/`, with `treebird.cgi` at `/usr/local/bin/treebird`.

After running `make`, Treebird's files will be in the `dist/` directory. _Copy_, ***DO NOT MOVE***, **everything but treebird.cgi** of this folder to your web server. Copy `treebird.cgi` to another directory of your choosing.

#### nginx
An example nginx configuration is available in [treebird.nginx.conf](./sample/treebird.nginx.conf).
* Make sure to change `example.com` to your instance's domain.
* Make sure to change the `root` to wherever the static files are being stored

#### fcgiwrap
fcgiwrap can be installed on debian with `sudo apt install fcgiwrap`.

The example is using the default configuration included with the `fcgiwrap` package on Debian.

### Using Apache and mod_proxy_fcgi

TODO

Example Apache configuration is available in [treebird.apache.conf](./sample/treebird.apache.conf).