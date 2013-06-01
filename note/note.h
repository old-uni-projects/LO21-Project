#ifndef NOTE_H
#define NOTE_H

#include <QObject>
#include <QString>
#include <note/strategies.h>

class Note : public QObject, public Strategies<Note>
{
		Q_OBJECT

	public:
		// Constructor
		Note( unsigned int id );
		Note( unsigned int id, const QString & name );

		// Getter
		unsigned int		getId() const;
		const QString &		getTitle() const;
		bool				isModified() const;

		// Setter
		void				setTitle(const QString & title);

		// Operator
		bool				operator==(const Note& n) const;

		// Virtual Method

		// Method

	private:
		// Constructor
		Note( const Note& n);

		// Member
		const unsigned int	m_id;
		QString				m_title;
		bool				m_modified;

		// Operator
		Note&				operator=(const Note& n);

};

#endif
