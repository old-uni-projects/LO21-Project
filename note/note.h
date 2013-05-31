#ifndef NOTE_H
#define NOTE_H

#include <QString>

class Note
{

	public:
		// Constructor
		Note( unsigned int id );
		Note( unsigned int id, const QString & name );

		// Getter
		unsigned int	getId() const;
		const QString &	getTitle() const;
		bool			isModified() const;

		// Setter
		void			setTitle(const QString & title);

		// Operator
		bool			operator==(const Note& n) const;

		// Virtual Method

		// Method

	private:
		// Constructor
		Note( const Note& n);

		// Member
		unsigned int	m_id;
		QString			m_title;
		bool			m_modified;

		// Operator
		Note&			operator=(const Note& n);

};

#endif