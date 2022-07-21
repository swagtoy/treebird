# Installing Treebird

This is a stub and isn't complete, please update it!

## Compiling

For the following GNU/Linux distributions, you will need the following libraries/packages:

###### Debian

`# apt install libcurl4-gnutls-dev libpcre2-dev libfcgi-dev base-devel`

###### Void GNU/Linux

`# xbps-install libcurl libcurl-devel base-devel pcre2 pcre2-devel fcgi fcgi-devel`

###### Arch

`# pacman -S curl base-devel`

Create a copy of `config.def.h` at `config.h`, edit the file with your information

Run `make`. This will also clone mastodont-c, and compile both it and Treebird.

If you `fossil update` any changes, `make update` should be run after updating

## Installation

Run `# make install`

If this succeeds (assuming you used default variables), you can now find Treebird at the following

- `/usr/local/share/treebird/` - Contains CSS, images, and other meta files
- `/usr/local/bin/treebird` - Regular executable CGI file, test it by running it as is, it shouldn't spit anything out

### Using NGINX

Treebird can be served over nginx by using a FastCGI daemon such as spawn-fcgi.

The example static files will be in `/usr/local/share/treebird/`, with `treebird.cgi` at `/usr/local/bin/treebird`.

After running `make`, Treebird's files will be in the `dist/` directory. _Copy_, ***DO NOT MOVE***, **everything but treebird.cgi** of this folder to your web server. Copy `treebird.cgi` to another directory of your choosing.

## Nginx
An example Nginx configuration is available in [treebird.nginx.conf](./sample/treebird.nginx.conf).
* Make sure to change `example.com` to your instance's domain.
* Make sure to change the `root` to wherever the static files are being stored

## Using Apache and mod_proxy_fcgi

Apache hasn't caused many troubles, and is in fact, what I use for development. You can see how to start
spawn-fcgi in `scripts/fcgistarter.sh`.

Example Apache configuration is available in [treebird.apache.conf](./sample/treebird.apache.conf).

#### spawn-fcgi

`spawn-fcgi` can be used for both Apache and Nginx. Read the manual for it to see how to work it, or view
the testing script at `scripts/fcgistarter.sh`

- Please, at all costs, avoid FCGIWrap. It's caused nothing but headaches and has proved no real use other than
  spitting `Cannot get script name, are DOCUMENT_ROOT and SCRIPT_NAME (or SCRIPT_FILENAME) set and is the script executable?`
  at you (even if those variables are set and the script is executable)
