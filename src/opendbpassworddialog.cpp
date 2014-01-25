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

#include "opendbpassworddialog.h"
#include "ui_opendbpassworddialog.h"

OpenDbPasswordDialog::OpenDbPasswordDialog(QString conName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenDbPasswordDialog)
{
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    connectionName = conName;

    QSettings settings("compass", "compass");
    QString recentFile = settings.value("recent_file", "").toString();

    if (! recentFile.isEmpty()) {
        fileName = recentFile;
        ui->fileNameEdit->setText(recentFile);
        qDebug() << "File selected:" << fileName;
    }
}

OpenDbPasswordDialog::~OpenDbPasswordDialog()
{
    delete ui;
}

void OpenDbPasswordDialog::on_cancelButton_clicked()
{
    this->close();
}

QString OpenDbPasswordDialog::openDbFile()
{
    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open Password Database"),
                                            "compass.cdb",
                                            tr("Compass Database (*.cdb);;All Files (*)"),
                                            &selectedFilter,
                                            options);
    return fileName;
}

void OpenDbPasswordDialog::on_openFileButton_clicked()
{
    fileName = openDbFile();

    if (!fileName.isEmpty()) {
        ui->fileNameEdit->setText(fileName);
        ui->passwordEdit->setFocus();
        qDebug() << "File selected:" << fileName;
    }
}

void OpenDbPasswordDialog::on_okButton_clicked()
{
    if (ui->fileNameEdit->text().isEmpty()) {
        QMessageBox::critical(0, tr("Please select a file"),
                              tr("Please select a database file."
                                 ), QMessageBox::Close);
        ui->openFileButton->setFocus();
    }
    else if (ui->passwordEdit->text().isEmpty()) {
        QMessageBox::critical(0, tr("Password field can not be empty."),
                              tr("Password field can not be empty!"
                                 ), QMessageBox::Close);
        ui->passwordEdit->selectAll();
        ui->passwordEdit->setFocus();
    }
    else {
        qDebug() << connectionName;
        QSqlDatabase::database(connectionName).close();
        if (connectDatabase(ui->fileNameEdit->text(), connectionName)) {
            if (checkPassword()) {
                QSettings settings("compass", "compass");
                settings.setValue("recent_file", ui->fileNameEdit->text());
                QDialog::accept();
            }
            else {
                closeDatabaseConnection(connectionName);
            }
        }
    }
}

bool OpenDbPasswordDialog::checkPassword()
{
    QSqlQuery query(QSqlDatabase::database(connectionName));

    if (query.exec("SELECT COUNT(*) FROM entry")) {
        while(query.next()) {
            if (query.value(0).toInt() < 1) {
                query.clear();

                QMessageBox messageBox;
                messageBox.setIcon(QMessageBox::Question);
                messageBox.setWindowTitle(tr("Empty Database"));
                messageBox.setText(tr("Database is empty. No password has been defined."));
                messageBox.setInformativeText(tr("Would you like to use this password as your master password?"));
                messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                messageBox.setDefaultButton(QMessageBox::No);
                int m = messageBox.exec();

                if (m == QMessageBox::Yes) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
    }

    if (query.exec("SELECT password FROM entry LIMIT 1")) {
        while (query.next()) {
            if (do256(ui->passwordEdit->text(), query.value(0).toString(), false) == "_wrong_password") {
                QMessageBox::critical(0, tr("Wrong Password"),
                                      tr("Password is wrong."
                                         ), QMessageBox::Close);
                return false;
            }
            return true;
        }
    }
    return false;
}

void OpenDbPasswordDialog::on_passwordEdit_textChanged(const QString & arg1)
{
    Q_UNUSED(arg1);
    ui->headerLabel->setText(tr("Enter Master Password:"));
}

void OpenDbPasswordDialog::on_passwordEdit_returnPressed()
{
    on_okButton_clicked();
}

void OpenDbPasswordDialog::on_showPasswordCheckBox_toggled(bool checked)
{
    if (checked) {
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
    }
}

QString OpenDbPasswordDialog::getPasswordText()
{
    return ui->passwordEdit->text();
}
