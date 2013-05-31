#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QWidget>

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <iostream>

class databaseManager
{
public:
    /* *************** Constructeur ******************/
    databaseManager(QString filename = "temp", QString user = "", QString pass = "");

    /***************** Getters ***********************/
    const QString getpath() const;

    /******************* Setters *********************/

    /***************** DB REQUEST ********************/
    bool query(QString query) const;
    void getNote(unsigned int id) const;
    void getNote() const;
    bool deleteNote (unsigned int id) const;
    bool deleteNote () const;
    bool insertNote (QString titre, QString type) const;
    bool addType(QString type) const;

    /******************* Temp *************************/

    bool addType() const;

 private:
    QSqlDatabase *database;
    QString dbpath;

    bool initDB();

};

#endif // DATABASEMANAGER_H