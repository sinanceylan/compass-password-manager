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

#include "importdialog.h"
#include "ui_importdialog.h"

ImportDialog::ImportDialog(QString password, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Import"));

    ui->importButton->setEnabled(false);

    entryModel = new QSqlTableModel(this, QSqlDatabase::database("default_connection"));
    entryModel->setTable("entry");
    entryModel->select();

    categoryModel = new QSqlTableModel(this, QSqlDatabase::database("default_connection"));
    categoryModel->setTable("category");
    ui->categoryComboBox->setModel(categoryModel);
    ui->categoryComboBox->setModelColumn(categoryModel->fieldIndex("title"));
    categoryModel->select();

    masterPassword = password;
}

ImportDialog::~ImportDialog()
{
    delete ui;
    closeDatabaseConnection("import_connection");
}

void ImportDialog::displayImportViewContextMenu(const QPoint &point)
{
    QMenu cellMenu(this);
    QList<QAction *> cellMenuActionList;
    cellMenuActionList.append(ui->addToImportListAction);
    cellMenuActionList.append(ui->removeFromImportListAction);
    cellMenu.addActions(cellMenuActionList);

    QModelIndex index = ui->importView->indexAt(point);

    if (index.isValid()) {
        cellMenu.exec(ui->importView->viewport()->mapToGlobal(point));
    }
}

void ImportDialog::on_openFileButton_clicked()
{
    //    newDatabaseConnection("import");
    OpenDbPasswordDialog dialog("import_connection", this);

    if (dialog.exec() == QDialog::Accepted) {
        tableModel = new QSqlTableModel(this, QSqlDatabase::database("import_connection"));
        tableModel->setTable("entry");
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
        tableModel->setHeaderData(Entry_Export, Qt::Horizontal, tr("Import"));

        ui->importView->setModel(tableModel);
        ui->importView->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->importView->setSelectionBehavior(QAbstractItemView::SelectRows);

        ui->importView->horizontalHeader()->setStretchLastSection(true);
        ui->importView->horizontalHeader()->setVisible(true);

        ui->importView->setColumnHidden(Entry_Id, true);
        ui->importView->setColumnHidden(Entry_Username, true);
        ui->importView->setColumnHidden(Entry_Password, true);
        ui->importView->setColumnHidden(Entry_Address, true);
        ui->importView->setColumnHidden(Entry_Icon, true);
        ui->importView->setColumnHidden(Entry_CreationTime, true);
        ui->importView->setColumnHidden(Entry_LastModTime, true);
        ui->importView->setColumnHidden(Entry_LastAccTime, true);
        ui->importView->setColumnHidden(Entry_ExpTime, true);

        ui->importView->setContextMenuPolicy(Qt::CustomContextMenu);

        connect(ui->importView,
                SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(displayImportViewContextMenu(QPoint)));

        importPassword = dialog.getPasswordText();
    }
}

void ImportDialog::on_closeButton_clicked()
{
    this->close();
}

void ImportDialog::on_removeFromImportListAction_triggered()
{
    QModelIndex index = ui->importView->currentIndex();
    if (index.isValid()) {
        tableModel->setData(tableModel->index(index.row(),Entry_Export), 0);
        tableModel->submitAll();
    }
}

void ImportDialog::on_addToImportListAction_triggered()
{
    QModelIndex index = ui->importView->currentIndex();
    if (index.isValid()) {
        tableModel->setData(tableModel->index(index.row(),Entry_Export), 1);
        tableModel->submitAll();
    }
}

void ImportDialog::on_importButton_clicked()
{
    ui->importButton->setEnabled(false);
    importList();
}

bool ImportDialog::importList()
{
    QSqlQuery query(QSqlDatabase::database("default_connection"));
    query.exec(QString("SELECT id FROM category WHERE title='%1'").arg(ui->categoryComboBox->currentText()));

    int categoryId = 1;
    while(query.next())
        categoryId = query.value(0).toInt();


    QSortFilterProxyModel *filterModel = new QSortFilterProxyModel();
    filterModel->setSourceModel(tableModel);

    filterModel->setFilterKeyColumn(Entry_Export);
    filterModel->setFilterFixedString(QString::number(1));

    QVariant value;
    for (int row = 0; row < filterModel->rowCount(); row++) {

        int rowCount = entryModel->rowCount();
        entryModel->insertRow(rowCount);

        for(int column = 1; column < filterModel->columnCount(); column++) {

            if (column == Entry_Username || column == Entry_Password || column == Entry_Address) {
                value = filterModel->data(filterModel->index(row, column));
                value = do256(importPassword, value.toString(), false);
                value = aes256(masterPassword, value.toString(), true);
            }
            else if (column == Entry_CategoryId) {
                value = categoryId;
            }
            else if (column == Entry_Export) {
                value = 0;
            }
            else {
                value = filterModel->data(filterModel->index(row, column));
            }
            entryModel->setData(entryModel->index(rowCount, column), value);
        }
        entryModel->submitAll();
    }
    return true;
}

void ImportDialog::on_categoryComboBox_currentIndexChanged(const QString &arg1)
{
    if (! arg1.isEmpty()) {
        ui->importButton->setEnabled(true);
    }
}
