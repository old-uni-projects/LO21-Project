#include "dbmanager/databasemanager.h"
#include <notemanager/notesmanager.h>
using namespace std;


/********************************************************************
 *                           Constructers                           *
 ********************************************************************/

DatabaseManager::DatabaseManager(const QString &path, const QString &user, const QString &pass) : database(new QSqlDatabase()), dbpath(path)
{
	//set database driver to QSQLITE avec une connection ayant pour nom "projet.lo21"
	*database = QSqlDatabase::addDatabase("QSQLITE", "projet.lo21");

	QFile file(dbpath);

	bool dbIsNew = !file.exists();

	database->setDatabaseName(dbpath);

	//can be removed
	database->setHostName("localhost");
	database->setUserName(user);
	database->setPassword(pass);

	if(!database->open())
	{
		throw DBException("INITIALISATION Database", "Can not open database. Path = "+file.fileName());
	}
	else
	{
		if (dbIsNew)
			initDB();
		else
		{
			/******** AFFICHE TOUTES LES TABLES DE LA DB *********
			 *******
			 ******
			 *****
			 ****
			 ***
			 **
			 *
			QStringList list = database->tables(QSql::Tables);
			QStringList::Iterator it = list.begin();
			while( it != list.end() )
			{
				cout << "Table: " << it->toStdString() << "\n";
				++it;
			}
			*/
		}
		database->exec("PRAGMA foreign_keys=ON;");
	}

}

/********************************************************************
 *                              getters                             *
 ********************************************************************/

const QString DatabaseManager::getpath() const
{
	return dbpath;
}

/********************************************************************
 *                           TrashManaging                          *
 ********************************************************************/

bool DatabaseManager::emptyTrash() const
{

	return query("Delete from Note where trashed=1");

}

bool DatabaseManager::isTrashed (unsigned int n) const
{
	QSqlQuery request(*database);
	if(request.exec("select trashed from Note where id="+QString::number(n)))
	{
		if(request.next())
			return request.value(0).toBool();
		else
			throw DBException("select trashed from Note where id="+QString::number(n), "Aucun enregistrement retourné ... Cette note n'existe pas ...");
	}
	else
		throw DBException("select trashed from Note where id="+QString::number(n), request.lastError().databaseText());
}

bool DatabaseManager::putToTrash (unsigned int n) const
{
	return query("UPDATE Note SET trashed = 1 WHERE id = "+ QString::number(n));
}

bool DatabaseManager::removeFromTrash (unsigned int n) const
{
	return query("UPDATE Note SET trashed = 0 WHERE id = "+ QString::number(n));
}

bool DatabaseManager::isTrashEmpty() const
{
	QSqlQuery request(*database);
	if(request.exec("select count(*) from Note where trashed = 1"))
	{
		if(request.next())
			return request.value(0).toInt() == 0;
		else
			throw DBException("select count(*) from Note where trashed = 1", request.lastError().databaseText());
	}
	else
		throw DBException("select count(*) from Note where trashed = 1", request.lastError().databaseText());
}
/********************************************************************
 *                            DB Requests                           *
 ********************************************************************/

bool DatabaseManager::query(const QString &query) const
{
	QSqlQuery request(*database);

	bool result = request.exec(query);

	if (result)
		return true;
	else
		throw DBException(query, request.lastError().databaseText());
}

bool DatabaseManager::initDB()
{
	QString qry[6];

	qry[0] = "create table Note (id integer , title varchar("+QString::number(constants::SIZE_MAX_TITLE)+"), typeNote varchar("+QString::number(constants::SIZE_MAX_TYPE_NOTE)+"), trashed  BOOL DEFAULT '0' NOT NULL, primary key(id))";
	qry[1] = "create table Article (id integer, txt text, primary key(id), FOREIGN KEY(id) REFERENCES Note(id) ON DELETE CASCADE)";
	qry[2] = "create table Document (id integer, primary key(id), FOREIGN KEY(id) REFERENCES Note(id) ON DELETE CASCADE)";
	qry[3] = "create table Multimedia (id integer, description TEXT, path varchar("+QString::number(constants::SIZE_MAX_PATH)+"), primary key(id), FOREIGN KEY(id) REFERENCES Note(id) ON DELETE CASCADE)";
	qry[4] = "create table AssocDoc (docMaster integer, note integer, primary key(docMaster, note), FOREIGN KEY(docMaster) REFERENCES Document(id) ON DELETE CASCADE, FOREIGN KEY(note) REFERENCES Note(id) ON DELETE CASCADE)";
	qry[5] = "create table AssocTag (id integer, name varchar("+QString::number(constants::SIZE_MAX_TAG)+"), primary key(id, name), FOREIGN KEY(id) REFERENCES Note(id) ON DELETE CASCADE)";

	bool b = true;

	for (int i = 0; i<6; ++i)
	{
		if (!query(qry[i]))
			b &= false;
	}

	if (!b)
	{
		QFile::remove(dbpath);
		exit(0);
	}

	return b;
}

unsigned int DatabaseManager::getLastID() const
{
	QSqlQuery query(*database);
	QString sql = "SELECT last_insert_rowid()";

	if(!query.exec(sql))
		throw DBException(sql, query.lastError().databaseText());

	query.next();// Only one result no need of the while loop

	return query.value(0).toUInt();

}

bool DatabaseManager::tagExist(const QString &t) const
{
	QSqlQuery query(*database);
	QString sql = "SELECT id from AssocTag where name ='"+escape(capitalize(t))+"'";

	if(!query.exec(sql))
		throw DBException(sql, query.lastError().databaseText());

	if(query.size() != 0)
		return true;
	else
		return false;
}

/********************************************************************
 *                              Deleters                              *
 ********************************************************************/

bool DatabaseManager::deleteNote (unsigned int id) const
{
	return query("Delete from Note where id = "+ QString::number(id));
}

bool DatabaseManager::deleteNote () const
{
	return query("Delete from Note where 1=1");
}

bool DatabaseManager::deleteTag (const QString &t) const
{
	return query("Delete from AssocTag where name ='"+escape(capitalize(t))+"'");
}

bool DatabaseManager::deleteTag () const
{
	return query("Delete from AssocTag where 1=1");
}

bool DatabaseManager::flushDB () const
{
	return deleteTag() && deleteNote();
}

/********************************************************************
 *                     Escapers / Capitalizers                      *
 ********************************************************************/

QString DatabaseManager::escape(QString s) const
{
	s.replace("'", "''");
	return s;
}

QString DatabaseManager::capitalize(QString str) const
{
	str = str.toLower();
	str[0] = str[0].toUpper();
	return str;
}

/********************************************************************
 *                            Inserters                             *
 ********************************************************************/

unsigned int DatabaseManager::insertNoteCommon(const QString & type) const{
	QString titre = "";

	if (query("INSERT INTO Note (id, title, typeNote, trashed) VALUES (NULL, '"+escape(titre)+"','"+escape(type)+"', 0)"))
		return getLastID();
	else
		exit(0);
}

bool DatabaseManager::insertMultimedia(const unsigned int id) const
{
	QString description = "";
	QString path = "";

	return query("INSERT INTO Multimedia (id, description, path) VALUES ("+ QString::number(id) +", '"+escape(description)+"', '"+escape(path)+"')");
}

unsigned int DatabaseManager::insertNote(const QString& typeNote) const
{
	unsigned int id = insertNoteCommon(typeNote);
	bool result = false;

	if(typeNote == "Article")
		result = query("INSERT INTO Article (id) VALUES ("+ QString::number(id) +")");
	else if(typeNote == "Document")
		result = query("INSERT INTO Document (id) VALUES ("+ QString::number(id) +")");
	else if(typeNote == "Image" ||typeNote == "Audio" || typeNote == "Video")
		result = insertMultimedia(id);
	else
		throw DBException("INSERT Note", "'"+typeNote+"' is not a Note type.");

	if (result)
		return id;
	else
	{
		deleteNote(id);
		throw DBException("INSERT Note", "SQL Error");
	}
}


/********************************************************************
 *                             Updaters                             *
 ********************************************************************/

bool DatabaseManager::updateNote (const Article& a)  const
{
	bool result = true;
	QString txt = a.getText();
	QString title = a.getTitle();
	int id = a.getId();

	result &= query("UPDATE Note SET title = '"+escape(title)+"' WHERE id ='"+ QString::number(id) +"'");
	result &= query("UPDATE Article SET txt = '"+escape(txt)+"' WHERE id ='"+ QString::number(id) +"'");

	return result;
}

bool DatabaseManager::updateNote (const Document& d)  const
{

	bool result = true;

	QString title = d.getTitle();
	int id = d.getId();

	result &= flushDoc(d);

	result &= query("UPDATE Note SET title = '"+escape(title)+"' WHERE id ='"+ QString::number(id) +"'");

	for (vector<Note* >::const_iterator it = d.begin(); it != d.end(); it++)
	{
		addNoteToDoc(d, **it);
	}

	return result;
}

bool DatabaseManager::updateNote (const MultiMedia& m)  const
{
	bool result = true;
	QString description = m.getDescription();
	QString path = m.getPath();
	QString title = m.getTitle();

	int id = m.getId();

	result &= query("UPDATE Note SET title = '"+escape(title)+"' WHERE id ='"+ QString::number(id) +"'");
	result &= query("UPDATE Multimedia SET description = '"+escape(description)+"', path = '"+escape(path)+"' WHERE id ='"+ QString::number(id)+"'");

	return result;
}


/********************************************************************
 *                            Retrievers                            *
 ********************************************************************/

std::vector< pair <unsigned int, QString > > DatabaseManager::getNotes() const
{
	std::vector< pair <unsigned int, QString > > result;
	pair <unsigned int, QString > temp;

	QSqlQuery request(*database);
	QString sql = "Select id, title from Note where trashed = 0";

	if(!request.exec(sql))
		throw DBException(sql, request.lastError().databaseText());

	while(request.next())
	{
		temp.first = request.value(0).toInt();
		temp.second = request.value(1).toString();

		result.push_back(temp);
	}

	return result;

}

std::vector< pair <unsigned int, QString > > DatabaseManager::getTrash() const
{
	std::vector< pair <unsigned int, QString > > result;
	pair <unsigned int, QString > temp;

	QSqlQuery request(*database);
	QString sql = "Select id, title from Note where trashed = 1";

	if(!request.exec(sql))
		throw DBException(sql, request.lastError().databaseText());

	while(request.next())
	{
		temp.first = request.value(0).toInt();
		temp.second = request.value(1).toString();

		result.push_back(temp);
	}

	return result;

}

QStringList DatabaseManager::getAllTags() const
{
	QSqlQuery request(*database);
	QStringList result;

	QString sql = "Select DISTINCT name from AssocTag";

	if(!request.exec(sql))
		throw DBException(sql, request.lastError().databaseText());

	while (request.next())
	{
		result << request.value(0).toString();
	}

	return result;
}

QStringList DatabaseManager::getTags(const Note &n) const
{
	QSqlQuery request(*database);
	QStringList result;

	int id = n.getId();

	QString sql = "Select name from AssocTag where id = '"+QString::number(id)+"'";

	if(!request.exec(sql))
		throw DBException(sql, request.lastError().databaseText());

	while (request.next())
	{
		result << request.value(0).toString();
	}

	return result;
}

std::vector< pair <unsigned int, QString > > DatabaseManager::getNotes(const QString &tag) const
{
	std::vector< pair <unsigned int, QString > > result;
	pair <unsigned int, QString > temp;

	QSqlQuery request(*database);

	QString sql = "Select n.id , n.title from AssocTag a, Note n where a.name = '"+escape(capitalize(tag))+"' and a.id = n.id and n.trashed = 0";

	if(!request.exec(sql))
		throw DBException(sql, request.lastError().databaseText());

	while (request.next())
	{
		temp.first = request.value(0).toInt();
		temp.second = request.value(1).toString();
		result.push_back(temp);
	}

	return result;

}

QString DatabaseManager::getNoteType(const unsigned int id)
{
	QSqlQuery request(*database);

	QString sql ="Select typeNote from Note where id = '"+QString::number(id)+"'";

	if(!request.exec(sql))
		throw DBException(sql, request.lastError().databaseText());


	request.next();

	return request.value(0).toString();
}

bool DatabaseManager::getNotesInDoc (Document& d) const
{
	int idDoc = d.getId();

	QSqlQuery request(*database);
	vector<QString> result;

	NotesManager& nm = NotesManager::getInstance();

	QString sql = "Select note from AssocDoc where docMaster = "+QString::number(idDoc);

	if(!request.exec(sql))
		throw DBException(sql, request.lastError().databaseText());

	while (request.next())
	{
		d.addNote(nm.getNote(request.value(0).toInt()));
	}

	return true;
}

/********************************************************************
 *                   AssocBuilders // AssocRemovers                 *
 ********************************************************************/

bool DatabaseManager::tagAssocNote (const Note& n, const QStringList& l) const // Associe une liste de tag et une note, si les tags n'existent pas ils sont créés
{
	bool result = true;
	for( QStringList::ConstIterator it =  l.begin(); it!=l.end(); it++ )
		result &= tagAssocNote(n, *it);

	return result;
}

bool DatabaseManager::tagAssocNote (const Note& n, const QString& t) const // Associe un tag et une note, si le tag n'existe pas il est créé
{
	return addTagAssoc(n,t);
}

bool DatabaseManager::addTagAssoc (const Note& n,const QString &t) const // Association d'un tag et d'une note
{
	int id = n.getId();
	return query("INSERT INTO AssocTag (id, name) VALUES ("+QString::number(id)+", '"+escape(capitalize(t))+"')");
}

bool DatabaseManager::removeTagAssoc (const Note& n, const QString &t) const // Désassociation d'un tag et d'un note
{
	int id = n.getId();
	return query("DELETE FROM AssocTag WHERE id = "+QString::number(id)+" and name = '"+escape(capitalize(t))+"'");
}

bool DatabaseManager::addNoteToDoc (const Document &d, const Note &n) const // Ajoute une Note dans un document
{
	int idDoc = d.getId();
	int idNote = n.getId();
	if (idDoc != idNote)
		return query("INSERT INTO AssocDoc (docMaster, note) VALUES ("+QString::number(idDoc)+", "+QString::number(idNote)+")");
	else
		throw DBException("Try to include the document in itself", "DocumentID = "+QString::number(idDoc));
}

bool DatabaseManager::removeNotefromDoc (const Document &d, const Note &n) const // Enleve une Note d'un Document
{
	int idDoc = d.getId();
	int idNote = n.getId();
	if (idDoc != idNote)
		return query("DELETE FROM AssocDoc WHERE docMaster = "+QString::number(idDoc)+" and note = "+QString::number(idNote));
	else
		throw DBException("Try to remove the document from itself", "DocumentID = "+QString::number(idDoc));
}

bool DatabaseManager::flushNoteAssoc (const Note& n) const //Enleve tous les tags d'une note
{
	int id = n.getId();
	return query("DELETE FROM AssocTag WHERE id = "+QString::number(id));
}

bool DatabaseManager::flushDoc (const Document& d) const
{
	return query("Delete From AssocDoc where docMaster = "+QString::number(d.getId()));
}

/********************************************************************
 *                             Fillers                              *
 ********************************************************************/

bool DatabaseManager::fillNote (Article& a)  const
{
	QSqlQuery query(*database);
	unsigned int id = a.getId();

	bool result = true;
	result &= query.exec("SELECT n.title, a.txt FROM Note n, Article a WHERE a.id = n.id and a.id = "+QString::number(id));

	if(query.isActive())
	{
		query.next();// Only one result no need of the while loop

		QString title = query.value(0).toString();
		QString text = query.value(1).toString();

		a.setTitle(title);
		a.setText(text);
	}

	return result;
}

bool DatabaseManager::fillNote (Document& d)  const
{
	QSqlQuery query(*database);
	int id = d.getId();

	bool result = true;
	result &= query.exec("SELECT title FROM Note WHERE id = "+QString::number(id));

	query.next();// Only one result no need of the while loop

	QString title = query.value(0).toString();

	d.setTitle(title);

	result &= getNotesInDoc(d);

	return result;
}

bool DatabaseManager::fillNote (MultiMedia& m)  const
{
	QSqlQuery query(*database);
	int id = m.getId();

	bool result = true;
	result &= query.exec("SELECT n.title, m.description, m.path FROM Note n, Multimedia m WHERE n.id = m.id and n.id = "+QString::number(id));

	query.next();// Only one result no need of the while loop

	QString title = query.value(0).toString();
	QString description = query.value(1).toString();
	QString path = query.value(2).toString();

	m.setTitle(title);
	m.setDescription(description);
	m.setPath(path);

	return result;
}

/********************************************************************
 *                            Destructor                            *
 ********************************************************************/

DatabaseManager::~DatabaseManager()
{
	QString dbname = database->connectionName();
	database->close();
	delete database;
	database = NULL;
	QSqlDatabase::removeDatabase("projet.lo21");
}

/********************************************************************
 *                            Singleton                             *
 ********************************************************************/

DatabaseManager* DatabaseManager::s_inst = NULL;

DatabaseManager& DatabaseManager::getInstance(QString path, QString user, QString pass){
	if( s_inst == NULL )
		s_inst = new DatabaseManager(path,user,pass);
	return (*s_inst);
}

void DatabaseManager::destroy()
{
	if( s_inst != NULL ){
		delete s_inst;
		s_inst = NULL;
	}
}
