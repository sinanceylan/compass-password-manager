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

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QMenu>

#include "opendbpassworddialog.h"
#include "entrydialog.h"

namespace Ui {
    class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QString password, QWidget *parent);
    ~ImportDialog();

private slots:
    void on_openFileButton_clicked();

    void on_closeButton_clicked();

    void on_removeFromImportListAction_triggered();

    void on_addToImportListAction_triggered();

    void displayImportViewContextMenu(const QPoint &point);

    void on_importButton_clicked();

    void on_categoryComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::ImportDialog *ui;

    QSqlTableModel *tableModel;

    QSqlTableModel *categoryModel;

    QSqlTableModel *entryModel;

    QString masterPassword;

    QString importPassword;

    bool importList();
};

#endif // IMPORTDIALOG_H
