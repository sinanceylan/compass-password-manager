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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QProcess>
#include <QKeyEvent>
#include <QDataWidgetMapper>
#include <QSystemTrayIcon>
#include <QAction>

#include "commonlib.h"
#include "passworddialog.h"
#include "entrydialog.h"
#include "categorydialog.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "optionsdialog.h"
#include "generatepassworddialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setVisible(bool visible);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void updateEntryView();

    void displayEntryViewContextMenu(const QPoint &point);

    void displayCategoryViewContextMenu(const QPoint &point);

    void on_exitAction_triggered();

    void on_openDatabaseAction_triggered();

    void on_newDatabaseAction_triggered();

    void on_addEntryAction_triggered();

    void on_removeEntryAction_triggered();

    void on_editEntryAction_triggered();

    void on_entryView_doubleClicked(const QModelIndex &index);

    void on_copyUsernameAction_triggered();

    void on_copyPasswordAction_triggered();

    void on_editCategoryAction_triggered();

    void on_addCategoryAction_triggered();

    void on_categoryView_doubleClicked(const QModelIndex &index);

    void on_removeCategoryAction_triggered();

    void on_searchEdit_textChanged(const QString &arg1);

    void on_clearButton_clicked();

    void on_findInDatabaseAction_triggered();

    void on_aboutAction_triggered();

    void on_addToExportListAction_triggered();

    void on_removeFromExportListAction_triggered();

    void on_importDatabaseAction_triggered();

    void on_exportDatabaseAction_triggered();

    void on_closeDatabaseAction_triggered();

    void on_changeMasterPasswordAction_triggered();

    void on_addCategoryToExportListAction_triggered();

    void on_removeCategoryFromExportListAction_triggered();

    void on_optionsAction_triggered();

    void on_passwordGeneratorAction_triggered();

    void on_copyEntryAction_triggered();

    void on_pasteEntryAction_triggered();

    void quitApplication();

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;

    QSqlRelationalTableModel *model;

    QDataWidgetMapper *mapper; //QtGui

    QSqlTableModel *categoryModel;

    QSqlRelationalTableModel *entryModel;

    QString termApplication;

    QString httpApplication;

    QString vncApplication;

    QString rdpApplication;

    QString masterPassword;

    QString tempPass;

//    int cycle;

//    QSqlRecord copyRecord;
    int rowIndex;

    QString recentFile;

    int catIndex;

    QAction *minimizeAction;

    QAction *maximizeAction;

    QAction *restoreAction;

    QAction *quitAction;

    QSystemTrayIcon *trayIcon;

    QMenu *trayIconMenu;

    int showDeleteConfirmationBox(QString title);

    void writeSettings();

    void readSettings();

    void createCategoryModelView();

    void createEntryModelView();

    void updateCategoryView();

    bool eventFilter(QObject* obj, QEvent *event);

    QString openDbFile();

    QString cryptPass(QString Pass);

    QString decryptPass();

    void createActions();

    void createTrayIcon();

};

#endif // MAINWINDOW_H
