@DX_RULES@

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src images examples doc

MOSTLYCLEANFILES = $(DX_CLEANFILES)


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfmsynth.pc


uninstall-hook:
	-rmdir $(pkgdatadir)
