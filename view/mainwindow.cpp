#include "mainwindow.h"
#include "ui_mainwindow.h"

/********************************************************************
 *                            Constructor                           *
 ********************************************************************/

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow), m_actualNote(NULL), m_actualNoteView(NULL), m_editMode(false)
{
	ui->setupUi(this);

	// Setup MainView
	setupMenu();
	setupSearchArea();
	setupEditorArea();
	setupExportArea();

}

/********************************************************************
 *                              Setup                               *
 ********************************************************************/

void MainWindow::setupMenu(){

	// New Menu
	QObject::connect(ui->actionNewArticle,SIGNAL(triggered()),this,SLOT(newNote()));
}

void MainWindow::setupSearchArea(){}

void MainWindow::setupEditorArea(){

	// Display
	showEditor(false);

	// Connects
	QObject::connect(ui->editSaveButton, SIGNAL(clicked()), this, SLOT(editSaveNote()));
	QObject::connect(ui->deleteCancelButton, SIGNAL(clicked()), this, SLOT(deleteCancelNote()));

	// Title
	ui->titleEdit->setMaxLength(constants::SIZE_MAX_TITLE);
}

void MainWindow::setupExportArea(){}

/********************************************************************
 *                           Slot Method		                    *
 ********************************************************************/

void MainWindow::newNote(){
	QAction* a = (QAction*)QObject::sender();
	editNewNote(a->text());
}

void MainWindow::editSaveNote(){
	if( !m_editMode ){
		// Setup Edit Mode
		ui->editSaveButton->setText("Save");
		ui->deleteCancelButton->setText("Cancel");
		ui->titleEdit->setReadOnly(false);
		m_actualNoteView->setEditMode(true);
	}
	else{
		// Save Note and Setup View Mode
		m_actualNoteView->saveChanges();
		m_actualNoteView->setEditMode(false);
		ui->titleEdit->setReadOnly(true);
		ui->deleteCancelButton->setText("Delete");
		ui->editSaveButton->setText("Edit");
	}
	m_editMode = !m_editMode;
}

void MainWindow::deleteCancelNote(){
	if( !m_editMode ){
		// To Do Delete
	}
	else{
		// Save Note and Setup View Mode
		m_actualNoteView->setEditMode(false);
		ui->titleEdit->setReadOnly(true);
		ui->deleteCancelButton->setText("Delete");
		ui->editSaveButton->setText("Edit");
	}
	m_editMode = !m_editMode;
}

/********************************************************************
 *                         Controller Method                        *
 ********************************************************************/

void MainWindow::editNewNote( const QString& typeNote ){
	if( m_editMode )
		showErrorMessageBox("Impossible to edit new note, currents modifications need to be saved before.");
	else{
		// Create Note
		NotesManager& nm = NotesManager::getInstance();
		Note& n = nm.getNewNote(typeNote);

		// Display Note View
		displayNote(n);
	}
}

void MainWindow::displayNote( unsigned int id ){
	if( m_editMode )
		showErrorMessageBox("Impossible to open Note, currents modifications need to be saved before.");
	else{
		// Load Note
		NotesManager& nm = NotesManager::getInstance();
		Note& n = nm.getNote(id);

		// Display Note View
		displayNote(n);
	}
}

void MainWindow::displayNote(Note &n){
	if( m_editMode )
		showErrorMessageBox("Impossible to open Note, currents modifications need to be saved before.");
	else{
		// Load Note View
		GeneralViewFactory& vf = GeneralViewFactory::getInstance();
		NoteView& nv = vf.getView(n);

		// Display Note View
		m_actualNote = &n;
		m_actualNoteView = &nv;
		nv.setParent(ui->speNoteArea);
		ui->speNoteLayout->addWidget(&nv);
		showEditor(true);
	}
}

void MainWindow::hideActualNote(){
	if( m_actualNote != NULL ){
		// Hide Actual Note View
		showEditor(false);
		ui->speNoteLayout->removeWidget(m_actualNoteView);
		m_actualNoteView->setParent(NULL);
		m_actualNoteView = NULL;
		m_actualNote = NULL;
	}
}

/********************************************************************
 *                           Info Method		                    *
 ********************************************************************/

void MainWindow::showErrorMessageBox(const QString& msg){
	QMessageBox messageBox;
	messageBox.critical(0,"Error",msg);
	messageBox.setFixedSize(500,200);
}

/********************************************************************
 *                           Tool Method		                    *
 ********************************************************************/

void MainWindow::loadActualNoteContent(){
	if( m_actualNote != NULL ){
		// Load Actual Note in View
		ui->titleEdit->setText(m_actualNote->getTitle());
		m_actualNoteView->loadNoteContent();
	}
}

void MainWindow::showEditor(bool b){
	ui->tabBox->setEnabled(b);
	ui->editor->setShown(b);
}

/********************************************************************
 *                            Destructor                            *
 ********************************************************************/

MainWindow::~MainWindow()
{
    delete ui;
}
