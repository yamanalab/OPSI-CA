HELIB     =/usr/local/src/HElib/src
COMPILER  = g++
#CPPFLAGS  = -std=c++11 -Wall -DNDEBUG -g2
CPPFLAGS  = -std=c++11 -Wall -g -O2
LDFLAGS   = -L/usr/local/lib -pthread -DFHE_THREADS -DFHE_DCRT_THREADS -DFHE_BOOT_THREADS
#LIBS      = $(HELIB)/fhe.a -lntl -lgmp -lm
LIBS      = $(HELIB)/fhe.a -lntl -lgmp -lm -lrt -lstdc++
INCLUDE   = -I$(HELIB) -I../share 
TARGET    = cloud
OBJDIR	  =./obj
SHAREOBJDIR =../share/obj
OBJECTS   = $(OBJDIR)/main.o  $(OBJDIR)/cloud.o $(OBJDIR)/protocol.o $(SHAREOBJDIR)/logger.o $(SHAREOBJDIR)/server.o $(SHAREOBJDIR)/socketlib.o $(SHAREOBJDIR)/function.o $(SHAREOBJDIR)/bloomfilter.o $(SHAREOBJDIR)/MurmurHash3.o
CREATEDIR   = ./log ./datasets ./pubkey ./result


$(TARGET): $(OBJECTS)
	mkdir -p $(CREATEDIR) 
	$(COMPILER) -o $@ $^ $(LDFLAGS) $(LIBS)
$(SHAREOBJDIR)/logger.o: ../share/logger.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/socketlib.o: ../share/socketlib.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/function.o: ../share/function.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/bloomfilter.o: ../share/bloomfilter.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/MurmurHash3.o: ../share/MurmurHash3.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/server.o: ../share/server.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(OBJDIR)/main.o: main.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(OBJDIR)/cloud.o: cloud.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(OBJDIR)/protocol.o: protocol.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)
	-rm -rf $(CREATEDIR)

