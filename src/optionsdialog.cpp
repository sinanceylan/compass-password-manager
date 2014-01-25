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

#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Options"));

    readSettings();

    completer = new QCompleter(this);
    completer->setMaxVisibleItems(10);


    // FileSystemModel that shows full paths
    FileSystemModel *fsModel = new FileSystemModel(completer);
    fsModel->setRootPath("");

    completer->setModel(fsModel);
    ui->termEdit->setCompleter(completer);
    ui->httpEdit->setCompleter(completer);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

QString OptionsDialog::selectFile(QString currentFile)
{
    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                            tr("Select File"),
                                            QString("%1").arg(currentFile),
                                            tr("All Files (*)"),
                                            &selectedFilter,
                                            options);
    return fileName;
}

void OptionsDialog::on_cancelButton_clicked()
{
    this->close();
}

void OptionsDialog::on_termSelectFileButton_clicked()
{
    QString fileName = selectFile(ui->termEdit->text());
    if (!fileName.isEmpty()) {
        ui->termEdit->setText(fileName);
    }
}

void OptionsDialog::on_httpSelectFileButton_clicked()
{
    QString fileName = selectFile(ui->httpEdit->text());
    if (!fileName.isEmpty()) {
        ui->httpEdit->setText(fileName);
    }
}

void OptionsDialog::on_okButton_clicked()
{
    writeSettings();
    QDialog::accept();
}

void OptionsDialog::writeSettings()
{
    QSettings settings("compass", "compass");
    settings.setValue("term_application", ui->termEdit->text());
    settings.setValue("http_application", ui->httpEdit->text());
}

void OptionsDialog::readSettings()
{
    QSettings settings("compass", "compass");

    ui->termEdit->setText(settings.value("term_application", "/usr/bin/konsole -e").toString());
    ui->httpEdit->setText(settings.value("http_application", "/usr/bin/firefox").toString());
}
