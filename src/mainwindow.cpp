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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->searchEdit->installEventFilter(this);

    //  Set initial splitter position
    ui->mainSplitter->setStretchFactor(ui->mainSplitter->indexOf(ui->categoryView),1);
    ui->mainSplitter->setStretchFactor(ui->mainSplitter->indexOf(ui->entryView),2);

    readSettings();

    createCategoryModelView();
    createEntryModelView();
    updateEntryView();
    updateCategoryView();

    ui->entryView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->categoryView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->entryView,
            SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(displayEntryViewContextMenu(QPoint)));

    connect(ui->categoryView,
            SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(displayCategoryViewContextMenu(QPoint)));


    if (! recentFile.isEmpty()) {
        on_openDatabaseAction_triggered();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    closeDatabaseConnection("default_connection");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{

    QSettings settings("compass", "compass");
    QPoint mainWindowPosition = settings.value("main_window_position", QPoint(200, 200)).toPoint();
    QSize mainWindowSize = settings.value("main_window_size", QSize(600, 400)).toSize();
    ui->mainSplitter->restoreState(settings.value("main_splitter_state").toByteArray());

    termApplication = settings.value("term_application", "/usr/bin/konsole -e").toString();
    httpApplication = settings.value("http_application", "/usr/bin/firefox").toString();

    recentFile = settings.value("recent_file", "").toString();
    resize(mainWindowSize);
    move(mainWindowPosition);

    qDebug() << "Settings loaded.";
}

void MainWindow::writeSettings()
{
    QSettings settings("compass", "compass");
    settings.setValue("main_window_position", pos());
    settings.setValue("main_window_size", size());
    settings.setValue("main_splitter_state", ui->mainSplitter->saveState());

    qDebug() << "Settings written.";
}

void MainWindow::on_exitAction_triggered()
{
    this->close();
}

void MainWindow::on_newDatabaseAction_triggered()
{
    PasswordDialog dialog("default_connection", "new_password", this);
    if (dialog.exec() == QDialog::Accepted) {
        //        addTestData("default_connection");
        createCategoryModelView();
        createEntryModelView();
        updateEntryView();

        masterPassword = cryptPass(dialog.getPasswordText());
    }
}

QString MainWindow::openDbFile()
{
    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Password Database"),
                                                    "compass.cdb",
                                                    tr("Compass Database (*.cdb);;All Files (*)"),
                                                    &selectedFilter,
                                                    options);
    return fileName;
}

void MainWindow::on_openDatabaseAction_triggered()
{
    OpenDbPasswordDialog dialog("default_connection", this);

    if (dialog.exec() == QDialog::Accepted) {
        createCategoryModelView();
        createEntryModelView();
        updateEntryView();
        updateCategoryView();

        masterPassword = cryptPass(dialog.getPasswordText());
    }
    else {
        createCategoryModelView();
        createEntryModelView();
        updateEntryView();
        updateCategoryView();
    }
}

void MainWindow::on_addEntryAction_triggered()
{

    if (categoryModel->rowCount() > 0) {
        QModelIndexList selectedList = ui->categoryView->selectionModel()->selectedRows();
        int row;

        if (selectedList.length() == 0) {
            row = 0;
        }
        else {
            row = selectedList.at(0).row();
        }

        QModelIndex index = ui->categoryView->model()->index(row,Category_Id);
        int catId = ui->categoryView->model()->data(index).toInt();

        QModelIndex indexTitle = ui->categoryView->model()->index(row,Category_Title);
        QString catTitle = ui->categoryView->model()->data(indexTitle).toString();

        EntryDialog dialog(-1, catId, catTitle, decryptPass(), "default_connection", this);

        if(dialog.exec() == QDialog::Accepted) {
            updateEntryView();
        }
        else {
            entryModel->select();
            ui->entryView->setCurrentIndex(index);
        }
    }
    else {
        QMessageBox::warning(0, tr("Category Not Found"),
                             tr("Please add a category at first."),
                             QMessageBox::Close);
    }
}

void MainWindow::on_editEntryAction_triggered()
{
    int entryId = -1;
    int catId = -1;

    QModelIndex entryIndex = ui->entryView->currentIndex();
    QModelIndex categoryIndex = ui->categoryView->currentIndex();

    if (categoryIndex.isValid()) {
        QSqlRecord categoryRecord = categoryModel->record(categoryIndex.row());
        catId = categoryRecord.value(Category_Id).toInt();

        if (entryIndex.isValid()) {
            QSqlRecord entryRecord = entryModel->record(entryIndex.row());
            entryId = entryRecord.value(Entry_Id).toInt();

            EntryDialog dialog(entryId, catId, "", decryptPass(), "default_connection", this);
            if(dialog.exec() == QDialog::Accepted) {
                updateEntryView();
            }
            else {
                entryModel->select();
                ui->entryView->setCurrentIndex(entryIndex);
            }
        }
    }
}

void MainWindow::on_removeEntryAction_triggered()
{

    QModelIndex index = ui->entryView->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        QString entryTitle = entryModel->data(entryModel->index(row, Entry_Title)).toString();

        switch (showDeleteConfirmationBox(entryTitle)) {
        case QMessageBox::Yes:
            entryModel->removeRow(row);
            entryModel->submitAll();
            updateEntryView();
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
        }
    }
}

void MainWindow::on_addCategoryAction_triggered()
{
    QSqlQuery query(QSqlDatabase::database(categoryModel->database().connectionName()));
    query.prepare("SELECT * FROM category");

    if (query.exec()) { //To check if database open. Find better check method.
        CategoryDialog dialog(-1, categoryModel, this);
        QModelIndex index = ui->categoryView->currentIndex();
        if(dialog.exec() == QDialog::Accepted) {
            updateCategoryView();
        }
        else {
            categoryModel->select();
            ui->categoryView->setCurrentIndex(index);
        }
    }
    else {
        QMessageBox::warning(0, tr("Database Not Found"),
                             tr("Please open database or create a new one."),
                             QMessageBox::Close);
    }
}

void MainWindow::on_editCategoryAction_triggered()
{
    int catId = -1;
    QModelIndex index = ui->categoryView->currentIndex();

    if (index.isValid()) {
        QSqlRecord record = categoryModel->record(index.row());
        catId = record.value(Category_Id).toInt();
    }
    CategoryDialog dialog(catId, categoryModel, this);

    if(dialog.exec() == QDialog::Accepted) {
        ui->categoryView->setCurrentIndex(index);
    }
    else {
        categoryModel->select();
        ui->categoryView->setCurrentIndex(index);
    }
}

void MainWindow::createCategoryModelView()
{
    categoryModel = new QSqlTableModel(this, QSqlDatabase::database("default_connection"));
    categoryModel->setTable("category");
    categoryModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);

    categoryModel->setSort(Category_Id, Qt::AscendingOrder);
    categoryModel->setHeaderData(Category_Title, Qt::Horizontal, tr("Category"));
    categoryModel->setHeaderData(Category_Description, Qt::Horizontal, tr("Description"));
    categoryModel->select();

    ui->categoryView->setModel(categoryModel);
    ui->categoryView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->categoryView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->categoryView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->categoryView->resizeColumnsToContents();
    ui->categoryView->horizontalHeader()->setStretchLastSection(true);
    ui->categoryView->horizontalHeader()->setVisible(true);

    ui->categoryView->setColumnHidden(Category_Id, true);
    ui->categoryView->setColumnHidden(Category_ParentId, true);
    ui->categoryView->setColumnHidden(Category_Position, true);
    ui->categoryView->setColumnHidden(Category_Icon, true);

    ui->categoryView->setCurrentIndex(categoryModel->index(0, 0));

    ui->categoryView->setColumnWidth(Category_Title, 120);

    connect(ui->categoryView->selectionModel(),
            SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updateEntryView()));
}

void MainWindow::createEntryModelView()
{
    entryModel = new QSqlRelationalTableModel(this, QSqlDatabase::database("default_connection"));

    entryModel->setTable("entry");
    entryModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);

    entryModel->setRelation(Entry_CategoryId, QSqlRelation("category", "id", "title"));
    entryModel->setSort(Entry_Id, Qt::AscendingOrder);

    entryModel->setHeaderData(Entry_Title, Qt::Horizontal, tr("Title"));
    entryModel->setHeaderData(Entry_Username, Qt::Horizontal, tr("Username"));
    //    entryModel->setHeaderData(Entry_Password, Qt::Horizontal, tr("Password"));
    entryModel->setHeaderData(Entry_Address, Qt::Horizontal, tr("Address"));
    entryModel->setHeaderData(Entry_CreationTime, Qt::Horizontal, tr("Creation Time"));
    entryModel->setHeaderData(Entry_LastModTime, Qt::Horizontal, tr("Last Mod. Time"));
    entryModel->setHeaderData(Entry_LastAccTime, Qt::Horizontal, tr("Last Acc. Time"));
    entryModel->setHeaderData(Entry_ExpTime, Qt::Horizontal, tr("Exp. Time"));
    entryModel->setHeaderData(Entry_Description, Qt::Horizontal, tr("Description"));
    entryModel->setHeaderData(Entry_Export, Qt::Horizontal, tr("Export"));

    ui->entryView->setModel(entryModel);
    ui->entryView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->entryView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->entryView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->entryView->setColumnWidth(Entry_Title, 120);
    ui->entryView->setColumnWidth(Entry_Username, 75);
    ui->entryView->setColumnWidth(Entry_Address, 250);
    ui->entryView->setColumnWidth(Entry_Description, 250);
    ui->entryView->setColumnWidth(Entry_Export, 75);

    ui->entryView->horizontalHeader()->setStretchLastSection(true);
    ui->entryView->horizontalHeader()->setVisible(true);

    ui->entryView->setColumnHidden(Entry_Id, true);
    ui->entryView->setColumnHidden(Entry_Username, true);
    ui->entryView->setColumnHidden(Entry_Address, true);
    ui->entryView->setColumnHidden(Entry_Password, true);
    ui->entryView->setColumnHidden(Entry_CategoryId, true);
    ui->entryView->setColumnHidden(Entry_Icon, true);
    ui->entryView->setColumnHidden(Entry_CreationTime, true);
    ui->entryView->setColumnHidden(Entry_LastModTime, true);
    ui->entryView->setColumnHidden(Entry_LastAccTime, true);
    ui->entryView->setColumnHidden(Entry_ExpTime, true);
    ui->entryView->setColumnHidden(Entry_Export, true);

    //    ui->entryView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //    ui->entryView->setItemDelegate(new ColorDelegate(this));

    //        ui->entryView->setItemDelegateForColumn(Entry_Description, new TextEditDelegate);
    //        ui->entryView->setItemDelegateForColumn(Entry_Address, new ComboBoxDelegate);
}

void MainWindow::updateEntryView()
{
    QModelIndex index = ui->categoryView->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = categoryModel->record(index.row());
        int id = record.value("id").toInt();
        entryModel->setFilter(QString("categoryid = %1").arg(id));
        entryModel->select();
        ui->entryView->resizeRowsToContents();
    } else {
        entryModel->setFilter("categoryid = -1");
    }
}

void MainWindow::updateCategoryView()
{
    QModelIndex index = ui->categoryView->currentIndex();
    if (index.isValid()) {
        categoryModel->select();
        ui->categoryView->selectRow(index.row());
        ui->categoryView->resizeRowsToContents();
    }
    else {
        ui->categoryView->selectRow(categoryModel->rowCount()-1);
    }
}

void MainWindow::displayEntryViewContextMenu(const QPoint &point)
{
    QModelIndex index = ui->entryView->indexAt(point);
    if (index.isValid()) {
        QMenu cellMenu(this);

        //        cellMenu.addAction("");

        QAction *action = new QAction(this);
        action->setSeparator(true);

        QString text = entryModel->data(entryModel->index(index.row(), entryModel->fieldIndex("address"))).toString();

        text = aes256(decryptPass(), text, false);

        QStringList addressList = text.split("\n", QString::SkipEmptyParts);

        foreach(QString address, addressList) {
            cellMenu.addAction(address.trimmed());
        }

        cellMenu.addAction(action);
        cellMenu.addAction(ui->copyUsernameAction);
        cellMenu.addAction(ui->copyPasswordAction);
        cellMenu.addSeparator();
        cellMenu.addAction(ui->editEntryAction);
        cellMenu.addAction(ui->copyEntryAction);
        cellMenu.addAction(ui->pasteEntryAction);
        cellMenu.addAction(ui->removeEntryAction);
        cellMenu.addSeparator();
        cellMenu.addAction(ui->addToExportListAction);
        cellMenu.addAction(ui->removeFromExportListAction);

        QAction* selectedItem = cellMenu.exec(ui->entryView->viewport()->mapToGlobal(point));

        if (selectedItem && (selectedItem->text().startsWith("ssh", Qt::CaseInsensitive) || selectedItem->text().startsWith("http", Qt::CaseInsensitive) || selectedItem->text().startsWith("term", Qt::CaseInsensitive))) {

            int passwordFieldIndex = entryModel->fieldIndex("password");
            int usernameFieldIndex = entryModel->fieldIndex("username");

            QString username = entryModel->data(entryModel->index(index.row(),usernameFieldIndex)).toString();
            QString password = entryModel->data(entryModel->index(index.row(),passwordFieldIndex)).toString();
            QString address = selectedItem->text();

            address.replace("<USER>", username, Qt::CaseInsensitive);
            address.replace("<PASS>", username, Qt::CaseInsensitive);

            QApplication::clipboard()->setText(aes256(decryptPass(), password, false) + "\n");


            if (address.startsWith("ssh", Qt::CaseInsensitive)) {
                address = termApplication + QString(" %1").arg(address);
            }
            else if (address.startsWith("http", Qt::CaseInsensitive)) {
                address = httpApplication + QString(" %1").arg(address);
            }
            else if (address.startsWith("term", Qt::CaseInsensitive)) {
                address = termApplication + QString(" %1").arg(address.mid(5).trimmed());
            }
            else {
                address = "";
            }

            QProcess *process = new QProcess();
            process->startDetached(address);
        }
    }
    else {
        QMenu viewMenu(this);
        QList<QAction *> viewMenuActionList;
        viewMenuActionList.append(ui->addEntryAction);
        viewMenuActionList.append(ui->pasteEntryAction);
        viewMenu.addActions(viewMenuActionList);

        viewMenu.exec(ui->entryView->viewport()->mapToGlobal(point));
    }
}

void MainWindow::displayCategoryViewContextMenu(const QPoint &point)
{
    QMenu cellMenu(this);
    QMenu *exportMenu;
    QList<QAction *> cellMenuActionList;
    cellMenuActionList.append(ui->editCategoryAction);
    cellMenuActionList.append(ui->removeCategoryAction);

    cellMenu.addActions(cellMenuActionList);

    exportMenu = cellMenu.addMenu(tr("E&xport"));
    exportMenu->addAction(ui->addCategoryToExportListAction);
    exportMenu->addAction(ui->removeCategoryFromExportListAction);

    QMenu viewMenu(this);
    QList<QAction *> viewMenuActionList;
    viewMenuActionList.append(ui->addCategoryAction);
    viewMenu.addActions(viewMenuActionList);

    QModelIndex index = ui->categoryView->indexAt(point);

    if (index.isValid()) {
        cellMenu.exec(ui->categoryView->viewport()->mapToGlobal(point));
    }
    else {
        viewMenu.exec(ui->categoryView->viewport()->mapToGlobal(point));
    }
}

void MainWindow::on_entryView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        on_editEntryAction_triggered();
}

void MainWindow::on_copyUsernameAction_triggered()
{
    QModelIndex index = ui->entryView->currentIndex();
    if (index.isValid()) {
        int usernameFieldIndex = entryModel->fieldIndex("username");
        QString username = entryModel->data(entryModel->index(index.row(),usernameFieldIndex)).toString();
        QApplication::clipboard()->setText(aes256(decryptPass(), username, false));
    }
}

void MainWindow::on_copyPasswordAction_triggered()
{
    QModelIndex index = ui->entryView->currentIndex();
    if (index.isValid()) {
        int passwordFieldIndex = entryModel->fieldIndex("password");
        QString password = entryModel->data(entryModel->index(index.row(),passwordFieldIndex)).toString();
        QApplication::clipboard()->setText(aes256(decryptPass(), password, false));
    }
}

void MainWindow::on_categoryView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        on_editCategoryAction_triggered();
}

void MainWindow::on_removeCategoryAction_triggered()
{
    QModelIndex index = ui->categoryView->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        QString categoryTitle = categoryModel->data(categoryModel->index(row, Category_Title)).toString();

        if (entryModel->rowCount() > 0) {
            QMessageBox messageBox;
            messageBox.setIcon(QMessageBox::Critical);
            messageBox.setWindowTitle(tr("Not Empty"));
            messageBox.setText(tr("This category is not empty. You can not delete %1!").arg(categoryTitle));
            messageBox.setInformativeText("Remove all entries at first and then remove category.");
            messageBox.setDefaultButton(QMessageBox::Close);
            messageBox.exec();
        }
        else {
            switch (showDeleteConfirmationBox(categoryTitle)) {
            case QMessageBox::Yes:
                categoryModel->removeRow(row);
                categoryModel->submitAll();
                updateCategoryView();
                break;
            case QMessageBox::No:
                break;
            case QMessageBox::Cancel:
                break;
            default:
                break;
            }
        }
    }
}

int MainWindow::showDeleteConfirmationBox(QString title)
{
    QMessageBox messageBox;
    messageBox.setIcon(QMessageBox::Question);
    messageBox.setWindowTitle("Are You Sure?");
    messageBox.setText(QString("Are you sure you want to delete %1 ?").arg(title));
    messageBox.setInformativeText("Warning: You can not revert this action!");
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    messageBox.setDefaultButton(QMessageBox::No);
    return messageBox.exec();
}

void MainWindow::on_searchEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    QString searchTerm = ui->searchEdit->text().trimmed();
    if (searchTerm.length() > 0) {
        entryModel->setFilter(QString("entry.title LIKE '%%1%' OR entry.username LIKE '%%1%'  OR entry.address LIKE '%%1%' OR entry.description LIKE '%%1%'").arg(searchTerm));
    }
    else { // When searchbox cleaned, populate current selected category.
        QModelIndex index = ui->categoryView->currentIndex();
        int categoryId = categoryModel->data(categoryModel->index(index.row(),Category_Id)).toInt();
        entryModel->setFilter(QString("entry.categoryid = '%1'").arg(categoryId));
    }
}

void MainWindow::on_clearButton_clicked()
{
    ui->searchEdit->clear();
    ui->searchEdit->setFocus();
}

void MainWindow::on_findInDatabaseAction_triggered()
{
    ui->searchEdit->setFocus();
}

bool MainWindow::eventFilter(QObject* obj, QEvent *event)
{
    if (obj == ui->searchEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Return) {
                on_searchEdit_textChanged("");
                //                ui->entryView->setFocus();
                return true;
            }
            if (keyEvent->key() == Qt::Key_Escape) {
                ui->searchEdit->clear();
                return true;
            }
        }
        return false;
    }
    return QMainWindow::eventFilter(obj, event);
}


void MainWindow::on_aboutAction_triggered()
{    
    QMessageBox::about(this, tr("About"), tr(
                           "Compass is a password manager to keep your sensitive data as encrypted form.\n\n"
                           "Copyright (C) 2011 - 2014  Sinan Ceylan\n"
                           "Version 1.0\n\n"

                           "This program is free software: you can redistribute it and/or modify\n"
                           "it under the terms of the GNU General Public License as published by\n"
                           "the Free Software Foundation, either version 3 of the License, or\n"
                           "(at your option) any later version.\n\n"

                           "This program is distributed in the hope that it will be useful,\n"
                           "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                           "GNU General Public License for more details.\n\n"

                           "You should have received a copy of the GNU General Public License\n"
                           "along with this program.  If not, see <http://www.gnu.org/licenses/>."
                           ));
}

void MainWindow::on_addToExportListAction_triggered()
{    
    QModelIndex index = ui->entryView->currentIndex();
    if (index.isValid()) {
        entryModel->setData(entryModel->index(index.row(),Entry_Export), 1);
        entryModel->submitAll();

    }
}

void MainWindow::on_removeFromExportListAction_triggered()
{
    QModelIndex index = ui->entryView->currentIndex();
    if (index.isValid()) {
        entryModel->setData(entryModel->index(index.row(),Entry_Export), 0);
        entryModel->submitAll();
    }
}

void MainWindow::on_addCategoryToExportListAction_triggered()
{
    QModelIndex index = ui->categoryView->currentIndex();
    if (index.isValid()) {
        int rowCount = entryModel->rowCount();
        if (rowCount > 0) {
            for(int row = 0; row < rowCount; row++) {
                entryModel->data(entryModel->index(row, Entry_Title));
                entryModel->setData(entryModel->index(row, Entry_Export), 1);
            }
            entryModel->submitAll();
        }
    }
}

void MainWindow::on_removeCategoryFromExportListAction_triggered()
{
    QModelIndex index = ui->categoryView->currentIndex();
    if (index.isValid()) {
        int rowCount = entryModel->rowCount();
        if (rowCount > 0) {
            for(int row = 0; row < rowCount; row++) {
                entryModel->setData(entryModel->index(row, Entry_Export), 0);
            }
            entryModel->submitAll();
        }
    }
}

void MainWindow::on_exportDatabaseAction_triggered()
{
    ExportDialog dialog(decryptPass(), this);

    if(dialog.exec() == QDialog::Rejected) {
        updateEntryView();
    }
}


void MainWindow::on_importDatabaseAction_triggered()
{
    ImportDialog dialog(decryptPass(), this);

    if(dialog.exec() == QDialog::Rejected) {
        updateEntryView();
    }
}

void MainWindow::on_closeDatabaseAction_triggered()
{
    closeDatabaseConnection("default_connection");
    updateCategoryView();
    updateEntryView();
}

void MainWindow::on_changeMasterPasswordAction_triggered()
{
    PasswordDialog dialog("default_connection", "change_password", this);
    if (dialog.exec() == QDialog::Accepted) {
        masterPassword = cryptPass(dialog.getPasswordText());
        qDebug() << "Password changed.";
    }
    else {
        createCategoryModelView();
        createEntryModelView();
        updateEntryView();
        updateCategoryView();
    }
}

void MainWindow::on_optionsAction_triggered()
{
    OptionsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QSettings settings("compass", "compass");
        termApplication = settings.value("term_application", "").toString();
        httpApplication = settings.value("http_application", "").toString();
    }
}

void MainWindow::on_passwordGeneratorAction_triggered()
{
    GeneratePasswordDialog dialog(this);
    dialog.exec();
}

QString MainWindow::cryptPass(QString Pass)
{
    QString pass = sha256(Pass);
    tempPass =sha256(generatePassword(randInt(32,128)));
    pass = aes256(tempPass, pass, true);
    return pass;
}

QString MainWindow::decryptPass()
{
    QString pass = masterPassword;
    pass = aes256(tempPass, pass, false);
    return pass;
}


void MainWindow::on_copyEntryAction_triggered()
{
    QModelIndex index = ui->entryView->currentIndex();
    if (index.isValid()) {
        rowIndex = entryModel->data(entryModel->index(index.row(),Entry_Id)).toInt();
    }
}

void MainWindow::on_pasteEntryAction_triggered()
{
    if  (rowIndex) {
        QModelIndex categoryIndex = ui->categoryView->currentIndex();

        if (categoryIndex.isValid()) {
            QSqlRecord categoryRecord = categoryModel->record(categoryIndex.row());
            int catId = categoryRecord.value(Category_Id).toInt();

            QSqlQuery query(QSqlDatabase::database("default_connection"));

            query.exec(QString("INSERT INTO entry SELECT null, title, username, password, address, description, creationtime, lastmodtime, lastacctime, exptime, %1, icon, export FROM entry WHERE id = %2").arg(catId).arg(rowIndex));
            query.lastError();
            updateEntryView();
        }
    }
}
