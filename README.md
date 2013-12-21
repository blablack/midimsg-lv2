midimsg-lv2
===========

A collection of basic LV2 plugins to translate midi messages to usable values.

The set contains:
- Channel Aftertouch
- MIDI Controller
- ModWheel

More information can be found here:
[http://objectivewave.wordpress.com/midimsg-lv2](http://objectivewave.wordpress.com/midimsg-lv2)


INSTALL
-------

	$  ./waf configure
	$  ./waf 
	$  ./waf install


RUNNING
-------

After the INSTALL step any LV2 host should automatically find the plugins and add them to the list of loadable ones.

Ingen is the recommended host.
More information about Ingen here: [http://drobilla.net/software/ingen/](http://drobilla.net/software/ingen/)


BUG REPORTING
-------------
Please use the issue tracker on github to report bugs:
[http://github.com/blablack/midimsg-lv2/issues](http://github.com/blablack/midimsg-lv2/issues)



