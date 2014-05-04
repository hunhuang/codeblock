ECHO OFF 
moc threadReadingData.h -o moc_threadReadingData.cpp
moc mainwindow.h -o moc_mainwindow.cpp
moc glwidget.h -o moc_glwidget.cpp
moc htmlmanual.h -o moc_HTMLmanual.cpp
ECHO All done with moc
