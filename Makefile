CC=c++ -g -fpermissive
I=-I/home/graves/software/include
L=-L/home/graves/software/lib
LIBS=-lFOX -lcdaudio

fxcd: cd.o CDWindow.o CDPlayer.o CDSeekButton.o CDPreferences.o CDInfo.o CDBMPIcon.o CDServerDialog.o
	$(CC) -o fxcd cd.o CDWindow.o CDPlayer.o CDInfo.o CDSeekButton.o CDPreferences.o CDBMPIcon.o CDServerDialog.o $(L) $(LIBS)

cd.o: cd.cpp CDWindow.h
	$(CC) -c cd.cpp $(I)

CDWindow.o: CDWindow.cpp CDWindow.h icons.h
	$(CC) -c CDWindow.cpp $(I)

CDPreferences.o: CDPreferences.cpp CDPreferences.h
	$(CC) -c CDPreferences.cpp $(I)

CDPlayer.o: CDPlayer.cpp CDPlayer.h
	$(CC) -c CDPlayer.cpp $(I)

CDInfo.o: CDInfo.cpp CDInfo.h
	$(CC) -c CDInfo.cpp $(I)

CDSeekButton.o: CDSeekButton.cpp CDSeekButton.h
	$(CC) -c CDSeekButton.cpp $(I)

CDServerDialog.o: CDServerDialog.cpp CDServerDialog.h
	$(CC) -c CDServerDialog.cpp $(I)

CDBMPIcon.o: CDBMPIcon.cpp CDBMPIcon.h
	$(CC) -c CDBMPIcon.cpp $(I)
