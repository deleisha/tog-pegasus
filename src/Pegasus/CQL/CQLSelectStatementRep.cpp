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

#include "CQLSelectStatement.h"
#include "CQLSelectStatementRep.h"

#include <iostream>

#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/AutoPtr.h>
#include "CQLValue.h"
#include "CQLIdentifier.h"
#include "CQLChainedIdentifier.h"

// ATTN: TODOs - 
// spec compliance
// assertions
// optimize
// localized CQL exceptions
// documentation
// trace? but this could be used by provider

PEGASUS_NAMESPACE_BEGIN

struct PropertyNode
{
  CIMName name;
  Boolean wildcard;
  AutoPtr<PropertyNode> sibling;
  AutoPtr<PropertyNode> firstChild;

  PropertyNode() {/*PEGASUS_STD(cout) << "new " << this << PEGASUS_STD(endl);*/}
  ~PropertyNode() {/*PEGASUS_STD(cout) << "delete " << this << PEGASUS_STD(endl);*/}
};


CQLSelectStatementRep::CQLSelectStatementRep()
  :SelectStatementRep(),
   _hasWhereClause(false)
{
}

CQLSelectStatementRep::CQLSelectStatementRep(String& inQlang, String& inQuery, QueryContext* inCtx)
  :SelectStatementRep(inQlang, inQuery, inCtx),
   _hasWhereClause(false)
{
}

CQLSelectStatementRep::CQLSelectStatementRep(const CQLSelectStatementRep& rep)
  :SelectStatementRep(rep),
   _selectIdentifiers(rep._selectIdentifiers),
   _whereIdentifiers(rep._whereIdentifiers),
   _hasWhereClause(rep._hasWhereClause),
   _predicate(rep._predicate)
{
}

CQLSelectStatementRep::~CQLSelectStatementRep()
{
}

CQLSelectStatementRep& CQLSelectStatementRep::operator=(const CQLSelectStatementRep& rhs)
{
  if (this ==  &rhs)
    return *this;

  SelectStatementRep::operator=(rhs);

  _whereIdentifiers = rhs._whereIdentifiers;
  _selectIdentifiers = rhs._selectIdentifiers;
  _predicate = rhs._predicate;

  return *this;
}

Boolean CQLSelectStatementRep::evaluate(const CIMInstance& inCI)
{
  if (!hasWhereClause())
    return true;
  else
  {
    try
    {
      return _predicate.evaluate(inCI, *_ctx);
    }
    catch (UninitializedObjectException& )
    {
      // The null contagion rule.
      // ATTN change this to a specific CQLException
      return false;
    }
  }
}

void CQLSelectStatementRep::applyProjection(CIMInstance& inCI) throw(Exception)
{
  // assumes that applyContext had been called.

  AutoPtr<PropertyNode> rootNode(new PropertyNode);
  Array<CQLIdentifier> fromList = _ctx->getFromList();
  rootNode->name = fromList[0].getName();  // not doing joins

  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    CQLValue val(_selectIdentifiers[i]);

    // The CQLValue::getNormalizedIdentifier API does the following:
    // 1) Checks if the instance can be projected:
    //    a) the instance and its embedded objects are the right type
    //       as determined by the class contexts (note that the instance
    //       can be a subclass of the class context) 
    //    b) the property referenced by the identifier exists on the instance
    //       or its embedded object as needed.
    //    c) if either of the above are not true, an exception is thrown
    // 2) Returns a CQLChainedIdentifier with each chain element replaced by
    //    the property name on the instance or its embedded object. All
    //    the class relationships (scoping, etc) have been resolved to actual
    //    property names on the instance.
    //    a) Note that '*' is not replaced.
    //    b) The API will always prepend the instance's class name

// ATTN: UNCOMMENT when API is available
    CQLChainedIdentifier normalizedId; // = val.getNormalizedIdentifier(inCI, *_ctx);

    Array<CQLIdentifier> ids = normalizedId.getSubIdentifiers();

    PropertyNode * curNode = rootNode.get();
    PropertyNode * curChild = curNode->firstChild.get();

    for (Uint32 j = 1; j < ids.size(); j++)
    {
      if (ids[j].isWildcard())
      {
	curNode->wildcard = true;
	break;
      }

      Boolean found = false;
      while (curChild != NULL && !found)
      {
	if (curChild->name == ids[j].getName())
        {
	  found = true;
	}
	else
        {
	  curChild = curChild->sibling.get();
	}
      }

      if (!found)
      {
	/*
	PEGASUS_STD(cout) << "adding-" << ids[j].getName().getString() 
			  << PEGASUS_STD(endl);
	PEGASUS_STD(cout) << "parent is -" << curNode->name.getString() << PEGASUS_STD(endl);
	if (curNode->firstChild.get() != NULL)
	  PEGASUS_STD(cout) << "sibling is -" << curNode->firstChild->name.getString()
			    << PEGASUS_STD(endl);
	*/
	curChild = new PropertyNode;
	curChild->sibling = curNode->firstChild;
	curChild->name = ids[j].getName();
	curChild->wildcard = false;
	curNode->firstChild.reset(curChild);  // safer than using the = operator
      }

      curNode = curChild;
      curChild = curNode->firstChild.get();
    }
  }

  Array<CIMName> requiredProps;

  PropertyNode* childNode = rootNode->firstChild.get();
  while (childNode != NULL)
  {
    /*
    PEGASUS_STD(cout) << "applying to  " << childNode->name.getString() 
		      << PEGASUS_STD(endl);
    if (childNode->wildcard)     
      PEGASUS_STD(cout) << "is wildcard "  << PEGASUS_STD(endl);
    */

    if (childNode->firstChild.get() != NULL)
    {
      /*
      PEGASUS_STD(cout) << "has first child  " << childNode->firstChild->name.getString()
			<< PEGASUS_STD(endl);
      */

      Uint32 index = inCI.findProperty(childNode->name);
      CIMProperty childProp = inCI.getProperty(index);
      inCI.removeProperty(index);
      applyProjection(childNode, childProp);
      inCI.addProperty(childProp);
    }

    // ATTN: what to do about selecting one element of an array.  Is this allowed
    // in basic, and if so, then it would cause a property type change.  
    // Line 461.  May need to call CQLValue to figure this out.

    // ATTN: Line 636-640 - does this only apply to basic?

    if (!rootNode->wildcard)
      requiredProps.append(childNode->name);   
 
    childNode = childNode->sibling.get();
  }

  if (!rootNode->wildcard)
    removeUnneededProperties(inCI, requiredProps);
}

void CQLSelectStatementRep::applyProjection(PropertyNode* node,
					    CIMProperty& nodeProp)
{
PEGASUS_STD(cout) << "applying to " << node->name.getString() << PEGASUS_STD(endl);

  PEGASUS_ASSERT(node->firstChild.get() != NULL);

  CIMInstance nodeInst;
  CIMValue nodeVal = nodeProp.getValue();
// ATTN - UNCOMMENT when emb objs are supported
//PEGASUS_ASSERT(nodeVal.getType() == CIMTYPE_INSTANCE);
//nodeVal.get(nodeInst);

  Array<CIMName> requiredProps;

  PropertyNode * curChild = node->firstChild.get();
  while (curChild != NULL)
  {
    // PEGASUS_STD(cout) << "has child  " << curChild->name.getString() << PEGASUS_STD(endl);

    if (curChild->firstChild.get() != NULL)
    {
      /*
      PEGASUS_STD(cout) << "child has first child  " << 
	curChild->firstChild->name.getString() << PEGASUS_STD(endl);
      */

      Uint32 index = nodeInst.findProperty(curChild->name);
      CIMProperty childProp = nodeInst.getProperty(index); 
      nodeInst.removeProperty(index);
      applyProjection(curChild, childProp);
      nodeInst.addProperty(childProp);
    }

    // ATTN: what to do about selecting one element of an array.  Is this allowed
    // in basic, and if so, then it would cause a property type change.  
    // Line 461.  May need to call CQLValue to figure this out.

    if (!node->wildcard)
    {
      requiredProps.append(curChild->name);
    }

    curChild = curChild->sibling.get();
  }

  if (!node->wildcard)
    removeUnneededProperties(nodeInst, requiredProps);

// ATTN - UNCOMMENT when emb objs are supported
//CIMValue newNodeVal(nodeInst);
//nodeProp.setValue(newNodeVal);
} 

void CQLSelectStatementRep::removeUnneededProperties(CIMInstance& inst, 
						     Array<CIMName>& requiredProps)
{
  for (Uint32 i = 0; i < inst.getPropertyCount(); i++)
  {
    Boolean found = false;
    for (Uint32 j = 0; j < requiredProps.size(); j++)
    {
      if (inst.getProperty(i).getName() == requiredProps[j])
      {
	found = true;
	break;
      }  
    }

    if (!found)
    {
      inst.removeProperty(i);
    }
  }
}

void CQLSelectStatementRep::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
  Array<CQLIdentifier> fromList = _ctx->getFromList();
  PEGASUS_ASSERT(fromList.size() == 1);  // no joins yet

  if (!inClassName.getClassName().equal(fromList[0].getName()))
  {
    throw Exception("TEMP MSG:  not in the FROM list ");
  }

  try
  {
    _ctx->getClass(inClassName.getClassName());
  }
  catch (CIMException& ce)
  {
    if (ce.getCode() == CIM_ERR_INVALID_CLASS || 
	ce.getCode() == CIM_ERR_NOT_FOUND)
    {
      // ATTN may just want to throw the CIMException rather than
      // CQL exception
      throw Exception("TEMP MSG - class does not exist");
    }
  }
}

//
// Validates that all the chained identifiers in the statement meet
// the rules in the CQL spec vs.the class definitions in the repository
//
void CQLSelectStatementRep::validateProperties() throw(Exception)
{
  // assumes applyContext has been called

  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    validateProperty(_selectIdentifiers[i]);
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    validateProperty(_whereIdentifiers[i]);
  }
}

//
// Validates that the chained identifier meets all the rules in the CQL
// spec vs.the class definitions in the repository
//
void CQLSelectStatementRep::validateProperty(CQLChainedIdentifier& chainId)
{
  // assumes that applyContext has been called

  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  // Determine the starting class context.
  // See CQLIdentifier::applyContext for a description
  // of the form of the identifier after applyContext.
  CIMName prevContext;
  Uint32 startingPos = 0;
  if (ids[0].isScoped())
  { 
    // The first element is scoped, therefore the FROM class
    // was not prepended.  Get the starting context from the FROM
    Array<CQLIdentifier> fromList = _ctx->getFromList();
    PEGASUS_ASSERT(fromList.size() == 1);   // no joins yet
    prevContext = fromList[0].getName();
  }
  else
  {
    // First element was not scoped, therefore the FROM class
    // was prepended.  Use the first element as the starting context.
    prevContext = ids[0].getName();
    startingPos = 1;
  }
  
  for (Uint32 pos = startingPos; pos < ids.size(); pos++)
  {
    // Determine the current class context
    CIMName curContext;
    if (ids[pos].isScoped())
    {
      // The chain element is scoped.  Lookup the scope
      // name in the FROM list, taking into account
      // aliases.
      curContext = lookupFromClass(ids[pos].getScope());
      if (curContext.getString() == String::EMPTY)
      {
	// It was not in the FROM list, just use the scope name.
	curContext = CIMName(ids[pos].getScope());
      }      
    }
    else
    {
      PEGASUS_ASSERT(pos == startingPos || ids[pos].isWildcard());
      curContext = prevContext;
    }

    // Get the class definition of the current class context
    CIMClass classDef;
    try 
    {
      classDef = _ctx->getClass(curContext);
    }
    catch (CIMException& ce)
    {
      if (ce.getCode() == CIM_ERR_NOT_FOUND ||
	  ce.getCode() == CIM_ERR_INVALID_CLASS)
      {
	// ATTN: better to just throw the CIMException rather
	// than the CQL exception?
	throw Exception("TEMP_MSG: class context does not exist");
      }
    }

    // Determine if the property name at the current position
    // exists on the current class context.
    if (classDef.findProperty(ids[pos].getName()) == PEG_NOT_FOUND)
    {
      throw Exception("TEMP MSG: prop not on scoped class");
    }

    // Checking class relationship rules in section 5.4.1.
    if (!curContext.equal(prevContext))
    {
      if (!isSubClass(curContext, prevContext) &&
	  !isSubClass(prevContext, curContext))
      {
	throw Exception("TEMP MSG: section 5.4.1 violation!");
      }
    }

    prevContext = curContext;
  }
}

CIMName CQLSelectStatementRep::lookupFromClass(const String&  lookup)
{
  CQLIdentifier id = _ctx->findClass(lookup);

  return id.getName();
}

Array<CIMObjectPath> CQLSelectStatementRep::getClassPathList()
{
  Array<CQLIdentifier> ids = _ctx->getFromList();
  PEGASUS_ASSERT(ids.size() == 1);  // no joins yet

  // No wbem-uri support yet.
  CIMObjectPath path(String::EMPTY, _ctx->getNamespace(), ids[0].getName());

  Array<CIMObjectPath> paths;
  paths.append(path);

  return paths;
}

CIMPropertyList CQLSelectStatementRep::getPropertyList(const CIMObjectPath& inClassName)
{
  // assumes that applyContext had been called.

  // check if namespace matches default namespace?
  
  // No wbem-uri support yet
  CIMName className = inClassName.getClassName();
  CIMClass theClass = _ctx->getClass(className);
 
  Boolean isWildcard;
  Array<CIMName> reqProps;
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    isWildcard = addRequiredProperty(reqProps, theClass, _selectIdentifiers[i]);
    
    if (isWildcard)
    {
      return CIMPropertyList();
    }
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    isWildcard = addRequiredProperty(reqProps, theClass, _whereIdentifiers[i]);

    PEGASUS_ASSERT(!isWildcard);
  }

  Uint32 propCnt = theClass.getPropertyCount();
  Boolean allProps = true;
  for (Uint32 i = 0; i < propCnt; i++)
  {
    if (!containsProperty(theClass.getProperty(i).getName(), reqProps))
    {
      allProps = false;
      break;
    }
  }

  if (allProps)
  {
    return CIMPropertyList();
  }
  else
  {
    return CIMPropertyList(reqProps);
  }
}

Boolean CQLSelectStatementRep::addRequiredProperty(Array<CIMName>& reqProps,
						     CIMClass& theClass,
						     CQLChainedIdentifier& chainId)
{
  // Assumes that applyContext had been called
  // Does not look at properties on embedded objects

  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  // see the description of CQLIdentifier::applyContext

  if (ids[0].isScoped())
  {
    PEGASUS_ASSERT(!ids[0].isWildcard());

    // Check if the scoped property is exposed by the class passed in.
    if (theClass.findProperty(ids[0].getName()) != PEG_NOT_FOUND) 
    {
      CIMName scopingClass = lookupFromClass(ids[0].getScope());
      if (scopingClass.getString() == String::EMPTY)
      {
	scopingClass = CIMName(ids[0].getScope());
      }    

      // Check if the scoping class is a subclass of the class passed in
      Boolean sub  = isSubClass(scopingClass, theClass.getClassName());
      
      // Add to the required properties only if the scoping class 
      // is not a subclass of the class passed in
      if (!sub)
      {
	if (!containsProperty(ids[0].getName(), reqProps))
	{  
	  reqProps.append(ids[0].getName());
	}
      }
    }
  }
  else
  {
    if (ids[1].isWildcard())
    {
      return true;
    }

    if (theClass.findProperty(ids[1].getName()) != PEG_NOT_FOUND) 
    {
      if (!containsProperty(ids[1].getName(), reqProps))
      {  
	reqProps.append(ids[1].getName());
      }
    }
  }
  
  return false;
}

Boolean CQLSelectStatementRep::containsProperty(const CIMName& name,
						const Array<CIMName>& props) 
{
  for (Uint32 i = 0; i < props.size(); i++)
  {
    if (props[i] == name)
    {
      return true;
    }
  }

  return false;
}

Boolean CQLSelectStatementRep::isSubClass(const CIMName& derived,
					  const CIMName& base)
{
  Array<CIMName> subClasses = _ctx->enumerateClassNames(base);
  for (Uint32 i = 0; i < subClasses.size(); i++)
  {
    if (subClasses[i] == derived)
    {
      return true;
    }	
  }
  
  return false;
}

void CQLSelectStatementRep::appendClassPath(const CQLIdentifier& inIdentifier)
{
  _ctx->insertClassPath(inIdentifier);
}

void CQLSelectStatementRep::setPredicate(CQLPredicate inPredicate)
{
  _predicate = inPredicate;
}

void CQLSelectStatementRep::insertClassPathAlias(const CQLIdentifier& inIdentifier, String inAlias)
{
  _ctx->insertClassPath(inIdentifier,inAlias);
}

void CQLSelectStatementRep::appendSelectIdentifier(const CQLChainedIdentifier& x)
{
  _selectIdentifiers.append(x);
}

Boolean CQLSelectStatementRep::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
  _whereIdentifiers.append(x);
  return true;
}

void CQLSelectStatementRep::applyContext()
{
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    _selectIdentifiers[i].applyContext(*_ctx);
    checkWellFormedIdentifier(_selectIdentifiers[i], true);
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    _whereIdentifiers[i].applyContext(*_ctx);
    checkWellFormedIdentifier(_whereIdentifiers[i], false);
  }

  if (hasWhereClause())
  {
    _predicate.applyContext(*_ctx);
  }

}

void CQLSelectStatementRep::checkWellFormedIdentifier(const CQLChainedIdentifier& chainId,
						      Boolean isSelectListId)
{
  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  Uint32 startingPos = 0;

  if (!ids[0].isScoped())
  {
    if (ids.size() == 1 && isSelectListId)
    {
      throw Exception("TEMP MSG: need to select a property on the FROM class");
    }
    
    startingPos = 1;
  }

  for (Uint32 pos = startingPos; pos < ids.size(); pos++)
  {  
    if (ids[pos].isArray() && isSelectListId)
    {
      throw Exception("TEMP MSG: array indexing not allowed in basic select");
    }

    if (ids[pos].isSymbolicConstant() && isSelectListId)
    {
      throw Exception("TEMP MSG: array indexing not allowed in basic select");
    }

    if (ids[pos].isWildcard())
    {
      if ( !isSelectListId)
      {
	throw Exception("TEMP MSG: wildcard not allowed in WHERE clause");
      }

      if ( pos != ids.size())
      {
	throw Exception("TEMP MSG: wildcard must be at the end of select-list property");
      }
    }

    if (pos > startingPos && !ids[pos].isWildcard())
    {
      if (!ids[pos].isScoped())
      {
	throw Exception("TEMP MSG: property on embedded object must be scoped");
      }
    }
  }
}

void CQLSelectStatementRep::normalizeToDOC()
{

}

String CQLSelectStatementRep::toString()
{
    printf("CQLSelectStatementRep::toString()\n");
	String s("SELECT ");
	for(Uint32 i = 0; i < _selectIdentifiers.size(); i++){
		if((i > 0) && (i < _selectIdentifiers.size())){
			s.append(",");
		}
		s.append(_selectIdentifiers[i].toString());
	}	
	s.append(" FROM ");
	Array<CQLIdentifier> _ids = _ctx->getFromList();
	for(Uint32 i = 0; i < _ids.size(); i++){
		if((i > 0) && (i < _ids.size())){
                        s.append(",");
                }
		s.append(_ids[i].toString());
	}
	if(_hasWhereClause){
		s.append(" WHERE ");
		s.append(_predicate.toString());
	}
	return s;
}

void CQLSelectStatementRep::setHasWhereClause()
{
        _hasWhereClause = true;
}

Boolean CQLSelectStatementRep::hasWhereClause()
{
        return _hasWhereClause;
}

void  CQLSelectStatementRep::clear()
{
	_ctx->clear();
}

PEGASUS_NAMESPACE_END
