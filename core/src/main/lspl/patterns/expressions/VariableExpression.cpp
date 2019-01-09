/*
 * VariableExpression.cpp
 *
 *  Created on: Sep 28, 2008
 *      Author: alno
 */
#include <cstdio>
#include <cstdlib>

#include "../../base/BaseInternal.h"
#include "VariableExpression.h"
#include "../../text/Transition.h"
#include "../matchers/Context.h"
#include "../matchers/Matcher.h"

using namespace lspl::text::attributes;
using namespace lspl::patterns::matchers;

namespace lspl { namespace patterns { namespace expressions {

VariableExpression::VariableExpression( const matchers::Variable & variable ) :
	variable( variable ) {
}

VariableExpression::VariableExpression( text::attributes::SpeechPart sp, uint index ) :
	variable( Variable( sp, index ) ) {
}

VariableExpression::VariableExpression( const Pattern & pt, uint index ) :
	variable( Variable( pt, index ) ) {
}

VariableExpression::~VariableExpression() {
}

void VariableExpression::evaluateTo( const text::Transition * currentAnnotation, const matchers::Variable currentVar, const Context & ctx, ValueList & results ) const {
	Context::ConstRange range = ctx.getValues( variable );

	for ( Context::ConstIterator it = range.first; it != range.second; ++ it )
		results.push_back( AttributeValue( *it->second ) );

	if ( currentVar == variable )
		results.push_back( AttributeValue( *currentAnnotation ) );
}

void VariableExpression::dump( std::ostream & out, const std::string & tabs ) const {
	out << variable;
}

bool VariableExpression::equals( const Expression & e ) const {
	if ( const VariableExpression * exp = dynamic_cast<const VariableExpression *>( &e ) ) {
		return exp->variable == variable;
	} else {
		return false;
	}
}

bool VariableExpression::containsVariable( matchers::Variable var ) const {
	return var == variable;
}

bool VariableExpression::containsVariables() const {
	return true;
}

bool VariableExpression::containsCurrentAnnotation() const {
	return false;
}

} } }
