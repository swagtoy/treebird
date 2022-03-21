# Installing Treebird

## Compiling

For the following GNU/Linux distributions, you will need the following libraries/packages:

- libcurl
  - Debian: `libcurl4-gnutls-dev`
  - Arch: `curl`
  - Void: `libcurl libcurl-devel`
- libcjson
  - Debian: `libcjson-dev`
  - Arch: `cjson`
  - Void: `cJSON cJSON-devel`
- libpcre
  - Debian: `libpcre3-dev`
  - Void: `libpcre2`
- libfcgi
  - Void: `fcgi fcgi-devel`

Create a copy of `config.def.h` at `config.h`, edit the file with your information

Run `make`. This will also clone mastodont-c, and compile both it and Treebird.

If you `git pull` any changes, `make update` should be run after updating, as it ensures
the library is up to date.

## Installation

- TODO?

Run `make install`, this might require root.

If this succeeds (assuming you used default variables), you can now find Treebird at the following

- `/usr/local/share/treebird/` - Contains CSS, images, and other meta files
- `/usr/local/bin/treebird` - Regular executable CGI file, test it by running it as is, it'll spit HTML out!

### Using NGINX (and uWSGI)

Treebird can be served over nginx by using a CGI server such as uwsgi.

The example static files will be in `/var/www/treebird/`, with `treebird.cgi` at `/usr/local/bin/treebird`.

After running `make`, Treebird's files will be in the `dist/` directory. _Copy_, ***DO NOT MOVE***, **everything but treebird.cgi** of this folder to your web server. Copy `treebird.cgi` to another directory of your choosing.

#### nginx
An example nginx configuration is available in [treebird.nginx.conf](./sample/treebird.nginx.conf).
* Make sure to change `example.com` to your instance's domain.
* Make sure to change the `root` to wherever the static files are being stored

#### uWSGI
uWSGI can be installed on debian with just `sudo apt isntall uwsgi`.

An example configuration is available in [treebird.uwsgi.ini](./sample/treebird.uwsgi.ini). This should be copied to `/etc/uwsgi/apps-enabled/treebird.ini` on debian (and potentially other) systems.

##### Note: the uwsgi configuration file is currently very basic.

### Using Apache and mod_proxy_fcgi

TODO