# Technopark Highload WebServer [![Build Status](https://travis-ci.org/shramru/webserver.svg?branch=master)](https://travis-ci.org/shramru/webserver)

To build and run:

- cmake .
- make
- ./httpd -n &lt;min_cpus&gt; -x &lt;max_cpus&gt; -p &lt;port&gt; -r "&lt;path_to_root_dir&gt;"

By default:

- min_cpus = &lt;count_of_cores&gt;
- max_cpus = min_cpus
- port = 80
- path_to_root_dir = &lt;current_dir&gt;

You should use root user (sudo) to bind port 80.