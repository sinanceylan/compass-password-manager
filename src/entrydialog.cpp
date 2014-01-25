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

#include "entrydialog.h"
#include "ui_entrydialog.h"


EntryDialog::EntryDialog(int id, int catId, QString catTitle, QString password, QString connectionName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryDialog)
{
    ui->setupUi(this);
    entryId = id;

    tableModel = new QSqlRelationalTableModel(this, QSqlDatabase::database(connectionName));
    tableModel->setTable("entry");
    tableModel->setRelation(Entry_CategoryId,
                            QSqlRelation("category", "id", "title"));
    tableModel->select();

    QSqlTableModel *relationModel = tableModel->relationModel(Entry_CategoryId);
    ui->categoryComboBox->setModel(relationModel);
    ui->categoryComboBox->setModelColumn(relationModel->fieldIndex("title"));

    masterPassword = password;

    if (id == -1) {
        setWindowTitle(tr("Add New Entry"));
        editMode = false;
        categoryId = catId;
        ui->categoryComboBox->setEnabled(false);

        int index = ui->categoryComboBox->findText(catTitle);
        ui->categoryComboBox->setCurrentIndex(index);

        int row = tableModel->rowCount();
        tableModel->insertRow(row);
    }
    else {
        setWindowTitle(tr("Edit/View Entry"));
        editMode = true;

        QSortFilterProxyModel *filterModel = new QSortFilterProxyModel(parent);
        filterModel->setSourceModel(tableModel);

        filterModel->setFilterKeyColumn(Entry_Id);
        filterModel->setFilterRegExp(QRegExp(QString::number(id) , Qt::CaseInsensitive,
                                             QRegExp::FixedString));

        mapper = new QDataWidgetMapper(this);
        mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
        mapper->setModel(filterModel);
        mapper->setItemDelegate(new ProxySqlRelationalDelegate(this));
        mapper->addMapping(ui->categoryComboBox, Entry_CategoryId);
        mapper->addMapping(ui->titleEdit, Entry_Title);
        mapper->addMapping(ui->addressText, Entry_Address);
        mapper->addMapping(ui->usernameEdit, Entry_Username);
        mapper->addMapping(ui->passwordEdit, Entry_Password);
        mapper->addMapping(ui->descriptionText, Entry_Description);

        mapper->setCurrentIndex(0);

        ui->passwordEdit->setText(aes256(masterPassword, ui->passwordEdit->text(), false));
        ui->repeatPasswordEdit->setText(ui->passwordEdit->text());

        ui->usernameEdit->setText(aes256(masterPassword, ui->usernameEdit->text(), false));
        ui->addressText->setPlainText(aes256(masterPassword, ui->addressText->toPlainText(), false));
//        ui->descriptionText->setPlainText(aes256(masterPassword, ui->descriptionText->toPlainText(), false));


    }
}

EntryDialog::~EntryDialog()
{
    delete ui;
}

bool EntryDialog::addEntry()
{
    QString title = ui->titleEdit->text().trimmed();
    QString username = ui->usernameEdit->text();

    QString password = ui->passwordEdit->text();
    QString repeatPassword = ui->repeatPasswordEdit->text();

    if (password != repeatPassword) {
        QMessageBox::critical(0, tr("Password does not match"),
                              tr("Password does not match. Please check and re-enter password."),
                              QMessageBox::Close);
        return false;
    }

    if (title.length() == 0) {
        QMessageBox::critical(0, tr("Title Empty"),
                              tr("Title can not be empty!"),
                              QMessageBox::Close);
        return false;
    }

    if (username.length() == 0) {
        QMessageBox::critical(0, tr("Username Empty"),
                              tr("Username can not be empty!"),
                              QMessageBox::Close);
        return false;
    }

    if (password.length() == 0) {
        QMessageBox::critical(0, tr("Password Empty"),
                              tr("Password can not be empty!"),
                              QMessageBox::Close);
        return false;
    }

    ui->passwordEdit->setText(aes256(masterPassword, ui->passwordEdit->text(), true));
    ui->usernameEdit->setText(aes256(masterPassword, ui->usernameEdit->text(), true));
    ui->addressText->setPlainText(aes256(masterPassword, ui->addressText->toPlainText(), true));
//    ui->descriptionText->setPlainText(aes256(masterPassword, ui->descriptionText->toPlainText(), true));

    if (editMode) {
        ui->titleEdit->setText(title);
        mapper->submit();
        return true;
    }
    else { // Add mode.
        QDateTime currentDateTime = QDateTime::currentDateTime();
        int rowCount = tableModel->rowCount() - 1;

        tableModel->setData(tableModel->index(rowCount,Entry_Title), title);
        tableModel->setData(tableModel->index(rowCount,Entry_Username), ui->usernameEdit->text());
        tableModel->setData(tableModel->index(rowCount,Entry_Password), ui->passwordEdit->text());
        tableModel->setData(tableModel->index(rowCount,Entry_Address), ui->addressText->toPlainText());
        tableModel->setData(tableModel->index(rowCount,Entry_Description), ui->descriptionText->toPlainText());
        tableModel->setData(tableModel->index(rowCount,Entry_CreationTime), currentDateTime);
        tableModel->setData(tableModel->index(rowCount,Entry_LastModTime), currentDateTime);
        tableModel->setData(tableModel->index(rowCount,Entry_LastAccTime), currentDateTime);
        tableModel->setData(tableModel->index(rowCount,Entry_ExpTime), currentDateTime);
        tableModel->setData(tableModel->index(rowCount,Entry_CategoryId), categoryId);
        tableModel->setData(tableModel->index(rowCount,Entry_Icon), "");
        tableModel->setData(tableModel->index(rowCount,Entry_Export), 0);
        qDebug() << tableModel->submitAll();
        qDebug() << tableModel->lastError();
        qDebug() << tableModel->data(tableModel->index(tableModel->rowCount() - 1 ,Entry_CategoryId));
        qDebug() << tableModel->record(tableModel->rowCount() - 1);
        return true;

    }
    qDebug() << "False";
    return false;

}

void EntryDialog::on_showPasswordCheckBox_toggled(bool checked)
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

void EntryDialog::on_okButton_clicked()
{
    ui->okButton->setEnabled(false);

    if (addEntry() == true)
        QDialog::accept();
    else
        ui->okButton->setEnabled(true);
}

void EntryDialog::on_cancelButton_clicked()
{
    this->close();
}

void EntryDialog::on_generatePasswordButton_clicked()
{
    GeneratePasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->passwordEdit->setText(dialog.getPassword());
        ui->repeatPasswordEdit->setText(ui->passwordEdit->text());
    }
}
