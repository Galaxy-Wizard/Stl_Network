#include "stdafx.h"
#include "DOMParser.h"

#include "../util/boost-string-conversion.h"

using namespace std;
using namespace boost;


namespace {
#ifdef _DEBUG
	wofstream tlog( L"traverse.txt" );
#endif
}


struct DOMParser::Impl
{
	Impl()
	{
	}

	Impl( const std::string &xml, const std::string &docid )
		: matching_policy(All)
		, parse_unicode_document(false)
	{
		try{
			xml_document = lexical_cast<wstring,string>( xml );
			xml_document_id = lexical_cast<wstring,string>( docid );
		}
		catch( bad_lexical_cast &e )
		{
			// todo
			e.what();
		}
	}

	Impl( const std::wstring &xml, const std::wstring &docid )
		: matching_policy(All)
	{
		xml_document = xml;
		xml_document_id = docid;
	}

	~Impl()
	{
	}


	std::wstring xml_document;
	std::wstring xml_document_id;

	int matching_policy;

	bool parse_unicode_document;

	Nodes find_result;
};


DOMParser::DOMParser()
: pimpl( new Impl )
{

}


DOMParser::DOMParser( const std::string &xml, const std::string &docid )
: pimpl( new Impl( xml, docid ) )
{

}


DOMParser::DOMParser( const std::wstring &xml, const std::wstring &docid )
: pimpl( new Impl( xml, docid ) )
{

}


DOMParser::~DOMParser()
{

}


DOMParser::ParseResult DOMParser::Print( void )
{
	try
	{

		DOMImplementation*  impl = 
			DOMImplementationRegistry::getDOMImplementation( wstring(L"LS").c_str( ) );

		DOMLSParser *parser = 
			static_cast<DOMImplementationLS*>(impl)->createLSParser(
			DOMImplementationLS::MODE_SYNCHRONOUS, 0
			);

		parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
		CustomErrorHandler  err;
		parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, &err);

		std::string txml = boost::lexical_cast<string,wstring>(pimpl->xml_document);
		std::string tid = boost::lexical_cast<string,wstring>(pimpl->xml_document_id);
		MemBufInputSource *membuff( 
			new MemBufInputSource(
			(const XMLByte*)txml.c_str()
			, txml.length()
			, tid.c_str(), false 
			)
			);
		assert( membuff );

		Wrapper4InputSource *wrapper(
			new Wrapper4InputSource( membuff )
			);
		assert( wrapper );

		XERCES_CPP_NAMESPACE::DOMDocument *doc = parser->parse( wrapper );

		DOMLSSerializer *writer = 
			static_cast<DOMImplementationLS*>(impl)->createLSSerializer( );
		assert( writer );

		writer->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, &err);
		DOMElement *elemlist = doc->getDocumentElement();
		if( NULL == elemlist )
		{
			assert( elemlist );
			return Failed;
		}

		boost::shared_ptr<XMLCh> tmp( writer->writeToString( elemlist ) );
		wstring buff(tmp.get());
		wcout << buff << endl;

		wrapper->release();
		parser->release();
		writer->release();

	}
	catch (const xercesc_3_1::DOMException& e) {
		e.getMessage();
		return Failed;
	} 
	catch (const std::exception& e) {
		e.what();
		BHDebug( false, e.what() );
		return Failed;
	}


	return Succeeded;
}


void DOMParser::Find(
	const Tags &target_tags
	, const Values &target_values
	, const AttributeNames &target_attribute_names
	, const AttributeValues &target_attribute_values
	, const Namespaces &target_namespaces
	)
{

	try
	{

		DOMImplementation*  impl = 
			DOMImplementationRegistry::getDOMImplementation( wstring(L"LS").c_str( ) );

		DOMLSParser *parser = 
			static_cast<DOMImplementationLS*>(impl)->createLSParser(
			DOMImplementationLS::MODE_SYNCHRONOUS, 0
			);

		parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
		CustomErrorHandler  err;
		parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, &err);

		std::string txml = boost::lexical_cast<string,wstring>(pimpl->xml_document);
		std::string tid = boost::lexical_cast<string,wstring>(pimpl->xml_document_id);
		MemBufInputSource *membuff( 
			new MemBufInputSource(
			(const XMLByte*)txml.c_str()
			, txml.length()
			, tid.c_str(), false 
			)
			);
		assert( membuff );

		Wrapper4InputSource *wrapper(
			new Wrapper4InputSource( membuff )
			);
		assert( wrapper );

		XERCES_CPP_NAMESPACE::DOMDocument *doc = parser->parse( wrapper );
		DOMNode *root = doc->getFirstChild();

		// enumelate
		Traverse( 
			root
			, target_tags, target_values
			, target_attribute_names, target_attribute_values
			, target_namespaces );

		wrapper->release();
		parser->release();

	}
	catch (const xercesc_3_1::DOMException& e) {
		e.getMessage();
		return;
	} 
	catch (const std::exception& e) {
		e.what();
		return;
	}

}


void DOMParser::Traverse(
	DOMNode *prev
	, const std::wstring &tag
	, int depth
	)
{
#if 0
	DOMElement *elem = dynamic_cast<DOMElement*>( prev );
	DOMAttr *attr = dynamic_cast<DOMAttr*>( prev );
	DOMCharacterData *cdata = dynamic_cast<DOMCharacterData*>( prev );
	XERCES_CPP_NAMESPACE::DOMDocument *doc = dynamic_cast<XERCES_CPP_NAMESPACE::DOMDocument*>( prev );
	DOMDocumentFragment *frag = dynamic_cast<DOMDocumentFragment*>( prev );
	DOMDocumentType *doc_type = dynamic_cast<DOMDocumentType*>( prev );
	DOMEntity *entity = dynamic_cast<DOMEntity*>( prev );
	DOMEntityReference *entity_ref = dynamic_cast<DOMEntityReference*>( prev );
	DOMNotation *note = dynamic_cast<DOMNotation*>( prev );
	DOMProcessingInstruction *pross = dynamic_cast<DOMProcessingInstruction*>( prev );
	DOMXPathNamespace *pathname = dynamic_cast<DOMXPathNamespace*>( prev );
	wstring tabs;
	for( int d = 0; d < depth; d++ ) tabs += L"\t";
	if( elem ) tlog << tabs << L"d=" << depth << L" DOMElement" << L".." << prev->getNodeName() << endl;
	if( attr ) tlog << tabs << L"d=" << depth << L" DOMAttr" << L".." << prev->getNodeName() << endl;
	if( cdata ) tlog << tabs << L"d=" << depth << L" DOMCharacterData" <<  L".." << prev->getNodeValue() << endl;
	if( doc ) tlog << tabs << L"d=" << depth << L" DOMDocument" <<  L".." << prev->getNodeName() << endl;
	if( frag ) tlog << tabs << L"d=" << depth << L" DOMEDOMDocumentFragmentlement" <<  L".." << prev->getNodeName() << endl;
	if( doc_type ) tlog << tabs << L"d=" << depth << L" DOMDocumentType" <<  L".." << prev->getNodeName() << endl;
	if( entity ) tlog << tabs << L"d=" << depth << L" DOMEntity" <<  L".." << prev->getNodeName() << endl;
	if( entity_ref ) tlog << tabs << L"d=" << depth << L" DOMEntityReference" <<  L".." << prev->getNodeName() << endl;
	if( note ) tlog << tabs << L"d=" << depth << L" DOMNotation" <<  L".." << prev->getNodeName() << endl;
	if( pross ) tlog << tabs << L"d=" << depth << L" DOMProcessingInstruction" <<  L".." << prev->getNodeName() << endl;
	if( pathname ) tlog << tabs << L"d=" << depth << L" DOMXPathNamespace" <<  L".." << prev->getNodeName() << endl;
#endif

	DOMNode *node(NULL);
	node = prev;

	std::wstring tmp_tag;
	if( prev->getNodeName() ) tmp_tag.assign( prev->getNodeName() );

	if( tmp_tag == tag )
	{
		pimpl->find_result.push_back( prev );
	}

	// traverse children
	DOMNodeList *children = prev->getChildNodes();
	for( XMLSize_t i = 0; i < children->getLength(); i++ )
	{
		node = children->item(i);
		assert( node );
		Traverse( node, tag );
	}
}


void DOMParser::Traverse(
	DOMNode *prev
	, const Tags &target_tags
	, const Values &target_values
	, const AttributeNames &target_attribute_names
	, const AttributeValues &target_attribute_values
	, const Namespaces &target_namespaces
	, int depth
	, bool loose
	)
{
	int restriction = Nothing;
	if( !target_tags.empty() ){ AddMatchingPolicy( Tag ); restriction |= Tag; }
	if( !target_values.empty() ){ AddMatchingPolicy( Value ); restriction |= Value; }
	if( !target_attribute_names.empty() ){ AddMatchingPolicy( AttributeName ); restriction |= AttributeName; }
	if( !target_attribute_values.empty() ){ AddMatchingPolicy( Attribute ); restriction |= Attribute; }
	if( !target_namespaces.empty() ){ AddMatchingPolicy( Namespace ); restriction |= Namespace; }

#if 0
	DOMElement *elem = dynamic_cast<DOMElement*>( prev );
	DOMAttr *attr = dynamic_cast<DOMAttr*>( prev );
	DOMCharacterData *cdata = dynamic_cast<DOMCharacterData*>( prev );
	XERCES_CPP_NAMESPACE::DOMDocument *doc = dynamic_cast<XERCES_CPP_NAMESPACE::DOMDocument*>( prev );
	DOMDocumentFragment *frag = dynamic_cast<DOMDocumentFragment*>( prev );
	DOMDocumentType *doc_type = dynamic_cast<DOMDocumentType*>( prev );
	DOMEntity *entity = dynamic_cast<DOMEntity*>( prev );
	DOMEntityReference *entity_ref = dynamic_cast<DOMEntityReference*>( prev );
	DOMNotation *note = dynamic_cast<DOMNotation*>( prev );
	DOMProcessingInstruction *pross = dynamic_cast<DOMProcessingInstruction*>( prev );
	DOMXPathNamespace *pathname = dynamic_cast<DOMXPathNamespace*>( prev );
	wstring tabs;
	for( int d = 0; d < depth; d++ ) tabs += L"\t";
	if( elem ) tlog << tabs << L"d=" << depth << L" DOMElement" << L".." << prev->getNodeName() << endl;
	if( attr ) tlog << tabs << L"d=" << depth << L" DOMAttr" << L".." << prev->getNodeName() << endl;
	if( cdata ) tlog << tabs << L"d=" << depth << L" DOMCharacterData" <<  L".." << prev->getNodeValue() << endl;
	if( doc ) tlog << tabs << L"d=" << depth << L" DOMDocument" <<  L".." << prev->getNodeName() << endl;
	if( frag ) tlog << tabs << L"d=" << depth << L" DOMEDOMDocumentFragmentlement" <<  L".." << prev->getNodeName() << endl;
	if( doc_type ) tlog << tabs << L"d=" << depth << L" DOMDocumentType" <<  L".." << prev->getNodeName() << endl;
	if( entity ) tlog << tabs << L"d=" << depth << L" DOMEntity" <<  L".." << prev->getNodeName() << endl;
	if( entity_ref ) tlog << tabs << L"d=" << depth << L" DOMEntityReference" <<  L".." << prev->getNodeName() << endl;
	if( note ) tlog << tabs << L"d=" << depth << L" DOMNotation" <<  L".." << prev->getNodeName() << endl;
	if( pross ) tlog << tabs << L"d=" << depth << L" DOMProcessingInstruction" <<  L".." << prev->getNodeName() << endl;
	if( pathname ) tlog << tabs << L"d=" << depth << L" DOMXPathNamespace" <<  L".." << prev->getNodeName() << endl;
#endif

	DOMNode *node(NULL);
	node = prev;

	std::wstring tag;
	if( prev->getNodeName() ) tag.assign( prev->getNodeName() );

	std::wstring value;
	if( prev->getFirstChild() ){
		DOMCharacterData *cdata = dynamic_cast<DOMCharacterData*>(prev->getFirstChild());
		if( cdata )
			value.assign( cdata->getNodeValue() );
		value = value;
	}

	DOMNamedNodeMap *attribute_map = node->getAttributes();
	XMLSize_t attrs(0);
	if( attribute_map ) attrs = attribute_map->getLength();

	std::wstring ns;
	if( prev->getNamespaceURI() ) ns.assign( prev->getNamespaceURI() );

	int found = Nothing;
	if( !target_tags.empty() )
	{
		boost_foreach( const std::wstring &t, target_tags )
		{
			if( t == tag )
			{
				found |= Tag;
				break;
			}
		}
	}
	if( !target_values.empty() )
	{
		boost_foreach( const std::wstring &t, target_values )
		{
			if( t == value )
			{
				found |= Value;
				break;
			}
		}
	}
	if( !target_attribute_names.empty() && attrs > 0 )
	{
		boost_foreach( const std::wstring &t, target_attribute_names )
		{
			for( XMLSize_t i = 0; i < attrs; i++ )
			{
				wstring attr_name( attribute_map->item(i)->getNodeName() );
				if( t == attr_name )
				{
					found |= AttributeName;
				}
			}
		}
	}
	if( !target_attribute_values.empty() && attrs > 0 )
	{
		boost_foreach( const std::wstring &t, target_attribute_values )
		{
			for( XMLSize_t i = 0; i < attrs; i++ )
			{
				wstring attr_name( attribute_map->item(i)->getNodeValue() );
				if( t == attr_name )
				{
					found |= Attribute;
				}
			}
		}
	}
	if( !target_namespaces.empty() )
	{
		boost_foreach( const std::wstring &t, target_namespaces )
		{
			if( t == ns )
			{
				found |= Namespace;
			}
		}
	}

	if( !loose && restriction == found )
	{
		pimpl->find_result.push_back( prev );
	}
	else if( loose && found != Nothing )
	{
		pimpl->find_result.push_back( prev );
	}


	// traverse children
	DOMNodeList *children = prev->getChildNodes();
	for( XMLSize_t i = 0; i < children->getLength(); i++ )
	{
		node = children->item(i);
		assert( node );
		Traverse( 
			node
			, target_tags, target_values
			, target_attribute_names, target_attribute_values
			, target_namespaces
			, depth+1
			);
	}

}


std::wstring DOMParser::UPNPGetControlURL( const std::string &service_type )
{
	try
	{
		DOMImplementation*  impl = 
			DOMImplementationRegistry::getDOMImplementation( wstring(L"LS").c_str( ) );

		DOMLSParser *parser = 
			static_cast<DOMImplementationLS*>(impl)->createLSParser(
			DOMImplementationLS::MODE_SYNCHRONOUS, 0
			);

		parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
		CustomErrorHandler  err;
		parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, &err);

		std::string txml = boost::lexical_cast<string,wstring>(pimpl->xml_document);
		std::string tid = boost::lexical_cast<string,wstring>(pimpl->xml_document_id);
		MemBufInputSource *membuff( 
			new MemBufInputSource(
			(const XMLByte*)txml.c_str()
			, txml.length()
			, tid.c_str(), false 
			)
			);
		assert( membuff );

//		cout<<endl<<endl<<endl<<endl<<endl<<txml.c_str()<<endl<<endl<<endl<<endl<<endl;

		Wrapper4InputSource *wrapper(
			new Wrapper4InputSource( membuff )
			);
		assert( wrapper );

		XERCES_CPP_NAMESPACE::DOMDocument *doc = parser->parse( wrapper );
		DOMNode *root = doc->getFirstChild();

		// enumelate
		DOMParser::Tags tags;
		DOMParser::Values values;
		DOMParser::AttributeNames attrnames;
		DOMParser::AttributeValues attrvalues;
		DOMParser::Namespaces namespaces;
		
		tags.push_back( std::wstring( L"serviceType" ) );
		wstring tmpst;
		try{
			tmpst = lexical_cast<wstring,string>(service_type);
		}catch( bad_lexical_cast &e ) { 
			e.what();
			aconsole( e.what() );
			return wstring(L"");
		}
		values.push_back( tmpst );
		
		Traverse( root, tags, values, attrnames, attrvalues, namespaces );

		if(pimpl->find_result.size()==0)
		{
			return wstring(L"");
		}

		assert( pimpl->find_result.size() == 1 );

		DOMNode *service_type = pimpl->find_result[0];
		assert( service_type );
		DOMNode *service = service_type->getParentNode();
		assert( service );

		pimpl->find_result.clear();
		Traverse( service, wstring(L"controlURL") );
		assert( pimpl->find_result.size() == 1 );

		wstring control_url;
		if( pimpl->find_result.size() == 1 )
		{
			DOMNode *control_url_node = pimpl->find_result[0];
			assert( control_url_node );
			if( control_url_node && control_url_node->getFirstChild() )
			{
				DOMCharacterData *text_node = dynamic_cast<DOMCharacterData*>(control_url_node->getFirstChild() );
				if( text_node )
				{
					const XMLCh *data = text_node->getNodeValue();
					if( data ) control_url.assign( data );
				}
			}
		}

		wrapper->release();
		parser->release();

		return control_url;
	}
	catch (const xercesc_3_1::DOMException& e) {
		e.getMessage();
	} 
	catch (const boost::exception& e) {
		e;
	}

	return wstring(L"");
}


std::wstring DOMParser::GetElementsContent( const std::wstring &tag )
{
	try
	{
		DOMImplementation*  impl = 
			DOMImplementationRegistry::getDOMImplementation( wstring(L"LS").c_str( ) );

		DOMLSParser *parser = 
			static_cast<DOMImplementationLS*>(impl)->createLSParser(
			DOMImplementationLS::MODE_SYNCHRONOUS, 0
			);

		parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
		CustomErrorHandler  err;
		parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, &err);

		std::string txml = boost::lexical_cast<string,wstring>(pimpl->xml_document);
		std::string tid = boost::lexical_cast<string,wstring>(pimpl->xml_document_id);
		MemBufInputSource *membuff( 
			new MemBufInputSource(
				(const XMLByte*)txml.c_str(), txml.length(), tid.c_str(), false )
			);
		assert( membuff );

		Wrapper4InputSource *wrapper( new Wrapper4InputSource( membuff ) );
		assert( wrapper );

		XERCES_CPP_NAMESPACE::DOMDocument *doc = parser->parse( wrapper );
		DOMNode *root = doc->getFirstChild();

		// enumelate
		DOMParser::Tags tags;
		DOMParser::Values values;
		DOMParser::AttributeNames attrnames;
		DOMParser::AttributeValues attrvalues;
		DOMParser::Namespaces namespaces;
		tags.push_back( tag );
		Traverse( root, tags, values, attrnames, attrvalues, namespaces );
		assert( pimpl->find_result.size() == 1 );

		wstring ip;
		DOMNode *wanip = pimpl->find_result[0];
		assert( wanip );
		if( !wanip )
		{
			wrapper->release();
			parser->release();
			pimpl->find_result.clear();
			return wstring(L"");
		}
		if( wanip->hasChildNodes() )
		{
			DOMNode *child = wanip->getFirstChild();
			if( child )
			{
				DOMNode *grandchild = child->getFirstChild();
				ip.assign( child->getNodeValue() );
			}
		}

		wrapper->release();
		parser->release();
		pimpl->find_result.clear();

		return ip;
	}
	catch (const xercesc_3_1::DOMException& e) {
		e.getMessage();
	} 
	catch (const boost::exception& e) {
		e;
	}

	return wstring(L"");
}



void DOMParser::SetMatchingPolicy( int value )
{
	pimpl->matching_policy = value;
}


void DOMParser::AddMatchingPolicy( MatchingPolicy pol )
{
	pimpl->matching_policy |= pol;
}


void DOMParser::RemoveMatchingPolicy( MatchingPolicy pol )
{
	pimpl->matching_policy = pimpl->matching_policy ^ pol;
}


bool DOMParser::HasPolicy( MatchingPolicy pol )
{
	if( 
		pol != Tag
		|| pol != AttributeName
		|| pol != Attribute
		|| pol != Value
		|| pol != Namespace
		|| pol != All
		)
	{
		BHDebug( false, L"illegal argment: complex policy" );
		return false;
	}

	return (pimpl->matching_policy & pol) > 0;
}

