AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  clustering_demo.h

lib_LTLIBRARIES = \
  libclustering_demo.la

libclustering_demo_la_SOURCES = \
  clustering_demo.cc

libclustering_demo_la_LIBADD = \
  -lCLHEP \
  -lphool \
  -lSubsysReco \
  -lgsl \
  -lgslcblas \
  -lg4vertex_io \
  -lcalo_io \
  -lphparameter

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libclustering_demo.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
