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

#ifndef COMMONLIB_H
#define COMMONLIB_H

#endif // COMMONLIB_H

#include <QDebug>
#include <QtSql>
//#include <QtGui>
#include <QDialog>
#include <QFileDialog>

#include <algorithm> //std::random_shuffle
//#include <qalgorithms.h>

#include "cryptopp/aes.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/modes.h" // xxx_Mode< >
#include "cryptopp/filters.h" // StringSource and StreamTransformation
#include "cryptopp/hex.h"
#include "cryptopp/sha.h"

using namespace std;
using namespace CryptoPP;

bool createDatabase(QString databaseName, QString connectionName);

bool unlockDatabase(QString databaseName, QString key, QString connectionName);

bool connectDatabase(QString databaseName, QString connectionName);

bool openDatabase(QString databaseName, QString connectionName);

void newDatabaseConnection(QString connectionName);

void closeDatabaseConnection(QString connectionName);

void createDefaultTables(QString connectionName);

void createExportTable(QString connectionName);

void addTestData(QString connectionName);

QString createNewDatabase(QString databaseName);

QString do256(QString Pass, QString Text, bool encrypt);

QString aes256(QString Pass, QString Text, bool encrypt);

QString sha256(QString Message);

int randInt(int low, int high);

QString generatePassword(int length);

QString stringToQString(std::string text);

