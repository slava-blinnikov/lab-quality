QT			+=	widgets xml sql
TEMPLATE	=	app
TARGET		=	SummaryForm
MOC_DIR		=	./moc
RCC_DIR		=	./moc
OBJECTS_DIR	=	./objects
DESTDIR     =	./release
INCLUDEPATH	+= ../MetaObjects/

RESOURCES += resources.qrc
RC_FILE = mainicon.rc

HEADERS +=	../MetaObjects/DBConnection.h \
		../MetaObjects/DBConnectionDialog.h \
		../MetaObjects/ProgramConfig.h \
		../MetaObjects/ReconnectingThread.h \
		../MetaObjects/GlobalTypes.h \
		MainWindow.h \
		FeaturesTableModel.h

SOURCES +=	../MetaObjects/DBConnection.cpp \
		../MetaObjects/DBConnectionDialog.cpp \
		../MetaObjects/ProgramConfig.cpp \
		../MetaObjects/ReconnectingThread.cpp \
		../MetaObjects/GlobalTypes.cpp \
		main.cpp \
		MainWindow.cpp \
		FeaturesTableModel.cpp
