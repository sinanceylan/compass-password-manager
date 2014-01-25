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

#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include <QDialog>
#include <QtSql>

#include <QDataWidgetMapper>
#include <QMessageBox>

namespace Ui {
    class CategoryDialog;
}

enum {
    Category_Id = 0,
    Category_Title = 1,
    Category_Description = 2,
    Category_ParentId = 3,
    Category_Position = 4,
    Category_Icon = 5
};

class CategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CategoryDialog(int catId, QSqlTableModel *model, QWidget *parent = 0);
    ~CategoryDialog();

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::CategoryDialog *ui;

    QSqlTableModel *tableModel;

    QDataWidgetMapper *mapper;

    bool editMode;

    int categoryId;

    bool addCategory();
};

#endif // CATEGORYDIALOG_H
