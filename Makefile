#
# OMNeT++/OMNEST Makefile for myTest20140210-10G
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out
#

# Name of target to be created (-o option)
TARGET = myTest20140210-10G$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -IMPCPtools \
    -IOLT \
    -IONU \
    -ISplitter \
    -Icommon \
    -Iresults \
    -Itraffic_gen \
    -Itraffic_gen/Messages

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/MPCPtools/MPCPTools.o \
    $O/OLT/DBA.o \
    $O/OLT/OLT_Scheduler.o \
    $O/OLT/Hungarian.o \
    $O/OLT/ONUTableEntry.o \
    $O/OLT/ONUTable.o \
    $O/ONU/PowerControler.o \
    $O/ONU/Scheduler.o \
    $O/ONU/Classifier.o \
    $O/Splitter/PON_Splitter.o \
    $O/traffic_gen/LocalNetwork.o \
    $O/traffic_gen/CoreNetwork.o \
    $O/traffic_gen/TrafficGen.o \
    $O/traffic_gen/Messages/EPON_messages_m.o \
    $O/traffic_gen/Messages/MyPacket_m.o

# Message files
MSGFILES = \
    traffic_gen/Messages/EPON_messages.msg \
    traffic_gen/Messages/MyPacket.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f myTest20140210-10G myTest20140210-10G.exe libmyTest20140210-10G.so libmyTest20140210-10G.a libmyTest20140210-10G.dll libmyTest20140210-10G.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h
	$(Q)-rm -f MPCPtools/*_m.cc MPCPtools/*_m.h
	$(Q)-rm -f OLT/*_m.cc OLT/*_m.h
	$(Q)-rm -f ONU/*_m.cc ONU/*_m.h
	$(Q)-rm -f Splitter/*_m.cc Splitter/*_m.h
	$(Q)-rm -f common/*_m.cc common/*_m.h
	$(Q)-rm -f results/*_m.cc results/*_m.h
	$(Q)-rm -f traffic_gen/*_m.cc traffic_gen/*_m.h
	$(Q)-rm -f traffic_gen/Messages/*_m.cc traffic_gen/Messages/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc MPCPtools/*.cc OLT/*.cc ONU/*.cc Splitter/*.cc common/*.cc results/*.cc traffic_gen/*.cc traffic_gen/Messages/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/MPCPtools/MPCPTools.o: MPCPtools/MPCPTools.cc \
	MPCPtools/MPCPTools.h \
	traffic_gen/Messages/EPON_messages_m.h
$O/OLT/DBA.o: OLT/DBA.cc \
	OLT/DBA.h \
	OLT/Hungarian.h \
	OLT/ONUTable.h \
	OLT/ONUTableEntry.h \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h \
	traffic_gen/Messages/MyPacket_m.h
$O/OLT/Hungarian.o: OLT/Hungarian.cc \
	OLT/Hungarian.h
$O/OLT/OLT_Scheduler.o: OLT/OLT_Scheduler.cc \
	OLT/DBA.h \
	OLT/Hungarian.h \
	OLT/ONUTable.h \
	OLT/ONUTableEntry.h \
	ONU/PowerControler.h \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h \
	traffic_gen/Messages/MyPacket_m.h
$O/OLT/ONUTable.o: OLT/ONUTable.cc \
	OLT/ONUTable.h \
	OLT/ONUTableEntry.h \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h \
	traffic_gen/Messages/MyPacket_m.h
$O/OLT/ONUTableEntry.o: OLT/ONUTableEntry.cc \
	OLT/ONUTableEntry.h \
	common/MPCP_codes.h
$O/ONU/Classifier.o: ONU/Classifier.cc \
	ONU/Classifier.h \
	ONU/PowerControler.h \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h \
	traffic_gen/Messages/MyPacket_m.h
$O/ONU/PowerControler.o: ONU/PowerControler.cc \
	ONU/PowerControler.h \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h \
	traffic_gen/Messages/MyPacket_m.h
$O/ONU/Scheduler.o: ONU/Scheduler.cc \
	MPCPtools/MPCPTools.h \
	OLT/DBA.h \
	OLT/Hungarian.h \
	OLT/ONUTable.h \
	OLT/ONUTableEntry.h \
	ONU/Classifier.h \
	ONU/PowerControler.h \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h \
	traffic_gen/Messages/MyPacket_m.h
$O/Splitter/PON_Splitter.o: Splitter/PON_Splitter.cc \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h \
	traffic_gen/Messages/MyPacket_m.h
$O/traffic_gen/CoreNetwork.o: traffic_gen/CoreNetwork.cc \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h
$O/traffic_gen/LocalNetwork.o: traffic_gen/LocalNetwork.cc \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h
$O/traffic_gen/TrafficGen.o: traffic_gen/TrafficGen.cc \
	common/MPCP_codes.h \
	traffic_gen/Messages/EPON_messages_m.h
$O/traffic_gen/Messages/EPON_messages_m.o: traffic_gen/Messages/EPON_messages_m.cc \
	traffic_gen/Messages/EPON_messages_m.h
$O/traffic_gen/Messages/MyPacket_m.o: traffic_gen/Messages/MyPacket_m.cc \
	traffic_gen/Messages/MyPacket_m.h

