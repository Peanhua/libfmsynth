.ui.hh:
	$(AM_V_GEN) $(QT_UIC) --output $@ $<

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
UiNodeInverse.hh		: UiNodeInverse.ui
UiNodeMultiply.hh		: UiNodeMultiply.ui
UiNodeOscillator.hh		: UiNodeOscillator.ui
UiNodeRangeConvert.hh		: UiNodeRangeConvert.ui
UiNodeReciprocal.hh		: UiNodeReciprocal.ui
UiNodeViewWaveform.hh		: UiNodeViewWaveform.ui



BUILT_SOURCES = 			\
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
	UiNodeInverse.hh		\
	UiNodeMultiply.hh		\
	UiNodeOscillator.hh		\
	UiNodeRangeConvert.hh		\
	UiNodeReciprocal.hh		\
	UiNodeViewWaveform.hh		

AM_CXXFLAGS =			\
	-DDATADIR='"$(datadir)/synth"' \
	$(ALUT_CFLAGS)		\
	$(FMT_CFLAGS)		\
	$(JSON_CFLAGS)		\
	$(OPENAL_CFLAGS)	\
	$(PTHREAD_CFLAGS)	\
	$(QT_CFLAGS)		\
	$(VALGRIND_CFLAGS)

lib_LTLIBRARIES = libfmsynth.la
libfmsynth_la_SOURCES =			\
	Blueprint.cc			\
	Blueprint.hh			\
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
	Types.hh			

pkginclude_HEADERS =			\
	Blueprint.hh			\
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
	NodeMemoryBuffer.hh		\
	NodeInverse.hh			\
	NodeMultiply.hh			\
	NodeOscillator.hh		\
	NodeRangeConvert.hh		\
	NodeReciprocal.hh		


bin_PROGRAMS = fmsedit fmsplay

#check_PROGRAMS = synth

#TESTS = $(check_PROGRAMS) test-compare-with-orig.sh

#test-compare-with-orig.sh : synth

fmsedit_LDADD =		\
	libfmsynth.la	\
	$(ALUT_LIBS)	\
	$(FMT_LIBS)	\
	$(JSON_LIBS)	\
	$(OPENAL_LIBS)	\
	$(PTHREAD_LIBS)	\
	$(QT_LIBS)

fmsplay_LDADD =		\
	libfmsynth.la	\
	$(ALUT_LIBS)	\
	$(FMT_LIBS)	\
	$(JSON_LIBS)	\
	$(OPENAL_LIBS)	\
	$(PTHREAD_LIBS)	


fmsedit_SOURCES =			\
	Link.cc				\
	Link.hh				\
	Player.cc			\
	Player.hh			\
	Settings.cc			\
	Settings.hh			\
	Types.hh			\
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

fmsplay_SOURCES =			\
	Player.cc			\
	Player.hh			\
	fmsplay.cc			

nodist_fmsedit_SOURCES = $(BUILT_SOURCES)

CLEANFILES = $(nodist_fmsedit_SOURCES)
