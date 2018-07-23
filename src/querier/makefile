HELIB     = /usr/local/src/HElib/src
COMPILER  = g++
#CPPFLAGS  = -std=c++11 -Wall -DNDEBUG -g2
CPPFLAGS  = -std=c++11 -Wall -g -O2
LDFLAGS   = -L/usr/local/lib -pthread -DFHE_THREADS -DFHE_DCRT_THREADS -DFHE_BOOT_THREADS
#LIBS      = $(HELIB)/fhe.a -lntl -lgmp -lm
LIBS      = $(HELIB)/fhe.a -lntl -lgmp -lm -lrt -lstdc++
INCLUDE   = -I$(HELIB) -I../share  
TARGET    = querier
OBJDIR	  =./obj
SHAREOBJDIR =../share/obj
OBJECTS   = $(OBJDIR)/main.o $(OBJDIR)/querier.o $(OBJDIR)/queriersocket.o $(SHAREOBJDIR)/client.o $(SHAREOBJDIR)/function.o $(SHAREOBJDIR)/bloomfilter.o $(SHAREOBJDIR)/MurmurHash3.o $(SHAREOBJDIR)/socketlib.o $(SHAREOBJDIR)/logger.o

CREATEDIR   = ./log ./pubkey ./seckey ./result

$(TARGET): $(OBJECTS) 
	$(COMPILER) -o $@ $^ $(LDFLAGS) $(LIBS)

$(OBJDIR)/main.o: main.cpp
	mkdir -p $(CREATEDIR)
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(OBJDIR)/querier.o: querier.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(OBJDIR)/queriersocket.o: queriersocket.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/client.o: ../share/client.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/function.o: ../share/function.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/bloomfilter.o: ../share/bloomfilter.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/MurmurHash3.o: ../share/MurmurHash3.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/socketlib.o: ../share/socketlib.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
$(SHAREOBJDIR)/logger.o: ../share/logger.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<
all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET) $(DELETETXT)
	-rm -rf $(CREATEDIR)

