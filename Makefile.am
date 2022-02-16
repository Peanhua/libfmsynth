@DX_RULES@

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src images examples

MOSTLYCLEANFILES = $(DX_CLEANFILES)


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfmsynth.pc


uninstall-hook:
	-rmdir $(pkgdatadir)


man_MANS = fmsedit.1 fmsplay.1

dist_man_MANS = $(man_MANS)
