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

#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QString password, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Export"));

    tableModel = new QSqlRelationalTableModel(this, QSqlDatabase::database("default_connection"));
    tableModel->setTable("entry");
    tableModel->setRelation(Entry_CategoryId,
                            QSqlRelation("category", "id", "title"));
    tableModel->select();

    tableModel->setSort(Entry_Id, Qt::AscendingOrder);

    tableModel->setHeaderData(Entry_Title, Qt::Horizontal, tr("Title"));
    tableModel->setHeaderData(Entry_Username, Qt::Horizontal, tr("Username"));
    tableModel->setHeaderData(Entry_Address, Qt::Horizontal, tr("Address"));
    tableModel->setHeaderData(Entry_Description, Qt::Horizontal, tr("Description"));
    tableModel->setHeaderData(Entry_CreationTime, Qt::Horizontal, tr("Creation Time"));
    tableModel->setHeaderData(Entry_LastModTime, Qt::Horizontal, tr("Last Mod. Time"));
    tableModel->setHeaderData(Entry_LastAccTime, Qt::Horizontal, tr("Last Acc. Time"));
    tableModel->setHeaderData(Entry_ExpTime, Qt::Horizontal, tr("Exp. Time"));
    tableModel->setHeaderData(Entry_CategoryId, Qt::Horizontal, tr("Category"));
    tableModel->setHeaderData(Entry_Export, Qt::Horizontal, tr("Export"));

    tableModel->setFilter("entry.export = 1");


    ui->exportView->setModel(tableModel);
    ui->exportView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->exportView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->exportView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->exportView->horizontalHeader()->setStretchLastSection(true);
    ui->exportView->horizontalHeader()->setVisible(true);


    ui->exportView->setColumnHidden(Entry_Id, true);
    ui->exportView->setColumnHidden(Entry_Username, true);
    ui->exportView->setColumnHidden(Entry_Password, true);
    ui->exportView->setColumnHidden(Entry_Address, true);
    ui->exportView->setColumnHidden(Entry_Icon, true);
    ui->exportView->setColumnHidden(Entry_CreationTime, true);
    ui->exportView->setColumnHidden(Entry_LastModTime, true);
    ui->exportView->setColumnHidden(Entry_LastAccTime, true);
    ui->exportView->setColumnHidden(Entry_ExpTime, true);
//    ui->exportView->setColumnHidden(Entry_Export, true);

    ui->exportView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->exportView,
            SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(displayExportViewContextMenu(QPoint)));

    masterPassword = password;
}

ExportDialog::~ExportDialog()
{
    delete ui;
    closeDatabaseConnection("export_connection");
}

void ExportDialog::displayExportViewContextMenu(const QPoint &point)
{
    QMenu cellMenu(this);
    QList<QAction *> cellMenuActionList;
    cellMenuActionList.append(ui->removeFromExportListAction);
    cellMenu.addActions(cellMenuActionList);

    QModelIndex index = ui->exportView->indexAt(point);

    if (index.isValid()) {
        cellMenu.exec(ui->exportView->viewport()->mapToGlobal(point));
    }
}

void ExportDialog::on_closeButton_clicked()
{
    this->close();
}

void ExportDialog::on_saveAsButton_clicked()
{
    ui->saveAsButton->setEnabled(false);

    if (exportList() == true)
        QDialog::accept();
    else
        ui->saveAsButton->setEnabled(true);
}

bool ExportDialog::exportList()
{
    if(tableModel->rowCount() == 0) {
        QMessageBox::critical(0, tr("Empty List"),
                              tr("Export list is empty."
                                 ), QMessageBox::Close);
        return false;
    }
    if (ui->passwordEdit->text().length() < 6) {
        QMessageBox::critical(0, tr("This password is too short"),
                              tr("This password is too short. Please choose one that is at least six characters."
                                 ), QMessageBox::Close);
        return false;
    }
    else if (ui->passwordEdit->text() != ui->repeatPasswordEdit->text()) {
        QMessageBox::critical(0, tr("Password does not match"),
                              tr("Password does not match. Please check and re-enter password."
                                 ), QMessageBox::Close);
        return false;
    }
    else {
#ifdef Q_OS_LINUX
        QString userName = getenv("USER");
#endif
#ifdef Q_WS_WIN
        QString userName = getenv("USERNAME");
#endif
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString fileName = createNewDatabase(QString("%1-export-%2").arg(userName).arg(currentDateTime.toString("ddMMyy-hhmm")));

        QFile *file = new QFile;
        if (file->exists(fileName)) {
            if (file->remove(fileName)) {
                qDebug() << "DB file overwritten:" << fileName;
            }
        }

        if (!fileName.isEmpty()) {
            if (createDatabase(fileName, "export_connection")) {
                QSqlTableModel *exportModel = new QSqlTableModel(this, QSqlDatabase::database("export_connection"));
                exportModel->setTable("entry");

                QVariant value;
                for (int row = 0; row < tableModel->rowCount(); row++) {
                    exportModel->insertRow(row);
                    for(int column = 1; column < tableModel->columnCount(); column++) {
                        value = tableModel->data(tableModel->index(row,column));
                        if (column == Entry_Username || column == Entry_Password || column == Entry_Address) {
                            value = aes256(masterPassword, value.toString(), false);
                            value = do256(ui->passwordEdit->text(), value.toString(), true);                            
                        }
                        exportModel->setData(exportModel->index(row,column), value);
                    }
                exportModel->submitAll();
                }

            }
        }
    }
    return false;
}

void ExportDialog::on_generatePasswordButton_clicked()
{
    GeneratePasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->passwordEdit->setText(dialog.getPassword());
        ui->repeatPasswordEdit->setText(ui->passwordEdit->text());
    }
}

void ExportDialog::on_showPasswordCheckBox_toggled(bool checked)
{
    if (checked) {
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
        ui->repeatPasswordEdit->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
        ui->repeatPasswordEdit->setEchoMode(QLineEdit::Password);
    }
}

void ExportDialog::on_removeFromExportListAction_triggered()
{
    QModelIndex index = ui->exportView->currentIndex();
    if (index.isValid()) {
        tableModel->setData(tableModel->index(index.row(),Entry_Export), 0);
        tableModel->setData(index, QColor(Qt::red), Qt::BackgroundRole);
        tableModel->submitAll();
    }
}
