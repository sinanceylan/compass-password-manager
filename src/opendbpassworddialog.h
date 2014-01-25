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

#ifndef OPENDBPWDLG_H
#define OPENDBPWDLG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "commonlib.h"

namespace Ui {
    class OpenDbPasswordDialog;
}

class OpenDbPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenDbPasswordDialog(QString conName, QWidget *parent = 0);
    ~OpenDbPasswordDialog();

    QString getPasswordText();

private slots:
    void on_cancelButton_clicked();

    void on_openFileButton_clicked();

    void on_okButton_clicked();

    void on_passwordEdit_textChanged(const QString &arg1);

    void on_passwordEdit_returnPressed();

    void on_showPasswordCheckBox_toggled(bool checked);

private:
    Ui::OpenDbPasswordDialog *ui;

    QString fileName;

    QString connectionName;

    QString openDbFile();

    bool checkPassword();
};

#endif // OPENDBPWDLG_H
