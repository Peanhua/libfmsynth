examplefilesdir = $(pkgdatadir)/examples

examplefiles_DATA = 		\
	Echo.sbp		\
	HelloWorld.sbp		\
	HitExplosion.sbp	\
	Tremolo.sbp		\
	Vibrato.sbp		\
	Weapon1.sbp		\
	Weird1.sbp		\
	Weird2.sbp		


EXTRA_DIST = $(examplefiles_DATA)

uninstall-hook:
	-rmdir $(examplefilesdir)
