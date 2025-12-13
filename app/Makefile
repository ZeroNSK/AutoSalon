# Makefile for AutoSalon Qt Application
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I/usr/include/qt5 -I/usr/include/qt5/QtCore -I/usr/include/qt5/QtSql -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui
LIBS = -lQt5Core -lQt5Sql -lQt5Widgets -lQt5Gui
MOC = moc-qt5
SRCDIR = src
OBJDIR = build
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/database.cpp $(SRCDIR)/operations.cpp $(SRCDIR)/gui/mainwindow.cpp
HEADERS = $(SRCDIR)/database.h $(SRCDIR)/operations.h $(SRCDIR)/gui/mainwindow.h
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/database.o $(OBJDIR)/operations.o $(OBJDIR)/mainwindow.o
MOC_SOURCES = $(OBJDIR)/moc_mainwindow.cpp
MOC_OBJECTS = $(OBJDIR)/moc_mainwindow.o

TARGET = AutoSalon

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Generate MOC files
$(OBJDIR)/moc_mainwindow.cpp: $(SRCDIR)/gui/mainwindow.h | $(OBJDIR)
	$(MOC) $(SRCDIR)/gui/mainwindow.h -o $(OBJDIR)/moc_mainwindow.cpp

# Compile source files
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o

$(OBJDIR)/database.o: $(SRCDIR)/database.cpp $(SRCDIR)/database.h | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRCDIR)/database.cpp -o $(OBJDIR)/database.o

$(OBJDIR)/operations.o: $(SRCDIR)/operations.cpp $(SRCDIR)/operations.h | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRCDIR)/operations.cpp -o $(OBJDIR)/operations.o

$(OBJDIR)/mainwindow.o: $(SRCDIR)/gui/mainwindow.cpp $(SRCDIR)/gui/mainwindow.h | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRCDIR)/gui/mainwindow.cpp -o $(OBJDIR)/mainwindow.o

$(OBJDIR)/moc_mainwindow.o: $(OBJDIR)/moc_mainwindow.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(OBJDIR)/moc_mainwindow.cpp -o $(OBJDIR)/moc_mainwindow.o

$(TARGET): $(OBJECTS) $(MOC_OBJECTS)
	$(CXX) $(OBJECTS) $(MOC_OBJECTS) $(LIBS) -o $(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)

install-deps:
	apt-get update && apt-get install -y \
		build-essential \
		qt5-default \
		libqt5sql5-dev \
		libqt5sql5-psql \
		libpq-dev \
		qtbase5-dev \
		qtbase5-dev-tools

.PHONY: all clean install-deps