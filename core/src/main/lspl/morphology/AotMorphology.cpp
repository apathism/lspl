/*
 * AotMorphology.cpp
 *
 *  Created on: Dec 24, 2008
 *      Author: alno
 */

#include "../base/BaseInternal.h"

#include "AotMorphology.h"

#include <string.h>

#include "aot/Source/AgramtabLib/RusGramTab.h"
#include "aot/Source/LemmatizerLib/Lemmatizers.h"
#include "aot/Source/LemmatizerLib/Paradigm.h"

#include "aot/Source/common/utilit.h"

using namespace lspl::text::attributes;

#include <iostream>

#ifdef WIN32
#define PD "\\"
#else
#define PD "/"
#endif

namespace lspl { namespace morphology {

const char * AotMorphology::findRml() {
	static std::string candidates[] = {"data",".." PD "data","deps" PD "aot",".." PD "deps" PD "aot",".." PD ".." PD "deps" PD "aot"};

	for ( int i = 0; i < 5; ++ i ) {
		std::string rmlIni = candidates[i] + "/Bin/rml.ini";

		if ( FILE * fi = fopen(rmlIni.c_str(),"r") ) {
			fclose( fi );

			return candidates[i].c_str();
		}
	}

	return 0;
}

void AotMorphology::setupRml() {
	if ( getenv("RML") )
		return;

	if ( const char * rml = findRml() ) {
#ifdef WIN32
			putenv( (std::string( "RML=") + rml).c_str() );
#else
			setenv( "RML", rml, 1 );
#endif
	} else {
		std::cerr << "Error: No suitable RML path found!" << std::endl;
	}
}

AotMorphology::~AotMorphology() {
	delete lemmatizer;
	delete agramtab;
}

AotMorphology::AotMorphology() {
	setupRml();

	try {
		std::string strError;
		lemmatizer = new CLemmatizerRussian();
		lemmatizer->m_bAllowRussianJo = true;
		lemmatizer->m_bMaximalPrediction = true;
		lemmatizer->m_bUseStatistic = true;

		if( !lemmatizer->LoadDictionariesRegistry( strError ) ) {
			throw MorphologyInitException( strError, __FILE__, __LINE__ );
		}

		agramtab = new CRusGramTab();
		if( !agramtab->LoadFromRegistry() ) {
			throw MorphologyInitException( "Couldn't load gramtab", __FILE__, __LINE__ );
		}
	} catch ( const MorphologyInitException & e ) {
		throw e;
	} catch ( const CExpc & e ) {
		throw MorphologyInitException( "Couldn't init morphology: " + e.m_strCause, __FILE__, 0 );
	} catch (...) {
		throw MorphologyInitException( "Couldn't init morphology due to unknown error", __FILE__, 0 );
	}
}

void AotMorphology::appendWordForms( const std::string & token, boost::ptr_vector<WordForm> & forms ) {
	std::string tokenCopy = token;
	std::vector<CFormInfo> aotForms;

	lemmatizer->CreateParadigmCollection(false, tokenCopy, is_russian_upper( (BYTE)token[0] ), true, aotForms);

	for ( uint i = 0; i < aotForms.size(); ++ i) {
		const CFormInfo & f = aotForms[i];
		std::string srcAncode = f.GetSrcAncode();
		const char * gramCodes = srcAncode.c_str();

		uint attributeSetCount = srcAncode.length() / 2;
		uint64 * attributeSets = new uint64[ attributeSetCount ];

		for ( uint i = 0; i < attributeSetCount; ++i )
			attributeSets[i] = getAttributes( gramCodes + 2*i );

		forms.push_back( new WordForm( getSpeechPart( gramCodes ), f.GetWordForm( 0 ), getStem( f ), attributeSets, attributeSetCount ) );
	}
}

const char * AotMorphology::getStem( const CFormInfo & f ) {
	if ( f.m_bFound ) {
		int lemmaInfoNo = f.GetParadigmId() & 0x7fffff;
		int lemmaStrNo = lemmatizer->m_LemmaInfos[lemmaInfoNo].m_LemmaStrNo;

		return lemmatizer->m_Bases[lemmaStrNo].GetString();
	} else {
		return "";
	}
}

SpeechPart AotMorphology::getSpeechPart( const char * gramCode ) {
	int speechPartAot = agramtab->GetPartOfSpeech( gramCode );
	static SpeechPart UNKNOWN = SpeechPart::WORD;
	static SpeechPart speechPartMap[22] = {
			SpeechPart::NOUN, // NOUN
			SpeechPart::ADJECTIVE, // ADJ_FULL
			SpeechPart::VERB, // VERB
			SpeechPart::PRONOUN, // PRONOUN
			SpeechPart::PRONOUN, // PRONOUN_P
			SpeechPart::PRONOUN, // PRONOUN_PREDK
			SpeechPart::NUMERAL, // NUMERAL
			SpeechPart::NUMERAL, // NUMERAL_P
			SpeechPart::ADVERB, // ADV
			UNKNOWN, // PREDK
			SpeechPart::PREPOSITION, // PREP
			UNKNOWN, // POSL
			SpeechPart::CONJUNCTION, // CONJ
			SpeechPart::INTERJECTION, // INTERJ
			UNKNOWN, // INP
			UNKNOWN, // PHRASE
			SpeechPart::PARTICLE, // PARTICLE
			SpeechPart::ADJECTIVE, // ADJ_SHORT
			SpeechPart::PARTICIPLE, // PARTICIPLE
			SpeechPart::ADVERBIAL_PARTICIPLE, // ADVERB_PARTICIPLE
			SpeechPart::PARTICIPLE, // PARTICIPLE_SHORT
			SpeechPart::VERB, // INFINITIVE
	};

	return speechPartMap[ speechPartAot ];
}

bool AotMorphology::getParadigm(std::string word, std::vector<CFormInfo> &form) {
	return lemmatizer->CreateParadigmCollection(false, word, true, true, form);
}

int AotMorphology::getSP(const char *code) const {
	return agramtab->GetPartOfSpeech(code);
}

uint64 AotMorphology::getAttributes( const char * gramCode ) {
	QWORD result;
	agramtab->GetGrammems( gramCode, result);
	return result;
}

std::string AotMorphology::getAttributesString( uint64 values ) {
	std::string s = agramtab->GrammemsToStr( values );

	if (!s.empty() && (s[s.length() -1] == ','))
			s.erase(s.length() - 1);

	return s;
}

std::string AotMorphology::upcase( const char * str ) {
	std::string word( str );

	for( uint i = 0; i < word.length(); i++ )
		if ( is_russian_lower((BYTE)word[i]) )
			word[i] = rtoupper((BYTE)word[i]);
		else if ( is_english_lower((BYTE)word[i]) )
			word[i] = etoupper((BYTE)word[i]);

	return word;
}

std::string AotMorphology::upcase( const char * start, const char * end ) {
	std::string word( start, end );

	for( uint i = 0; i < word.length(); i++ )
		if ( is_russian_lower((BYTE)word[i]) )
			word[i] = rtoupper((BYTE)word[i]);
		else if ( is_english_lower((BYTE)word[i]) )
			word[i] = etoupper((BYTE)word[i]);

	return word;
}

std::string AotMorphology::lowcase( const char * str ) {
	std::string word( str );

	for( uint i = 0; i < word.length(); i++ )
		if ( is_russian_upper((BYTE)word[i]) )
			word[i] = rtolower((BYTE)word[i]);
		else if ( is_english_upper((BYTE)word[i]) )
			word[i] = etolower((BYTE)word[i]);

	return word;
}

std::string AotMorphology::lowcase( const char * start, const char * end ) {
	std::string word( start, end );

	for( uint i = 0; i < word.length(); i++ )
		if ( is_russian_upper((BYTE)word[i]) )
			word[i] = rtolower((BYTE)word[i]);
		else if ( is_english_upper((BYTE)word[i]) )
			word[i] = etolower((BYTE)word[i]);

	return word;
}

} } // namespace lspl::morphology
