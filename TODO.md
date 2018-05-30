## ToDo

* Add `--keep-alive / --no-keep-alive` options, so connections are either kept
  open or closed immediately after the first exchange
* Make message processing pluggable, i.e. the server receives data, passes it
  to a plugin, and returns to client whatever the plugin in question gave it
  back

