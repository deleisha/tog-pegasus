//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CQLChainedIdentifier.h"
#include "CQLChainedIdentifierRep.h"
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

CQLChainedIdentifierRep::CQLChainedIdentifierRep(){
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(String inString)
{
	parse(inString);
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(const CQLIdentifier &id)
{
        _subIdentifiers.append(id);
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(const CQLChainedIdentifierRep* rep){
	_subIdentifiers = rep->_subIdentifiers;
}

CQLChainedIdentifierRep::~CQLChainedIdentifierRep(){
	
}

const Array<CQLIdentifier>& CQLChainedIdentifierRep::getSubIdentifiers()const
{
	return _subIdentifiers;
}

CQLIdentifier CQLChainedIdentifierRep::getLastIdentifier()const{
	if(_subIdentifiers.size() > 0)
		return _subIdentifiers[_subIdentifiers.size()-1];
	return CQLIdentifier();
}

String CQLChainedIdentifierRep::toString()const{
	String s;
	for(Uint32 i = 0; i < _subIdentifiers.size(); i++){
		s.append(_subIdentifiers[i].toString());
		if(i < _subIdentifiers.size() - 1)
			s.append(".");
	}
	return s;
}

void CQLChainedIdentifierRep::append(const CQLIdentifier & id){
	_subIdentifiers.append(id);
}

Boolean CQLChainedIdentifierRep::isSubChain(const CQLChainedIdentifier & chain)const{
	Array<CQLIdentifier> ids = chain.getSubIdentifiers();
	for(Uint32 i = 0; i < ids.size(); i++){
		if(ids[i] != _subIdentifiers[i].getName())
			return false;
	}
	return true;
}

CQLIdentifier CQLChainedIdentifierRep::operator[](Uint32 index)const
{
	return CQLIdentifier(_subIdentifiers[index]);
}

CQLChainedIdentifierRep& CQLChainedIdentifierRep::operator=(const CQLChainedIdentifierRep& rhs){
	if(&rhs != this){
		_subIdentifiers = rhs._subIdentifiers;
	}
	return *this;
}

Uint32 CQLChainedIdentifierRep::size()const{
	return _subIdentifiers.size();
}

Boolean CQLChainedIdentifierRep::prepend(const CQLIdentifier & id){
	/*
	   Compare id against the first element in _subIdentifiers, 
	   if not an exact match, then prepend.  This is used to fully
	   qualify the chained identifier.
	*/
	if(id != _subIdentifiers[0]){
		_subIdentifiers.prepend(id);
		return true;
	}
	return false;
}

void CQLChainedIdentifierRep::applyContext(QueryContext& inContext)
{
  if (_subIdentifiers.size() == 0)
    return;

  // Chained identifiers that are standalone symbolic constants 
  // should have had the context applied already.  If this method
  // is called and this is still a standalone symbolic constant,
  // then that is an error.
  if (_subIdentifiers.size() == 1 &&
      _subIdentifiers[0].getName().getString().size() == 0 &&
      _subIdentifiers[0].isSymbolicConstant())
  {
    throw Exception("TEMP MSG: CQLChainedIdentifierRep::applyContext - standalone symb const");
  }

  CQLIdentifier firstId = _subIdentifiers[0];
  
  // Process if the first identifier has some contents.
  if ((firstId.getName().getString().size() != 0) || firstId.isWildcard()) 
  {
    Array<CQLIdentifier> fromList = inContext.getFromList();

    if (firstId.isWildcard())
    {
      // Single element chain that is wildcarded.
      // Prepend the FROM class.
      _subIdentifiers.prepend(fromList[0]);
    }  
    else
    {
      // Not a single element wildcard.
      if (firstId.isScoped())
      {
        // The first identifier is a scoped property or a 
        // scoped symbolic constant.
        // Prepend the FROM class.
        // Example: SELECT * FROM F WHERE someprop = X::p#'OK'
	_subIdentifiers.prepend(fromList[0]);
      }
      else
      {
        // The first identifier is not a scoped property or a scoped
        // symbolic constant.

        // Determine if the first identifier's name is in the FROM list.
	String classContext = firstId.getName().getString();
	CQLIdentifier matchedId = inContext.findClass(classContext);
	if (matchedId.getName().getString().size() == 0)
	{
	  // Could not find the firstId in the FROM list.
	  // Assume the firstId is a property on the FROM class,
          // and prepend the FROM class, except in the case described below.
          //
          // NOTE:
          // We need special handling for symbolic constants, because there are cases
          // where the first id is a class that is not the FROM class.
          // Refer to section 5.4.3 of the CQL spec.
          //
          // Examples:
          // SELECT * FROM F WHERE F.someprop = ClassNotInFromList.prop#'OK'
          // We don't want to prepend the FROM class to ClassNotInFromList 
          // in this case.
          //
          // SELECT * FROM F WHERE F.someprop = embeddedObject.X::p#'OK'
          // Even though embeddedObject may an property on F, and not
          // a classname, we still don't need to prepend F because the
          // scoping class X will be used to get the value of 'OK' on property
          // p.
          //
          // SELECT * FROM F WHERE F.someprop = propOnClassF#'OK'
          // We want to prepend in this case.  The identifier propOnClassF
          // is assumed to be a property on the FROM class, not a classname
          // itself.
          //
          // Note that standalone symbolic constants, like #'OK'
          // are errors in this code.
          // See above for details.
          //
	  if (_subIdentifiers.size() == 1 ||
	      !_subIdentifiers[_subIdentifiers.size()-1].isSymbolicConstant())
	  {
	    _subIdentifiers.prepend(fromList[0]);
	  }
	}
	else
	{	
	  // The firstId was found in the FROM list, but it could have been 
	  // an alias
	  if (!matchedId.getName().equal(firstId.getName()))
	  {
	    // It was an alias.
	    // Replace the alias with the FROM class
	    _subIdentifiers[0] = matchedId;
	  }
	  else 
	  {
	    // It was not an alias. Do nothing.
	    ;
	  }
	}
      }
    }
  }
  
  // Go through and replace any aliases on scoping classes
  for (Uint32 i = 0; i < _subIdentifiers.size(); i++)
  {
    if (_subIdentifiers[i].isScoped())
    {
      CQLIdentifier match = inContext.findClass(_subIdentifiers[i].getScope());
      if (match.getName().getString().size() != 0)
      {
        // The scoping class was either the FROM class, or an alias of the
        // FROM class.  Replace the scoping class with the FROM class.
	_subIdentifiers[i].applyScope(match.getName().getString());
      }
    }
  }
}

void CQLChainedIdentifierRep::parse(String & string){
	/* 
	  - parse string on "."
	  - start from the end of string
	  - if more than one substring found, 
		-- store first found string then
		-- prepend remaining substrings 
	*/
	Char16 delim('.');
	Uint32 index;
	String range;

	/* remove any array ranges so we dont parse a false . */
	if((index = string.find("[")) != PEG_NOT_FOUND){
		range = string.subString(index);
		string.remove(index);
	}

	index = string.reverseFind(delim);
	if(index == PEG_NOT_FOUND){
		/* append the range we may have removed */
		string.append(range);
		_subIdentifiers.append(CQLIdentifier(string));
	}else{
		/* append the range we may have removed */
		String tmp = string.subString(index+1);
		tmp.append(range);
		PEGASUS_STD(cout) << "tmp = " << tmp << PEGASUS_STD(endl);
		_subIdentifiers.append(CQLIdentifier(tmp));

		while(index != PEG_NOT_FOUND){
			tmp = string.subString(0,index);
			index = tmp.reverseFind(delim);
			if(index == PEG_NOT_FOUND){
				_subIdentifiers.prepend(CQLIdentifier(tmp));
			}
			else{
				_subIdentifiers.prepend(CQLIdentifier(tmp.subString(index+1)));
			}
		}
	}
}

PEGASUS_NAMESPACE_END
