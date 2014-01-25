/*
 *************************************************************************
 *                                                                       *
 * Copyright (C) 2011 - 2014, Sinan Ceylan  sinanceylan@gmail.com        *
 * All rights reserved.                                                  *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************
 */

#include "commonlib.h"

void newDatabaseConnection(QString connectionName)
{
    QSqlDatabase::addDatabase("QSQLITE", connectionName);
}

void closeDatabaseConnection(QString connectionName)
{
    {
        QSqlDatabase db = QSqlDatabase::database(connectionName, false);
        db.close();
        if (!db.isOpen()) {
            qDebug() << "Database Closed.";
        }
    }
    QSqlDatabase::removeDatabase(connectionName);


}

bool createDatabase(QString databaseName, QString connectionName)
{
    newDatabaseConnection(connectionName);
    if (openDatabase(databaseName, connectionName)) {
        qDebug() << "New database created.";
        if (connectionName == "default_connection") {
            createDefaultTables(connectionName);
        }
        else if (connectionName == "export_connection") {
            createExportTable(connectionName);
        }
        return true;
    }
    return false;
}

bool openDatabase(QString databaseName, QString connectionName)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName, false);

    if (db.isOpen())
        closeDatabaseConnection(connectionName);

    db.setDatabaseName(databaseName);

    if (!db.open()) {
        qDebug() << "Cannot connect database" << databaseName << db.lastError();
        return false;
    }
    qDebug() << "Database connection established." << databaseName;
    return true;
}

bool connectDatabase(QString databaseName, QString connectionName)
{
    newDatabaseConnection(connectionName);
    if (openDatabase(databaseName, connectionName)) {
        return true;
    }
    QSqlDatabase::removeDatabase(connectionName);
    return false;
}

void createDefaultTables(QString connectionName)
{
    QSqlQuery query(QSqlDatabase::database(connectionName));

    qDebug() << "Creating default tables.";
    qDebug() << query.exec("CREATE TABLE category ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "title VARCHAR(256) , "
                           "description VARCHAR(256) , "
                           "parentid INTEGER , "
                           "icon VARCHAR(256) )");

    //    Create entry table
    qDebug() << query.exec("CREATE TABLE entry ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "title VARCHAR(256) , "
                           "username VARCHAR(256) , "
                           "password VARCHAR(256) , "
                           "address VARCHAR(256) , "
                           "description VARCHAR(256) , "
                           "creationtime DATETIME , "
                           "lastmodtime DATETIME , "
                           "lastacctime DATETIME , "
                           "exptime DATETIME , "
                           "categoryid INTEGER , "
                           "icon VARCHAR(256) , "
                           "export INTEGER"
                           ")");
}

void createExportTable(QString connectionName)
{
    QSqlQuery query(QSqlDatabase::database(connectionName));

    qDebug() << "Creating export table.";
    qDebug() << query.exec("CREATE TABLE entry ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "title VARCHAR(256) , "
                           "username VARCHAR(256) , "
                           "password VARCHAR(256) , "
                           "address VARCHAR(256) , "
                           "description VARCHAR(256) , "
                           "creationtime DATETIME , "
                           "lastmodtime DATETIME , "
                           "lastacctime DATETIME , "
                           "exptime DATETIME , "
                           "categoryid INTEGER , "
                           "icon VARCHAR(256) , "
                           "export INTEGER"
                           ")");
}


void addTestData(QString connectionName)
{
    QSqlQuery query(QSqlDatabase::database(connectionName));
    qDebug() << query.exec("insert into category values(1, 'First', 'This is the first category.', 1, 'first.ico')");
    qDebug() << query.exec("insert into category values(2, 'Second', 'This is the second category.', 1, 'second.ico')");

    qDebug() << query.exec("insert into entry values(1, 'MyTitle', 'bond', 'SeCrET', '127.0.0.1', 'My localhost server.', '2007-01-01 10:00:00', '2008-01-01 10:00:00', '2009-01-01 10:00:00', '2010-01-01 10:00:00', 1, 'server.ico', 0)");
    qDebug() << query.exec("insert into entry values(2, 'MyTitle2', 'james', 'SeCrET', '127.0.0.1', 'My localhost server.', '2007-01-01 10:00:00', '2008-01-01 10:00:00', '2009-01-01 10:00:00', '2010-01-01 10:00:00', 1, 'server.ico', 0)");
    qDebug() <<  query.exec("insert into entry values(3, 'MyTitle3', 'gadget', 'SeCrET', '127.0.0.1', 'My localhost server.', '2007-01-01 10:00:00', '2008-01-01 10:00:00', '2009-01-01 10:00:00', '2010-01-01 10:00:00', 2, 'server.ico', 0)");
}

QString createNewDatabase(QString databaseName)
{
    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(0,
                                                    QObject::tr("Create New Database"),
                                                    QString("%1.cdb").arg(databaseName),
                                                    QObject::tr("Compass Database (*.cdb);;All Files (*)"),
                                                    &selectedFilter,
                                                    options);
    return fileName;
}

int randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

QString generatePassword(int length)
{
    QString passGenerated;
    QString passOption;
    int max, intGen;

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    passOption += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    passOption += "abcdefghijklmnopqrstuvwxyz";
    passOption += "0123456789";
    passOption += "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

    max = passOption.size();

    for (int i = 0; i < length ; i++)
    {
        intGen = (qrand() % max);
        passGenerated[i] = passOption[intGen];
    }
    return passGenerated;
}

QString do256(QString Pass, QString Text, bool encrypt)
{
    Pass = sha256(Pass);
    return aes256(Pass, Text, encrypt);
}

QString aes256(QString Pass, QString Text, bool encrypt)
{    
    string text = Text.toStdString();
    string pass = Pass.toStdString();
    string CipherText;
    string RecoveredText;

    Pass.clear();
    Text.clear();

    byte key[ AES::DEFAULT_KEYLENGTH ], iv[ AES::BLOCKSIZE ];
    StringSource( reinterpret_cast<const char*>(pass.c_str()), true, new HashFilter(*(new SHA256), new ArraySink(key, AES::DEFAULT_KEYLENGTH)) );
    memset( iv, 0x00, AES::BLOCKSIZE );

    if (encrypt)
    {
        CBC_Mode<AES>::Encryption Encryptor( key, sizeof(key), iv );
        StringSource( text, true, new StreamTransformationFilter( Encryptor,new HexEncoder(new StringSink( CipherText ) ) ) );
    }
    else
    {
        try
        {
            CBC_Mode<AES>::Decryption Decryptor( key, sizeof(key), iv );
            StringSource( text, true, new HexDecoder(new StreamTransformationFilter( Decryptor, new StringSink( RecoveredText ) ) ) );
        }
        catch ( Exception& e)
        {
            return "_wrong_password";
        }
        catch (...)
        {
            return "_wrong_password";
        }
    }

    if (encrypt)
        return stringToQString(CipherText);

    return stringToQString(RecoveredText);
}

QString sha256(QString Message)
{
    SHA256 Sha256Hash;
    byte Sha256Digest[ SHA256::DIGESTSIZE ];
    string message = Message.toStdString();

    Sha256Hash.CalculateDigest( Sha256Digest, (byte*) message.c_str(), message.length() );

    HexEncoder encoder;
    string Sha256Output;
    encoder.Attach( new StringSink( Sha256Output ) );
    encoder.Put( Sha256Digest, sizeof(Sha256Digest) );
    encoder.MessageEnd();

    return stringToQString(Sha256Output);
}

QString stringToQString(std::string text)
{
    return QString::fromUtf8(text.c_str());
}


