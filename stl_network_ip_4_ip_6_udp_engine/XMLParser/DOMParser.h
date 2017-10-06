#ifndef __DOMPARSER_H__
#define __DOMPARSER_H__

#include "../util/xerces-helper.h"

///// IMPORTANT //////////////////////////////////////////////////////
// This parser has been using the xerces that does'nt corresponding to
// unicode. A rebuild of the xerces library with the IBM-ICU library
// will be needed.
//////////////////////////////////////////////////////////////////////


class DOMParser
{
public:
	enum ParseResult
	{
		Succeeded
		, Failed
	};

	enum MatchingPolicy
	{
		Nothing = 0x0000
		, Tag = 0x0001
		, AttributeName = 0x0002
		, Attribute = 0x0004
		, Value = 0x0008
		, Namespace = 0x0010
		, All = Tag | AttributeName | Attribute | Value | Namespace
	};

	typedef std::deque<DOMNode*> Nodes;

	typedef std::deque<std::wstring> Tags;
	typedef std::deque<std::wstring> Values;
	typedef std::deque<std::wstring> AttributeNames;
	typedef std::deque<std::wstring> AttributeValues;
	typedef std::deque<std::wstring> Namespaces;

public:
	DOMParser();
	DOMParser( const std::string &xml, const std::string &docid );
	DOMParser( const std::wstring &xml, const std::wstring &docid );
	~DOMParser();

public:
	ParseResult Print( void );

	void Find( 
		const Tags &target_tags
		, const Values &target_values
		, const AttributeNames &target_attribute_names
		, const AttributeValues &target_attribute_values
		, const Namespaces &target_namespaces
		);

	void Traverse(
		DOMNode *prev
		, const std::wstring &tag
		, int depth = 0 );

	void Traverse(
		DOMNode *prev
		, const Tags &target_tags
		, const Values &target_values
		, const AttributeNames &target_attribute_names
		, const AttributeValues &target_attribute_value
		, const Namespaces &target_namespaces
		, int depth = 0
		, bool loose = false
		);

	//upnp
public:
	std::wstring UPNPGetControlURL( const std::string &service_type );

public:
	std::wstring GetElementsContent( const std::wstring &tag );

public:
	void SetMatchingPolicy( int value );
	void AddMatchingPolicy( MatchingPolicy pol );
	void RemoveMatchingPolicy( MatchingPolicy pol );
private:
	bool HasPolicy( MatchingPolicy pol );


private:
	struct Impl;
	boost::shared_ptr<Impl> pimpl;
};


#endif

