# Installing Treebird

## Compiling

You need the following libraries / packages:

- libcurl
  - Debian: `libcurl4-gnutls-dev`
- libcjson
  - Debian: `libcjson-dev`
  - Arch: `cjson`
- libpcre3
  - Debian: `libpcre3-dev`

First, edit `config.h` to your instance's liking.

`make` will clone the library, and compile both it and the project.

`make update` should be run after updating, as it ensures both the library and the project are up to date.

## Deploying

After running `make`, Treebird's files will be in the `dist/` directory. _Copy_, ***DO NOT MOVE***, **everything but treebird.cgi** of this folder to your web server. Copy `treebird.cgi` to another directory of your choosing.

- The example static files will be in `/var/www/treebird/`, with `treebird.cgi` in `/usr/local/bin/treebird.cgi`.

### Using NGINX (and uWSGI) to serve Treebird
Treebird can be served over nginx by using a CGI server such as uwsgi.

#### nginx
An example nginx configuration is available in [treebird.nginx.conf](./treebird.nginx.conf).
* Make sure to change `example.com` to your instance's domain.
* Make sure to change the `root` to wherever the static files are being stored

#### uWSGI
uWSGI can be installed on debian with just `sudo apt isntall uwsgi`.

An example configuration is available in [treebird.uwsgi.ini](./treebird.uwsgi.ini). This should be copied to `/etc/uwsgi/apps-enabled/treebird.ini` on debian (and potentially other) systems.

##### Note: the uwsgi configuration file is currently very basic.