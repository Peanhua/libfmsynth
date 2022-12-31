.ui.hh:
	$(AM_V_GEN) $(QT_UIC) --output $@ $<
	if [ $(PRE512QT) -ne 0 ]; then \
		$(SED) -i '/PlaceholderText/d' $@ ; \
	fi

UiHelpAbout.hh			: UiHelpAbout.ui
UiMainWindow.hh			: UiMainWindow.ui
UiNode.hh			: UiNode.ui
UiNodeADHSR.hh			: UiNodeADHSR.ui
UiNodeAdd.hh			: UiNodeAdd.ui
UiNodeAudioDeviceOutput.hh	: UiNodeAudioDeviceOutput.ui
UiNodeAverage.hh		: UiNodeAverage.ui
UiNodeComment.hh		: UiNodeComment.ui
UiNodeConstant.hh		: UiNodeConstant.ui
UiNodeClamp.hh			: UiNodeClamp.ui
UiNodeDelay.hh			: UiNodeDelay.ui
UiNodeFileOutput.hh		: UiNodeFileOutput.ui
UiNodeFilter.hh			: UiNodeFilter.ui
UiNodeGrowth.hh			: UiNodeGrowth.ui
UiNodeInverse.hh		: UiNodeInverse.ui
UiNodeMultiply.hh		: UiNodeMultiply.ui
UiNodeOscillator.hh		: UiNodeOscillator.ui
UiNodeRangeConvert.hh		: UiNodeRangeConvert.ui
UiNodeReciprocal.hh		: UiNodeReciprocal.ui
UiNodeViewWaveform.hh		: UiNodeViewWaveform.ui



AM_CXXFLAGS =				\
	-DDATADIR='"$(pkgdatadir)"'	\
	-DLIBFMSYNTH_ENABLE_NODETESTING=$(ENABLE_NODETESTING)	\
	$(JSON_CFLAGS)			\
	$(PTHREAD_CFLAGS)		\
	$(VALGRIND_CFLAGS)


pkginclude_HEADERS =			\
	Blueprint.hh			\
	ConstantValue.hh		\
	Input.hh			\
	Node.hh				\
	NodeADHSR.hh			\
	NodeAdd.hh			\
	NodeAudioDeviceOutput.hh	\
	NodeAverage.hh			\
	NodeClamp.hh			\
	NodeConstant.hh			\
	NodeDelay.hh			\
	NodeFileOutput.hh		\
	NodeFilter.hh			\
	NodeGrowth.hh			\
	NodeMemoryBuffer.hh		\
	NodeInverse.hh			\
	NodeMultiply.hh			\
	NodeOscillator.hh		\
	NodeRangeConvert.hh		\
	NodeReciprocal.hh		\
	Output.hh			\
	Util.hh



lib_LTLIBRARIES = libfmsynth.la

bin_PROGRAMS = fmsplay fmsbench

BUILT_SOURCES = 

CLEANFILES = 


# libfmsynth:
libfmsynth_la_SOURCES =			\
	Blueprint.cc			\
	Blueprint.hh			\
	ConstantValue.cc		\
	ConstantValue.hh		\
	Input.cc			\
	Input.hh			\
	Node.cc				\
	Node.hh				\
	Node_Create.cc			\
	NodeADHSR.cc			\
	NodeADHSR.hh			\
	NodeAdd.cc			\
	NodeAdd.hh			\
	NodeAudioDeviceOutput.cc	\
	NodeAudioDeviceOutput.hh	\
	NodeAverage.cc			\
	NodeAverage.hh			\
	NodeClamp.cc			\
	NodeClamp.hh			\
	NodeConstant.cc			\
	NodeConstant.hh			\
	NodeDelay.cc			\
	NodeDelay.hh			\
	NodeFileOutput.cc		\
	NodeFileOutput.hh		\
	NodeFilter.cc			\
	NodeFilter.hh			\
	NodeGrowth.cc			\
	NodeGrowth.hh			\
	NodeInverse.cc			\
	NodeInverse.hh			\
	NodeMemoryBuffer.cc		\
	NodeMemoryBuffer.hh		\
	NodeMultiply.cc			\
	NodeMultiply.hh			\
	NodeOscillator.cc		\
	NodeOscillator.hh		\
	NodeRangeConvert.cc		\
	NodeRangeConvert.hh		\
	NodeReciprocal.cc		\
	NodeReciprocal.hh		\
	Output.cc			\
	Output.hh			\
	RtAudio.hh			\
	Util.cc				\
	Util.hh

if !ENABLE_NODETESTING
install-data-hook:
	$(AM_V_GEN) $(SED) -i 's/ifndef LIBFMSYNTH_ENABLE_NODETESTING/if 1/' $(includedir)/libfmsynth/Node.hh
	$(AM_V_GEN) $(SED) -i 's/define LIBFMSYNTH_ENABLE_NODETESTING 1/define LIBFMSYNTH_ENABLE_NODETESTING 0/' $(includedir)/libfmsynth/Node.hh
endif


# fmsedit:
if FMSEDIT

BUILT_SOURCES += 			\
	UiEditNodeProperties.hh		\
	UiHelpAbout.hh			\
	UiMainWindow.hh			\
	UiNode.hh			\
	UiNodeADHSR.hh			\
	UiNodeAdd.hh			\
	UiNodeAudioDeviceOutput.hh	\
	UiNodeAverage.hh		\
	UiNodeClamp.hh			\
	UiNodeComment.hh		\
	UiNodeConstant.hh		\
	UiNodeDelay.hh			\
	UiNodeFileOutput.hh		\
	UiNodeFilter.hh			\
	UiNodeGrowth.hh			\
	UiNodeInverse.hh		\
	UiNodeMultiply.hh		\
	UiNodeOscillator.hh		\
	UiNodeRangeConvert.hh		\
	UiNodeReciprocal.hh		\
	UiNodeViewWaveform.hh		

bin_PROGRAMS += fmsedit

fmsedit_CXXFLAGS = 			\
	$(AM_CXXFLAGS)			\
	$(EXTRA_FMSEDIT_QT_CXXFLAGS)	\
	$(FMT_CFLAGS)			\
	$(QT_CFLAGS)			\
	$(RTAUDIO_CFLAGS)		

fmsedit_LDADD =		\
	libfmsynth.la	\
	$(FMT_LIBS)	\
	$(JSON_LIBS)	\
	$(PTHREAD_LIBS)	\
	$(QT_LIBS)	\
	$(RTAUDIO_LIBS)

fmsedit_SOURCES =			\
	AudioDevice.cc			\
	AudioDevice.hh			\
	Link.cc				\
	Link.hh				\
	Player.cc			\
	Player.hh			\
	Settings.cc			\
	Settings.hh			\
	QtIncludeBegin.hh		\
	QtIncludeEnd.hh			\
	Ui.hh				\
	UiEditNodeProperties.ui		\
	UiHelpAbout.ui			\
	UiMainWindow.ui			\
	UiNode.ui			\
	UiNodeAdd.ui			\
	UiNodeADHSR.ui			\
	UiNodeAudioDeviceOutput.ui	\
	UiNodeAverage.ui		\
	UiNodeClamp.ui			\
	UiNodeComment.ui		\
	UiNodeConstant.ui		\
	UiNodeDelay.ui			\
	UiNodeFileOutput.ui		\
	UiNodeFilter.ui			\
	UiNodeGrowth.ui			\
	UiNodeInverse.ui		\
	UiNodeMultiply.ui		\
	UiNodeOscillator.ui		\
	UiNodeRangeConvert.ui		\
	UiNodeReciprocal.ui		\
	UiNodeViewWaveform.ui		\
	WidgetAddNode.cc		\
	WidgetAddNode.hh		\
	WidgetBlueprint.cc		\
	WidgetBlueprint.hh		\
	WidgetConnector.cc		\
	WidgetConnector.hh		\
	WidgetConnectorInput.cc		\
	WidgetConnectorInput.hh		\
	WidgetConnectorOutput.cc	\
	WidgetConnectorOutput.hh	\
	WidgetEditNodeProperties.cc	\
	WidgetEditNodeProperties.hh	\
	WidgetGraphADHSR.cc		\
	WidgetGraphADHSR.hh		\
	WidgetGraphWaveform.cc		\
	WidgetGraphWaveform.hh		\
	WidgetHelpAbout.cc		\
	WidgetHelpAbout.hh		\
	WidgetInputConstant.cc		\
	WidgetInputConstant.hh		\
	WidgetLine.cc			\
	WidgetLine.hh			\
	WidgetMainWindow.cc		\
	WidgetMainWindow.hh		\
	WidgetNode.cc			\
	WidgetNode.hh			\
	WidgetNodeADHSR.cc		\
	WidgetNodeADHSR.hh		\
	WidgetNodeAdd.cc		\
	WidgetNodeAdd.hh		\
	WidgetNodeAudioDeviceOutput.cc	\
	WidgetNodeAudioDeviceOutput.hh	\
	WidgetNodeAverage.cc		\
	WidgetNodeAverage.hh		\
	WidgetNodeClamp.cc		\
	WidgetNodeClamp.hh		\
	WidgetNodeComment.cc		\
	WidgetNodeComment.hh		\
	WidgetNodeConstant.cc		\
	WidgetNodeConstant.hh		\
	WidgetNodeDelay.cc		\
	WidgetNodeDelay.hh		\
	WidgetNodeFileOutput.cc		\
	WidgetNodeFileOutput.hh		\
	WidgetNodeFilter.cc		\
	WidgetNodeFilter.hh		\
	WidgetNodeGrowth.cc		\
	WidgetNodeGrowth.hh		\
	WidgetNodeInverse.cc		\
	WidgetNodeInverse.hh		\
	WidgetNodeMultiply.cc		\
	WidgetNodeMultiply.hh		\
	WidgetNodeOscillator.cc		\
	WidgetNodeOscillator.hh		\
	WidgetNodeRangeConvert.cc	\
	WidgetNodeRangeConvert.hh	\
	WidgetNodeReciprocal.cc		\
	WidgetNodeReciprocal.hh		\
	WidgetNodeViewWaveform.cc	\
	WidgetNodeViewWaveform.hh	\
	WidgetRectangle.cc		\
	WidgetRectangle.hh		\
	fmsedit.cc				

nodist_fmsedit_SOURCES = $(BUILT_SOURCES)

CLEANFILES += $(nodist_fmsedit_SOURCES)
endif


# fmsplay:
fmsplay_CXXFLAGS = 		\
	$(AM_CXXFLAGS)		\
	$(RTAUDIO_CFLAGS)		

fmsplay_LDADD =		\
	libfmsynth.la	\
	$(JSON_LIBS)	\
	$(PTHREAD_LIBS)	\
	$(RTAUDIO_LIBS)	

fmsplay_SOURCES =	\
	AudioDevice.cc	\
	AudioDevice.hh	\
	fmsplay.cc			


# fmsbench:
fmsbench_LDADD =	\
	libfmsynth.la	\
	$(JSON_LIBS)	

fmsbench_SOURCES =	\
	fmsbench.cc			


# Uninstall:
uninstall-hook:
	-rmdir $(pkgincludedir)



# Testing:
check_PROGRAMS = BlueprintTest InputTest NodeTest NodeAddTest NodeDelayTest NodeGrowthTest NodeOscillatorTest NodeRangeConvertTest

TESTS = $(check_PROGRAMS)

EXTRA_DIST = Test.hh BlueprintTest.cc InputTest.cc NodeTest.cc NodeAddTest.cc NodeDelayTest.cc NodeGrowthTest.cc NodeOscillatorTest.cc NodeRangeConvertTest.cc

BlueprintTest_LDADD = $(NodeTest_LDADD)
BlueprintTest_SOURCES = BlueprintTest.cc Test.hh

InputTest_LDADD = libfmsynth.la $(JSON_LIBS)
InputTest_SOURCES = InputTest.cc Test.hh

NodeTest_LDADD = libfmsynth.la $(JSON_LIBS)
NodeTest_SOURCES = NodeTest.cc Test.hh

NodeAddTest_LDADD = $(NodeTest_LDADD)
NodeAddTest_SOURCES = NodeAddTest.cc Test.hh

NodeDelayTest_LDADD = $(NodeTest_LDADD)
NodeDelayTest_SOURCES = NodeDelayTest.cc Test.hh

NodeGrowthTest_LDADD = $(NodeTest_LDADD)
NodeGrowthTest_SOURCES = NodeGrowthTest.cc Test.hh

NodeOscillatorTest_LDADD = $(NodeTest_LDADD)
NodeOscillatorTest_SOURCES = NodeOscillatorTest.cc Test.hh

NodeRangeConvertTest_LDADD = $(NodeTest_LDADD)
NodeRangeConvertTest_SOURCES = NodeRangeConvertTest.cc Test.hh

# Testing TAP setup:
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh
