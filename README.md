# RatFE

A pleroma frontend for rats.

The goal is to create a frontend that's lightweight enough to be viewed without JS, but
usable enough to improve the experience with JS.

RatFE uses C with FCGI, mastodont-c (another library that complements RatFE) for
communication, and plain JavaScript. It also uses sqlite3 to store any user-specific
information and groups.

## Isn't FCGI outdated?

No.

## Isn't Sqlite3 slow?

No, in fact, it may likely be faster than if it were using postgresql. The pleroma server
communicates using postgresql because there are lots of incoming requests and outgoing
requests on the server, and there can be multiple requests at once occuring, which get
queued. We don't need that kind of speed and we shouldn't interfere with it anyway.

Plus, Sqlite3 is only being used for user groups and possibly some session information.

## Rat?

Rats are c00l.
