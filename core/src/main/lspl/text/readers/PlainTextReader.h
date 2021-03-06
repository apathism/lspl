#ifndef _LSPL_TEXT_READERS_PLAINTEXTREADER_H_
#define _LSPL_TEXT_READERS_PLAINTEXTREADER_H_

#include "../../base/Base.h"

#include "../Forward.h"
#include "../TextConfig.h"

#include "TextReader.h"

#include <string>
#include <boost/ptr_container/ptr_vector.hpp>

class CGraLine;

#include "../../morphology/WordForm.h"
#include "../../graphan/Unit.h"

namespace lspl { namespace text { namespace readers {

class LSPL_EXPORT PlainTextReader : public lspl::text::readers::TextReader
{
public:
	PlainTextReader();
	virtual ~PlainTextReader();

	TextRef readFromStream( std::istream & is );
	TextRef readFromStream( std::istream & is, const std::string & enc );

	TextRef readFromString( const std::string & str );
	TextRef readFromString( const std::string & str, const std::string & enc );
public:
	TextConfig config;
private:
	void addTransitions( TextBuilder & builder, Node & start, Node & end, const graphan::Unit & unit, boost::ptr_vector<morphology::WordForm> & forms );
};

} } }

#endif /*_LSPL_TEXT_READERS_PLAINTEXTREADER_H_*/
