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

#ifndef PasswordDialog_H
#define PasswordDialog_H

#include <QDialog>

#include <QMessageBox>
#include <QFile>

#include "commonlib.h"
#include "generatepassworddialog.h"

namespace Ui {
    class PasswordDialog;
}

class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(QString _connectionName, QString _action, QWidget *parent = 0);
    ~PasswordDialog();

    QString getPasswordText();

private slots:
    void on_cancelButton_clicked();

    void on_showPasswordCheckBox_toggled(bool checked);

    void on_okButton_clicked();

    void on_genPassButton_clicked();

    void on_safeCheck_toggled(bool checked);

private:
    Ui::PasswordDialog *ui;
    QString connectionName;
    QString action;
};

#endif // PasswordDialog_H
