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

#include "generatepassworddialog.h"
#include "ui_generatepassworddialog.h"

GeneratePasswordDialog::GeneratePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneratePasswordDialog)
{
    ui->setupUi(this);
    on_genButton_clicked();
}

GeneratePasswordDialog::~GeneratePasswordDialog()
{
    delete ui;
}

QString GeneratePasswordDialog::generatePassword()
{

    QString passGenerated;
    QString passOption;
    int max, intGen;

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    if (ui->upperCheck->isChecked())
        passOption += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (ui->lowerCheck->isChecked())
        passOption += "abcdefghijklmnopqrstuvwxyz";

    if (ui->numberCheck->isChecked())
        passOption += "0123456789";

    if (ui->specialCheck->isChecked())
        passOption += "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

    if (!passOption.isEmpty())
    {
        max = passOption.size();

        for (int i = 0; i < ui->lengthBox->value() ; i++)
        {
            intGen = (qrand() % max);
            passGenerated[i] = passOption[intGen];
        }
    }
    else
    {
        QMessageBox::critical(0, tr("Error"),
                             tr("Please select a password option"),
                             QMessageBox::Close);
    }
    return passGenerated;
}

void GeneratePasswordDialog::on_genButton_clicked()
{
    QString pass = generatePassword();
    if (! pass.isEmpty()) {
        ui->passwordEdit->setText(pass);
    }
}

void GeneratePasswordDialog::on_closeButton_clicked()
{
    this->close();
}

void GeneratePasswordDialog::on_copyButton_clicked()
{
    if (! ui->passwordEdit->text().isEmpty()) {
        QApplication::clipboard()->setText(ui->passwordEdit->text());
    }
}

QString GeneratePasswordDialog::getPassword()
{
    return ui->passwordEdit->text();
}

void GeneratePasswordDialog::on_okButton_clicked()
{
    return QDialog::accept();
}
