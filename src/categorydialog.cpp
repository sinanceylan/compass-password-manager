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

#include "categorydialog.h"
#include "ui_categorydialog.h"

CategoryDialog::CategoryDialog(int catId, QSqlTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CategoryDialog)
{
    ui->setupUi(this);
    tableModel = model;
    categoryId = catId;

    if (catId == -1) {
        setWindowTitle(tr("Add New Category"));
        editMode = false;

        int row = tableModel->rowCount();
        tableModel->insertRow(row);
        qDebug() << tableModel->lastError();
    }
    else {
        setWindowTitle(tr("Edit/View Category"));
        editMode = true;

        QSortFilterProxyModel *filterModel = new QSortFilterProxyModel(parent);
        filterModel->setSourceModel(tableModel);

        filterModel->setFilterKeyColumn(Category_Id);
        filterModel->setFilterRegExp(QRegExp(QString::number(catId) , Qt::CaseInsensitive,
                                             QRegExp::FixedString));

        mapper = new QDataWidgetMapper(this);
        mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
        mapper->setModel(filterModel);
        mapper->addMapping(ui->titleEdit, Category_Title);
        mapper->addMapping(ui->descriptionText, Category_Description);
        mapper->setCurrentIndex(0);
    }
}

CategoryDialog::~CategoryDialog()
{
    delete ui;
}

bool CategoryDialog::addCategory()
{
    QString title = ui->titleEdit->text().trimmed();

    QSqlQuery query(QSqlDatabase::database(tableModel->database().connectionName()));
    query.prepare(QString("SELECT * FROM category WHERE title='%1'").arg(title));

    if (title.length() == 0) {
        QMessageBox::critical(0, tr("Title Empty"),
                              tr("Title can not be empty!"),
                              QMessageBox::Close);
        return false;
    }

    // To check if category already exist.
    if (query.exec()) {
        if ((! query.next()) || (query.value(0).toInt() == categoryId && editMode == true)) { // Category is NOT exist OR we are in edit mode.
            if (editMode) {
                ui->titleEdit->setText(title);
                mapper->submit();
                return true;
            }
            else { // Add mode.
                int rowCount = tableModel->rowCount() - 1;
                tableModel->setData(tableModel->index(rowCount,Category_Title), title);
                tableModel->setData(tableModel->index(rowCount,Category_Description), ui->descriptionText->toPlainText());
                tableModel->setData(tableModel->index(rowCount,Category_ParentId), 0);
                tableModel->setData(tableModel->index(rowCount,Category_Position), 0);
                tableModel->setData(tableModel->index(rowCount,Category_Icon), "");
                qDebug() << tableModel->submitAll();
                qDebug() << tableModel->lastError();
                return true;
            }
        }
        else { // Category exist.
            QMessageBox::critical(0, tr("Already Exist"),
                                  tr("%1 already exist!").arg(title),
                                  QMessageBox::Close);
            return false;
        }
    }
    return false;
}

void CategoryDialog::on_okButton_clicked()
{
    ui->okButton->setEnabled(false);

    if (addCategory() == true)
        QDialog::accept();
    else
        ui->okButton->setEnabled(true);
}

void CategoryDialog::on_cancelButton_clicked()
{
    this->close();
}
