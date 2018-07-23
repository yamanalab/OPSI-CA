LOUDDIR =./src/cloud
DATAOWNERADIR =./src/dataowner/dataownerA
DATAOWNERBDIR =./src/dataowner/dataownerB
QUERIERDIR =./src/querier

subsystem:
        $(MAKE) -C $(CLOUDDIR)
        $(MAKE) -C $(DATAOWNERADIR)
        $(MAKE) -C $(DATAOWNERBDIR)
        $(MAKE) -C $(QUERIERDIR)

clean:
        $(MAKE) -C $(CLOUDDIR) clean
        $(MAKE) -C $(DATAOWNERADIR) clean
        $(MAKE) -C $(DATAOWNERBDIR) clean
        $(MAKE) -C $(QUERIERDIR) clean
