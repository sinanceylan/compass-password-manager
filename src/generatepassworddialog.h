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

#ifndef GENERATEPASSWORDDIALOG_H
#define GENERATEPASSWORDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QTime>
#include <QClipboard>

namespace Ui {
class GeneratePasswordDialog;
}

class GeneratePasswordDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GeneratePasswordDialog(QWidget *parent = 0);
    ~GeneratePasswordDialog();

    QString getPassword();
    
private slots:
    void on_genButton_clicked();

    void on_closeButton_clicked();

    void on_copyButton_clicked();

    void on_okButton_clicked();

private:
    Ui::GeneratePasswordDialog *ui;
    QString generatePassword();
};

#endif // GENERATEPASSWORDDIALOG_H
