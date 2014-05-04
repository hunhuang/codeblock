


#ifndef QTLOGO_H
#define QTLOGO_H


#include <qtcore/qobject>

#include <qtgui/qcolor>


class Patch;


struct Geometry;


class QtLogo : public QObject
{

public:

	QtLogo(QObject *parent, int d = 64, qreal s = 1.0);
    ~QtLogo();
    void setColor(QColor c);
    void draw() const;

private:

	void buildGeometry(int d, qreal s);

    QList<Patch *> parts;
    Geometry *geom;

};


#endif


