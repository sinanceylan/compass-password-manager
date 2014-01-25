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

#ifndef ENTRYDIALOG_H
#define ENTRYDIALOG_H

#include <QDialog>
#include <QtSql>
//#include <QtGui>
#include <QDateTime>
#include <QDataWidgetMapper>
#include "commonlib.h"
#include "generatepassworddialog.h"


#include "proxysqlrelationaldelegate.h"

namespace Ui {
    class EntryDialog;
}

enum {
    Entry_Id = 0,
    Entry_Title = 1,
    Entry_Username = 2,
    Entry_Password = 3,
    Entry_Address = 4,
    Entry_Description = 5,
    Entry_CreationTime = 6,
    Entry_LastModTime = 7,
    Entry_LastAccTime = 8,
    Entry_ExpTime = 9,
    Entry_CategoryId = 10,
    Entry_Icon = 11,
    Entry_Export = 12
};

class EntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EntryDialog(int id, int catId, QString catTitle, QString password, QString connectionName, QWidget *parent = 0);
    ~EntryDialog();

private slots:
    void on_showPasswordCheckBox_toggled(bool checked);

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_generatePasswordButton_clicked();

private:
    Ui::EntryDialog *ui;

    QSqlRelationalTableModel *tableModel;

    QDataWidgetMapper *mapper;

    bool editMode;

    int categoryId;

    int entryId;

    QString masterPassword;

    bool addEntry();
};

#endif // ENTRYDIALOG_H
