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

#include "passworddialog.h"
#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QString _connectionName, QString _action, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    action = _action;
    ui->setupUi(this);
    ui->oldPasswordEdit->setVisible(false);
    ui->oldPasswordLabel->setVisible(false);
    ui->okButton->setText(tr("Create &File"));
    setWindowTitle("Create New Database");

    if (action == "change_password") {
        ui->oldPasswordLabel->setVisible(true);
        ui->oldPasswordEdit->setVisible(true);
        ui->passwordLabel->setText(tr("New &Password"));
        ui->okButton->setText(tr("Change &Master Password"));
        ui->titleLabel->setText(tr("Change Master Password"));
        setWindowTitle(tr("Change Master Password"));
    }

    layout()->setSizeConstraint(QLayout::SetFixedSize);
    connectionName = _connectionName;
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

void PasswordDialog::on_cancelButton_clicked()
{
    this->close();
}

void PasswordDialog::on_showPasswordCheckBox_toggled(bool checked)
{
    if (checked) {
        ui->oldPasswordEdit->setEchoMode(QLineEdit::Normal);
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
        ui->repeatPasswordEdit->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->oldPasswordEdit->setEchoMode(QLineEdit::Password);
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
        ui->repeatPasswordEdit->setEchoMode(QLineEdit::Password);
    }
}

void PasswordDialog::on_okButton_clicked()
{
    if (ui->passwordEdit->text().length() < 6) {
        QMessageBox::critical(0, tr("This password is too short"),
                              tr("This password is too short. Please choose one that is at least six characters. "
                                 ), QMessageBox::Close);
    }
    else if (ui->passwordEdit->text() != ui->repeatPasswordEdit->text()) {
        QMessageBox::critical(0, tr("Password does not match"),
                              tr("Password does not match. Please check and re-enter password."
                                 ), QMessageBox::Close);
    }
    else if (action == "new_password") {
        QString fileName = createNewDatabase("compass");


        QFile *file = new QFile;
        if (file->exists(fileName)) {
            if (file->remove(fileName)) {
                qDebug() << "DB file overwritten:" << fileName;
            }
        }

        if (!fileName.isEmpty()) {
            createDatabase(fileName, connectionName);
        }
        QDialog::accept();
    }
    else if (action == "change_password"){
        QSqlDatabase db = QSqlDatabase::database("default_connection");

        QSqlQuery query(db);
        query.prepare("SELECT id, username, password, address FROM entry");

        if (query.exec()) {
            db.transaction();
            QSqlQuery updateQuery(db);
            bool commit = true;
            QString id;

            while (query.next()) {
                id = query.value(0).toString();

                QString username = query.value(1).toString();
                QString password = query.value(2).toString();
                QString address = query.value(3).toString();

                QString dec_username = do256(ui->oldPasswordEdit->text(), username, false);
                QString dec_password = do256(ui->oldPasswordEdit->text(), password, false);
                QString dec_address = do256(ui->oldPasswordEdit->text(), address, false);

                if (dec_password != "_wrong_password" && dec_address != "_wrong_password" && dec_username != "_wrong_password") {
                    QString enc_username = do256(ui->passwordEdit->text(), dec_username, true);
                    QString enc_password = do256(ui->passwordEdit->text(), dec_password, true);
                    QString enc_address = do256(ui->passwordEdit->text(), dec_address, true);
                    if (updateQuery.exec(QString("UPDATE entry SET username = '%1', password = '%2', address = '%3' WHERE id = %4").arg(enc_username, enc_password, enc_address, id)) == false) {
                        commit = false;
                        break;
                    }
                }
                else {
                    commit = false;
                    break;
                }
            }

            if (commit) {
                db.commit();
                qDebug() << "commit";
                QMessageBox::information(0, tr("Password changed successfully"),
                                         tr("Master password changed successfully.")
                                         , QMessageBox::Close);
                QDialog::accept();
            }
            else {
                query.clear();
                db.rollback();
                qDebug() << "rollback";
                QMessageBox::critical(0, tr("An error occured"),
                                      QString(tr("An error occured while changing master password. ID = %1 does not encrypted with given password. No change has been made.")).arg(id)
                                      , QMessageBox::Close);
            }
        }
    }

}

QString PasswordDialog::getPasswordText()
{
    return ui->passwordEdit->text();
}

void PasswordDialog::on_genPassButton_clicked()
{
    GeneratePasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->passwordEdit->setText(dialog.getPassword());
        ui->repeatPasswordEdit->setText(ui->passwordEdit->text());
    }
}


void PasswordDialog::on_safeCheck_toggled(bool checked)
{
    if (checked) {
        ui->okButton->setEnabled(true);
    }
    else {
        ui->okButton->setEnabled(false);
    }
}
