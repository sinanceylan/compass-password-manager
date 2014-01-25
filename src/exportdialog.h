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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QtSql>
//#include <QtGui>
//#include <QtWidgets>
#include <QMenu>

#include "entrydialog.h"
#include "commonlib.h"

namespace Ui {
    class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QString password, QWidget *parent);
    ~ExportDialog();

private slots:
    void on_closeButton_clicked();

    void on_saveAsButton_clicked();

    void on_generatePasswordButton_clicked();

    void on_showPasswordCheckBox_toggled(bool checked);

    void displayExportViewContextMenu(const QPoint &point);

    void on_removeFromExportListAction_triggered();

private:
    Ui::ExportDialog *ui;

    QSqlRelationalTableModel *tableModel;

    QString masterPassword;

    bool exportList();

};

#endif // EXPORTDIALOG_H
