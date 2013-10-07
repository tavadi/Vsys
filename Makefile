# ############################################################################
# ############################################################################
#
# 				GEPRI - Uebung 4 Version 1.0  -   08.05.2013
#
#						Hörmann Daniel - if12b067
#					 Thorstensen Benjamin - if12b066
#
# ############################################################################
# ############################################################################
#
#
# Makefile for gridserver.cpp, vehicleclient.cpp, griddisplay.cpp


all: myclient myserver

gridserver: myclient.cpp
	g++ -g -Wall -Werror -o myclient myclient.cpp

vehicleclient: myserver.cpp
	g++ -g -Wall -Werror -o myserver myserver.cpp



clean: 
	rm -f myserver
	rm -f myclient

dist:
	mkdir client_server-0.5
	cp *.cpp *.h Makefile client_server-0.5
	tar cvzf client_server-1.1.tgz client_server-0.5
	rm -R -f client_server-0.5
	
distclean:
	rm -f *.o client_server-0.5.tgz