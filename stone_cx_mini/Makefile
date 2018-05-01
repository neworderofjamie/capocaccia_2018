EXECUTABLE      := simulator
SOURCES         := simulator.cc simulatorCommon.cc
INCLUDE_FLAGS   := -I$(GENN_ROBOTICS_PATH)/common
LINK_FLAGS      := `pkg-config --libs opencv`
CXXFLAGS       := `pkg-config --cflags opencv`
CPU_ONLY=1

ifdef RECORD_ELECTROPHYS
    CXXFLAGS += -DRECORD_ELECTROPHYS
endif

include $(GENN_PATH)/userproject/include/makefile_common_gnu.mk
