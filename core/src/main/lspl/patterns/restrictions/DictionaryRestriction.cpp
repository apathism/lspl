/*
 * DictionaryRestriction.cpp
 *
 *  Created on: Sep 28, 2008
 *      Author: alno
 */
#include "../../base/BaseInternal.h"
#include "../../text/Transition.h"
#include "../../text/attributes/AttributeKey.h"
#include "../matchers/Context.h"
#include "../matchers/Matcher.h"
#include "DictionaryRestriction.h"

using namespace lspl::text::attributes;

using lspl::patterns::expressions::Expression;

namespace lspl { namespace patterns { namespace restrictions {

DictionaryRestriction::DictionaryRestriction( const dictionaries::DictionaryConstRef & dict ) :
	dictionary( dict ) {
}

DictionaryRestriction::~DictionaryRestriction() {
}

bool DictionaryRestriction::matches( const text::Transition * currentAnnotation, const matchers::Variable currentVar, const matchers::Context & ctx ) const {
	std::vector<std::string> words;

	for( boost::ptr_vector<Expression>::const_iterator it = args.begin(); it != args.end(); ++ it ) {
		Expression::ValueListPtr v = it->evaluate( currentAnnotation, currentVar, ctx );

		words.push_back( v->empty() ? AttributeValue::UNDEFINED.getString() : v->back().getString() );
	}

	return dictionary->accepts( words );
}

void DictionaryRestriction::dump( std::ostream & out, const std::string & tabs ) const {
	out << dictionary->Name();
}

bool DictionaryRestriction::equals( const Restriction & r ) const {
	if ( const DictionaryRestriction * dr = dynamic_cast<const DictionaryRestriction *>( &r ) ) {
		if ( dr->dictionary != dictionary ) return false;
		if ( dr->args.size() != args.size() ) return false;

		for ( uint i = 0, l = args.size(); i < l; ++ i )
			if ( !dr->args[i].equals( args[i] ) )
				return false;

		return true;
	} else {
		return false;
	}
}

bool DictionaryRestriction::containsVariable( matchers::Variable var ) const {
	for( boost::ptr_vector<Expression>::const_iterator it = args.begin(); it != args.end(); ++ it )
		if ( it->containsVariable( var ) )
			return true;

	return false;
}

bool DictionaryRestriction::containsVariables() const {
	for( boost::ptr_vector<Expression>::const_iterator it = args.begin(); it != args.end(); ++ it )
		if ( it->containsVariables() )
			return true;

	return false;
}

bool DictionaryRestriction::containsCurrentAnnotation() const {
	for( boost::ptr_vector<Expression>::const_iterator it = args.begin(); it != args.end(); ++ it )
		if ( it->containsCurrentAnnotation() )
			return true;

	return false;
}

} } }
