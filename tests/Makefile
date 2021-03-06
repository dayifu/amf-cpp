CXXFLAGS += -g -Wall -Wextra -pedantic -pthread -std=c++0x

ifneq ($(shell $(CXX) --version | grep clang),)
        ifeq ($(shell uname -s),Darwin)
                CXXFLAGS += -stdlib=libc++
        endif
endif

GTEST_DIR = googletest
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include
# and include AMF libs
CPPFLAGS += -I../src

# Tests produced by this Makefile
TESTS = main
# Tests used by the main test
SUBTESTS = array bool bytearray deserializer date dictionary double integer \
           null object packet serializer string undefined vector xml xmldocument
SUBTEST_OBJS = $(addsuffix .o,$(SUBTESTS))
# precompiled header name
PCH = amftest.hpp
# gtest files
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

all: $(TESTS)

clean:
	rm -f $(TESTS) $(SUBTEST_OBJS) .dep $(PCH).gch

dist-clean: clean
	rm -f gtest-all.o gtest_main.o

$(SUBTEST_OBJS): %.o : %.cpp $(GTEST_HEADERS) $(PCH).gch
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

main: $(SUBTEST_OBJS) ../libamf.a gtest-all.o gtest_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

$(PCH).gch: $(PCH)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

# gtest build rules
gtest-all.o: $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -isystem $(GTEST_DIR) $(CXXFLAGS) \
		-c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o: $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -isystem $(GTEST_DIR) $(CXXFLAGS) \
		-c $(GTEST_DIR)/src/gtest_main.cc

.dep:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $(addsuffix .cpp, $(SUBTESTS)) | \
		sed 's,:, $@:,' > $@
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MT $(PCH).gch $(PCH) | \
		sed 's,:, $@:,' >> $@

ifneq ($(MAKECMDGOALS),clean)
-include .dep
endif
