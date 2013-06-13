#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include <QString>
#include <note/note.h>

class MultiMedia : public Note
{
		Q_OBJECT

	public:
		// Constructor
		MultiMedia( unsigned int id );

		// Getter
		const QString &	getDescription() const;
		const QString &	getPath() const;

		// Setter
		void			setDescription( const QString & description);
		void			setPath( const QString & path);
		void			binaryToWorkspace();

	private:
		// Constructor
		MultiMedia( const MultiMedia& n);

		// Member
		QString			m_description;
		QString			m_path;

		// Operator
		MultiMedia&		operator=(const MultiMedia& n);

};

#endif
