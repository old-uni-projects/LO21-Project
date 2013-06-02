SOURCES += \
    main.cpp \
    export/exportstrategy.cpp \
    export/exportstrategynotfoundexception.cpp \
    note/note.cpp \
    note/article.cpp \
    note/document.cpp \
    note/documentindexoutofarrayexception.cpp \
    note/multimedia.cpp \
    note/image.cpp \
    dbmanager/databasemanager.cpp \
    note/audio.cpp \
    note/video.cpp \
    export/exporthtml.cpp \
    export/exporttex.cpp \
    export/exporttext.cpp \
    notemanager/notesmanager.cpp \
	notefactory/notefactorynotfoundexception.cpp \
    notefactory/generalnotefactory.cpp \
    constants.cpp \
    dbmanager/dbexception.cpp \
    notemanager/notesmanagerexception.cpp \
    export/generalexportfactory.cpp \
    export/exportfactorynotfoundexception.cpp

HEADERS += \
    export/exportstrategy.h \
    export/exportstrategynotfoundexception.h \
    note/note.h \
    note/article.h \
    note/document.h \
    note/documentindexoutofarrayexception.h \
    note/multimedia.h \
    note/image.h \
    note/audio.h \
    note/video.h \
    note/classdef.h \
    dbmanager/databasemanager.h \
    export/exporthtml.h \
    export/exporttex.h \
    export/exporttext.h \
    notemanager/notesmanager.h \
    notefactory/notefactorynotfoundexception.h \
	notefactory/generalnotefactory.h \
    constants.h \
    notefactory/interfacenotefactory.h \
    notefactory/notefactory.h \
    dbmanager/dbexception.h \
    notemanager/notesmanagerexception.h \
	interfacestrategyfactory.h \
	strategyfactory.h \
    export/interfaceexportfactory.h \
    export/exportfactory.h \
    export/generalexportfactory.h \
    export/exportfactorynotfoundexception.h

FORMS +=

QT += widgets \
      sql

