//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "SampleFamilyProvider.h"
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>


PEGASUS_NAMESPACE_BEGIN

SampleFamilyProvider::SampleFamilyProvider(void)
{
}

SampleFamilyProvider::~SampleFamilyProvider(void)
{
}


String _toStringPropertyList(const CIMPropertyList& pl)
{
    String tmp;
    for (Uint32 i = 0; i < pl.size() ; i++)
    {
        if (i > 0)
            tmp.append(", ");
        tmp.append(pl[i].getString());
    }
    return(tmp);
}
String _showPropertyList(const CIMPropertyList& pl)
{
    if (pl.isNull())
        return("NULL");

    String tmp = "size= ";

    tmp.append((pl.size() == 0) ? "Empty" : _toStringPropertyList(pl));
    return(tmp);
}

/** determine of the the input property is in the list.
    @param property list
    @param name of property to compare
*/
Boolean _returnThisProperty(const CIMPropertyList& pl, const CIMName& pn)
{
    if (pl.isNull())
        return(true);
    if (pl.size() == 0)
        return(false);

	for (Uint32 i = 0; i < pl.size() ; i++)
	{
		// if name found in propertyList return true
		if (pn.equal(pl[i].getString()))
        {
			return(true);
        }
	}
	return(false);
}

CIMInstance _filterProperties(const CIMInstance& instance , const CIMPropertyList& pl)
{
    // Null means send all properties.  Return original instance.
    if (pl.isNull())
        return instance;

    // make copy of instance.
    CIMInstance rtnInstance = instance.clone();
    
    // remove properties not in property list from cloned copy
    for (Uint32 i = 0 ; i < instance.getPropertyCount() ; i++)
    {
        CIMName n = instance.getProperty(i).getName();
        if (!_returnThisProperty(pl, n))
        {
            Uint32 pos = rtnInstance.findProperty(n);
            if (pos != PEG_NOT_FOUND)
                rtnInstance.removeProperty(pos);
        }
    }
    return(rtnInstance);
}

/** _filterInstancesToTargetPaths - Takes as input a list of associations along with the
    filters and returns the CIMObjectpaths that pass the filters.  This function is intended
    to operate on the list returned from the references and filter on the result class information.
    @param assocInstance - The association instance being processed.
    @param targetObjectPath - The original target. This is required since this is the one reference
    we don't want.
    @resultClass - The resultClass we want to filter on
    @resultRole  - The result role we want to filter on
    @return - returns the CIMObjectPaths that represent the other side of associations that pass the
    filters.
 */
Array<CIMObjectPath> _filterAssocInstanceToTargetPaths(CIMInstance & assocInstance, CIMObjectPath & targetObjectPath,
            CIMName resultClass, String resultRole)
{
    Array<CIMObjectPath> returnPaths;
    // get all reference properties except for target.
    for (Uint32 i = 0 ; i < assocInstance.getPropertyCount() ; i++)
    {
        CIMProperty p = assocInstance.getProperty(i);
        CDEBUG("_FilterAssocInstance. property name = " << p.getName().getString());
        if (p.getType() == CIMTYPE_REFERENCE)
        {
            CIMValue v = p.getValue();
            CIMObjectPath path;
            v.get(path);
            
            if (!targetObjectPath.identical(path))
            {
               CDEBUG("Test resultClass. resultClass= " << resultClass << " Path = " << path.toString());
               CDEBUG("Test resultClass. resultClass= " << resultClass << " Path Class= " << path.getClassName().getString());
                if (resultClass.isNull() || resultClass == path.getClassName())
                {
                    CDEBUG("Test resultRole. resultRole= " << resultRole << " property Name= " << p.getName().getString());
                    if (resultRole == String::EMPTY || p.getName().getString() == resultRole)
                    {
                        CDEBUG("Append path " << path.toString());
                        returnPaths.append(path);
                    }
                }
            }
        }
    }
    return( returnPaths );
}


/** Filter the valid CIMReferences from an association instance. If there is a role 
    property, gets all but the role property.
     @param target - The target class for the association
     @param instance - The association class instance we are searching for references
     @param role - The role we require. I there is no role, this is String::EMPTY
    @return - Objectpath from the instances reference properties that
            match the target and role.
 * 
*/
Boolean _isInstanceValidReference(const CIMObjectPath& target, CIMInstance& instance, const String& role)
{

    // Test if role parameter is valid property.
    Uint32 pos;
    CDEBUG("_FilterReferenceProperties. role= " << role);
    if (role != String::EMPTY)
    {
        // Test if property exists.
        if ((pos = instance.findProperty(role)) == PEG_NOT_FOUND)
        {
            CDEBUG("Invalid Param Exception");
            throw CIMException(CIM_ERR_INVALID_PARAMETER);
        }
     
         //Now check to be sure this is a reference property
         // This test may not be necessary. Combine it into the loop.
         //const CIMProperty p = instance.getProperty(pos);
         if (instance.getProperty(pos).getType() != CIMTYPE_REFERENCE)
         {
             throw CIMException(CIM_ERR_INVALID_PARAMETER);
         }
    }

    //Now search instance for all reference properties
    for (Uint32 j = 0; j < instance.getPropertyCount() ; j++)
    {
        const CIMProperty p = instance.getProperty(j);
        CDEBUG("Search for Reference type properties " << j);
        if (p.getType() == CIMTYPE_REFERENCE)
        {
            // If there is no role or the role is the same as this property name
            CDEBUG("Test for valid role. role= " << role << " property= " << p.getName());
            CIMValue v = p.getValue();
            CIMObjectPath path;
            v.get(path);
            // NOTE WE must be namespace and host free here.
            if ((role == String::EMPTY) || (role == p.getName().getString()))
            {
                CDEBUG("_FilterReferenceProperties. target= " << target.toString() << " path= " << path.toString());
                if (target.identical(path))
                    return(true);
            }
        }
    }
    return( false );
}

Boolean _isValidObjectClass(CIMObjectPath)
{
    return true;
}


// filterReferenceNames is a common filter that is used to
// filter the set of possible instances to return against the
// possible filters provided with the reference and reference
// names operations.  It returns the index to object that pass
// the set of filter tests.
Array<CIMInstance> _filterReferenceNames(
    const Array<CIMInstance>& targetAssociationInstanceList,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role)
{
    // Build a host and namespace independent reference to the target objectname
    CDEBUG("_filterReferenceNames. objName= " << objectName.toString() << " resultClass= " << resultClass << " role= " << role);
    CIMObjectPath targetReference = CIMObjectPath(
    String(),
    CIMNamespaceName(),
    objectName.getClassName(),
    objectName.getKeyBindings());

    Array<CIMInstance> foundList;

    // Note with the new rules, the associaiton class should never be NULL.
    // We get the association class to determine what we deliver.
    if (resultClass.isNull())
    {
       return (foundList);
    }
    for (Uint32 i = 0 ; i < targetAssociationInstanceList.size() ; i++)
    {
        CIMInstance instance = targetAssociationInstanceList[i];
        if (_isInstanceValidReference(targetReference, instance, role))
        {
            // DELETE THISCDEBUG("_FilterReferenceNames Moving Object named " << tmp[j].toString());
            foundList.append(instance);
        }
    }
    return( foundList );
}

CIMInstance _buildPersonDynamicInstance(String& data, String& secondProperty, Uint32 counterProperty)
{
    CIMInstance instance("TST_PersonDynamic");
    instance.addProperty(CIMProperty("Name", data));   // key
    instance.addProperty(CIMProperty("secondProperty", secondProperty));
    instance.addProperty(CIMProperty("instanceCounter", counterProperty));
    return(instance);
}

CIMInstance _buildInstanceLineageDynamic(CIMObjectPath parent, CIMObjectPath child)
{
    // Note that this is a nasty assumption. May be a different namespace. Need
    // to build dynamically
    // This namespacename is used to set the namespace in the references.
    // We should set it dynamically.
    
    //String nameSpace = "root/SampleProvider";
    //String host = System::getHostName();

    CIMName thisClassReference = "TST_PersonDynamic";
    CIMName assocClassName = "TST_LineageDynamic";
    CIMInstance instance(assocClassName);
    //parent.setHost(host);
    //parent.setNameSpace(nameSpace);
    instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
    instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
    return(instance);

}

CIMInstance _buildInstanceLabeledLineageDynamic(CIMObjectPath parent, CIMObjectPath child, String label)
{
    // Note that this is a nasty assumption. May be a different namespace. Need
    // to build dynamically
    // This namespacename is used to set the namespace in the references.
    // We should set it dynamically.
    
    CIMName thisClassReference = "TST_PersonDynamic";
    CIMName assocClassName = "TST_LabeledLineageDynamic";
    CIMInstance instance(assocClassName);
    instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
    instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
    instance.addProperty(CIMProperty("label", label));
    return(instance);

}


void SampleFamilyProvider::initialize(CIMOMHandle & cimom)
{

    CDEBUG ("initialize");
	_cimom = cimom;
    {   
        String nameSpace = "root/SampleProvider";
        /*
        // Create the classes so we have something to build from. This should not have to 
        // be here but we cannot get to repository yet.
        */
        
        String refClassName = "TST_PersonDynamic";
        String tst_PersonClassName = "TST_Person";

        //CIMClass class1(CIMName(refClassName), CIMName(tst_PersonClassName));
        CIMClass class1(CIMName("TST_PersonDynamic"), CIMName("TST_Person"));
        class1
            .addProperty(CIMProperty (CIMName ("Name"), String())
                .addQualifier(CIMQualifier ( CIMName ("Key"), true)));
        
        class1
            .addProperty(CIMProperty (CIMName ("secondProperty"), String()));
        
        class1
            .addProperty(CIMProperty (CIMName ("counter"), Uint32(0)));

        _referencedClass = class1;

		//XmlWriter::printClassElement(_referencedClass);
		//MofWriter::printClassElement(_referencedClass);

        // Create the association class
        CIMClass a1("TST_LineageDynamic");

        // ATTN: Karl - THis generates a value reference with nothing in it for the classname. WHY?
        CIMValue cv = CIMObjectPath();
        CIMProperty p1(CIMName ("parent"), CIMObjectPath(), 0, CIMName(refClassName));
        p1.addQualifier(CIMQualifier(CIMName ("Key"), true));

        CIMProperty p2(CIMName ("child"), CIMObjectPath(),0, CIMName(refClassName));
        p2.addQualifier(CIMQualifier(CIMName ("Key"), true));

        a1
        .addQualifier(CIMQualifier(CIMName ("association"), true))
        .addProperty(CIMProperty(p1))
        .addProperty(CIMProperty(p2));
        _assocClass = a1;

        // ATTN KS the following is just a hack to get a class for the lineagelabeled assoc
        CIMClass c2;
        try
        {
        c2 = _cimom.getClass(
            OperationContext(),
            nameSpace,
            CIMName("TST_LabeledLineageDynamic"),
            false,
            true,
            true,
            CIMPropertyList());
        }
        catch(CIMException& e)
        {
            CDEBUG("Exception hit " << e.getMessage());
            // ATTN: KS 20030303 - Add an exception return here.
        }

        _assocLabeledClass = c2;
        CDEBUG ("Initialize - Association Class built");
		//XmlWriter::printClassElement(_assocLabeledClass);
		//MofWriter::printClassElement(_assocLabeledClass);

    }
    // Build instances of the referenced class
    String _father("Father");
    String _mother("Mother");
    String _son1("Son1");
    String _son2("Son2");
    String _daughter1("Daughter1");
    String _daughter2("Daughter2");
    String _one("one");
    String _two("two");
    String _three( "three");
    String _four( "four");
    String _five( "five");
    Uint32 Father = _instances.size();
    _instances.append(_buildPersonDynamicInstance(_father, _one, 1));
    Uint32 Mother = _instances.size();
    _instances.append(_buildPersonDynamicInstance(_mother, _two, 2));
    Uint32 Son1 = _instances.size();
    _instances.append(_buildPersonDynamicInstance(_son1, _three, 3));
    Uint32 Son2 = _instances.size();
    _instances.append(_buildPersonDynamicInstance(_son2, _four, 4));
    Uint32 Daughter1 = _instances.size();
    _instances.append(_buildPersonDynamicInstance(_daughter1, _five, 5));

    CDEBUG ("initialize - referenced Class Instances Built");
    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
        // Create Instance Names
        _instanceNames.append(_instances[i].buildPath(_referencedClass));
    }
    _delay = 3000;
    CDEBUG ("initialize - referenced Class Instance Names Built using buildpath");
    //
    // Now make the instances for the associations
    //
    CDEBUG ("Initialise - Building Assoc Class instances");
    {
        CIMName thisClassReference = "TST_PersonDynamic";
        CIMName assocClassName = "TST_LineageDynamic";
        
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Father],_instanceNames[Son1]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Father],_instanceNames[Son2]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Father],_instanceNames[Daughter1]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Mother],_instanceNames[Son1]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Mother],_instanceNames[Son2]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Mother],_instanceNames[Daughter1]));
      
        for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
        {
            // Create Instance Names
            _instanceNamesLineageDynamic.append(_instancesLineageDynamic[i].buildPath(assocClassName));
        }
        CDEBUG("LineageDynamic " << _instancesLineageDynamic.size() << " instances ");

    }
    {
        CIMName thisClassReference = "TST_PersonDynamic";
        CIMName assocLabeledClassName = "TST_LabeledLineageDynamic";
        
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Father],_instanceNames[Son1],String("one")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Father],_instanceNames[Son2],String("two")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Father],_instanceNames[Daughter1],String("three")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Mother],_instanceNames[Son1],String("four")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Mother],_instanceNames[Son2],String("five")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Mother],_instanceNames[Daughter1],String("six")));
      
        for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
        {
            // Create Instance Names
            _instanceNamesLabeledLineageDynamic.append(
                _instancesLabeledLineageDynamic[i].buildPath(assocLabeledClassName));
        }
        CDEBUG("LabeledLineageDynamic " << _instancesLabeledLineageDynamic.size() << " instances ");

    }
}

void SampleFamilyProvider::terminate(void)
{
    
    // TODO Additional cleanup needed
    CDEBUG("Terminate SampleFamilyProvider ");
}

void SampleFamilyProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    // convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		CIMNamespaceName(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();
    CIMName myClass = instanceReference.getClassName();

    CDEBUG("PropertyList = " << _showPropertyList(propertyList));

    if (myClass == CIMName("tst_persondynamic"))
    {
    	// instance index corresponds to reference index
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
    		if(localReference == _instances[i].buildPath(_referencedClass))
    		{
    			// deliver requested instance
    			handler.deliver(_filterProperties(_instances[i], propertyList));
    			break;
    		}
    	}
    }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
        for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		if(localReference == _instances[i].buildPath(_assocClass))
    		{
    			// deliver requested instance
    			handler.deliver(_filterProperties(_instancesLineageDynamic[i], propertyList));
    			break;
    		}
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		if(localReference == _instances[i].buildPath(_assocLabeledClass))
    		{
    			// deliver requested instance
    			handler.deliver(_filterProperties(_instancesLabeledLineageDynamic[i],propertyList));
    			break;
    		}
    	}
    }

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    CDEBUG("enumerateInstances" << " PropertyList = " << _showPropertyList(propertyList));
    CIMNamespaceName nameSpace = classReference.getNameSpace();


    // begin processing the request
	handler.processing();

    CIMName myClass = classReference.getClassName();

    if (myClass == CIMName("tst_persondynamic"))
    {
        for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            handler.deliver(_filterProperties(_instances[i], propertyList));
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
        for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		handler.deliver(_filterProperties(_instancesLineageDynamic[i], propertyList));
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		handler.deliver(_filterProperties(_instancesLabeledLineageDynamic[i], propertyList));
    	}
    }

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
	CDEBUG("Enumerate InstanceNames of " << classReference.toString());
    // begin processing the request
	handler.processing();

    CIMName myClass = classReference.getClassName();
    CIMClass cimClass;
	try
    {
    cimClass = _cimom.getClass(
		OperationContext(),
		classReference.getNameSpace(),
		classReference.getClassName(),
		false,
		true,
		true,
		CIMPropertyList());
    }
    catch(CIMException& e)
    {
        CDEBUG("Exception hit " << e.getMessage());
		throw CIMException(CIM_ERR_NOT_FOUND);
    }

    //XmlWriter::printClassElement(_referencedClass);
    // ATTN: Use the above to check the existence of the class. Note that we use it in only
    // one place for the moment.  Update to cover the others.
    CIMNamespaceName nameSpace = classReference.getNameSpace();

    CDEBUG("EnumerateInstanceNames for class = " << myClass);

    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            handler.deliver(_instances[i].buildPath(cimClass));
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
    		handler.deliver(_instancesLineageDynamic[i].buildPath(_assocClass));
    	}

    }
    //ATTN KS This one is in error since we did not build the corresponding instances.
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
    		handler.deliver(_instancesLabeledLineageDynamic[i].buildPath(_assocLabeledClass));
    	}

    }
    // complete processing the request
    handler.complete();
}

void SampleFamilyProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
    CIMName myClass = instanceReference.getClassName();
	
    CIMObjectPath localReference = CIMObjectPath(
		String(),
		CIMNamespaceName(),
		myClass,
		instanceReference.getKeyBindings());
	
	// begin processing the request
	handler.processing();
    Boolean instanceFound = false;

    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            if(localReference == _instances[i].buildPath(_referencedClass))
            {
                // overwrite existing instance
                _instances[i] = instanceObject;
                instanceFound = true;
                break;
            }
    	}

     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLineageDynamic[i].buildPath(_assocClass))
            {
                // overwrite existing instance
                _instancesLineageDynamic[i] = instanceObject;
                instanceFound = true;
                break;
            }
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
            // overwrite existing instance
            _instancesLabeledLineageDynamic[i] = instanceObject;
            instanceFound = true;
            break;
    	}

    }
	if (!instanceFound)
		throw CIMException(CIM_ERR_NOT_FOUND);

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
    CIMName myClass = instanceReference.getClassName();
    
    // convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	
    CIMObjectPath localReference = CIMObjectPath(
		String(),
		CIMNamespaceName(),
		myClass,
		instanceReference.getKeyBindings());
	
    handler.processing();
	
    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            if(localReference == _instances[i].buildPath(_referencedClass))
            {
                throw CIMObjectAlreadyExistsException(
                                      localReference.toString());
            }
    	}
        // add the new instance to the array
        _instances.append(instanceObject);
        _instanceNames.append(instanceReference);

        // deliver the new instance
        handler.deliver(_instanceNames[_instanceNames.size() - 1]);
     }

    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLineageDynamic[i].buildPath(_assocClass))
            {
                throw CIMObjectAlreadyExistsException(
                                      localReference.toString());
            }
    	}
        // add the new instance to the array
        _instancesLineageDynamic.append(instanceObject);
        _instanceNamesLineageDynamic.append(instanceReference);

        // deliver the new instance
        handler.deliver(_instanceNamesLineageDynamic[_instanceNamesLineageDynamic.size() - 1]);

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLabeledLineageDynamic[i].buildPath(_assocLabeledClass))
            {
                throw CIMObjectAlreadyExistsException(
                                      localReference.toString());
            }
    	}
        // add the new instance to the array
        _instancesLabeledLineageDynamic.append(instanceObject);
        _instanceNamesLabeledLineageDynamic.append(instanceReference);

        // deliver the new instance
        handler.deliver(_instanceNamesLabeledLineageDynamic[_instanceNamesLabeledLineageDynamic.size() - 1]);
    }

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		CIMNamespaceName(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());
	
	// begin processing the request

    Boolean instanceFound = false;
	handler.processing();
    CIMName myClass = instanceReference.getClassName();
    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            if(localReference == _instances[i].buildPath(_referencedClass))
            {
			// remove instance from the array
			_instances.remove(i);
			_instanceNames.remove(i);
            instanceFound = true;
            }
			// exit loop
			break;
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLineageDynamic[i].buildPath(_assocClass))
            {
                // remove instance from the array
                _instancesLineageDynamic.remove(i);
                _instanceNamesLineageDynamic.remove(i);
                instanceFound = true;
            }

			// exit loop
			break;
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLabeledLineageDynamic[i].buildPath(_assocLabeledClass))
            {
                // remove instance from the array
                _instancesLabeledLineageDynamic.remove(i);
                _instanceNamesLabeledLineageDynamic.remove(i);
                instanceFound = true;
            }

            // exit loop
            break;
    	}
    }
	if (!instanceFound)
		throw CIMException(CIM_ERR_NOT_FOUND);
	handler.complete();
}


void SampleFamilyProvider::associators(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler)
{
	CDEBUG("Associators object= " << objectName.toString() << " associationClass= " << associationClass.getString() << " resultClass= " << resultClass.getString());
    // begin processing the request
    // Get the namespace and host names to create the CIMObjectPath

    CDEBUG("PropertyList = " << _showPropertyList(propertyList));

    String host = System::getHostName();
    CIMNamespaceName nameSpace = objectName.getNameSpace();
    CIMName myClass = objectName.getClassName();
	CIMObjectPath localObjectName = CIMObjectPath(
		String(),
		CIMNamespaceName(),
		objectName.getClassName(),
		objectName.getKeyBindings());

    if (associationClass == CIMName("TST_LineageDynamic"))
    {
        // Filter out the required objectpaths from the association list.
        Array<CIMInstance> assocInstances;
        assocInstances = _filterReferenceNames(_instancesLineageDynamic,
                                                   objectName,
                                                   associationClass,
                                                   role);
    
        for (Uint32 i = 0 ; i < assocInstances.size() ; i++)
        {
            Array<CIMObjectPath> resultPaths;
            resultPaths = _filterAssocInstanceToTargetPaths(assocInstances[i], localObjectName, resultClass, resultRole);

            for (Uint32 i = 0 ; i < resultPaths.size() ; i++)
            {

                // instance index corresponds to reference index
                for(Uint32 i = 0, n = _instances.size(); i < n; i++)
                {
                    CIMObjectPath newPath = _instances[i].buildPath(_referencedClass);
                    CDEBUG("findInstanceForpath \n    newPath= " << newPath.toString() << " count " << i << "\n    localObjectName= " << localObjectName.toString());
                    if(localObjectName.identical(newPath))
                    {
                        // deliver requested instance
                        CDEBUG("FoundInstanceForPath returns Instance = " << i << " newPath= " << newPath.toString());
                        newPath.setHost(host);
                        newPath.setNameSpace(nameSpace);
                        _instances[i].setPath(newPath);
                        handler.deliver(_instances[i]);
                    }
                }

            }
        }
    }
	// complete processing the request
	handler.complete();
}


void SampleFamilyProvider::associatorNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	ObjectPathResponseHandler & handler)
{
    // Get the namespace and host names to create the CIMObjectPath
    String host = System::getHostName();
    CIMNamespaceName nameSpace = objectName.getNameSpace();

    CIMObjectPath localObjectName = CIMObjectPath(
        String(),
        CIMNamespaceName(),
        objectName.getClassName(),
        objectName.getKeyBindings());

	CDEBUG("AssociationNames Operation. objectName= " << objectName.toString() << " assocClass= " << associationClass  << " resultClass= " << resultClass << " role= " << role);
    
    Array<CIMInstance> assocInstances;
    
    if (associationClass == CIMName("TST_LineageDynamic"))
    {
        // Filter out the required objectpaths from the association list.
        CDEBUG("AssocNames for TST_LineageDynamic"); 
        assocInstances = _filterReferenceNames(_instancesLineageDynamic,
                                                   objectName,
                                                   associationClass,
                                                   role);
    
        for (Uint32 i = 0 ; i < assocInstances.size() ; i++)
        {
            CDEBUG("Search assocInstances. " << i);
            Array<CIMObjectPath> resultPaths;
            resultPaths = _filterAssocInstanceToTargetPaths(assocInstances[i], localObjectName, resultClass, resultRole);

            CDEBUG("rtn from _FilterAssocInst. count = " << resultPaths.size());
            for (Uint32 i = 0 ; i < resultPaths.size() ; i++)
            {
                CDEBUG("resultPath found. = " << resultPaths[i].toString());
                if (resultPaths[i].getHost().size() == 0)
                    resultPaths[i].setHost(host);

                if (resultPaths[i].getNameSpace().isNull())
                    resultPaths[i].setNameSpace(nameSpace);

                handler.deliver(resultPaths[i]);
            }
        }
    }

    if (resultClass == CIMName("TST_LabeledLineageDynamic"))
    {
        // Filter out the required objectpaths from the association list.
        assocInstances = _filterReferenceNames(_instancesLineageDynamic,
                                                   objectName,
                                                   resultClass,
                                                   role);

        for (Uint32 i = 0 ; i < assocInstances.size() ; i++)
        {
            CDEBUG("Search assocInstances. " << i);
            Array<CIMObjectPath> resultPaths;
            resultPaths = _filterAssocInstanceToTargetPaths(assocInstances[i], localObjectName, resultClass, resultRole);

            CDEBUG("rtn from _FilterAssocInst. count = " << resultPaths.size());
            for (Uint32 i = 0 ; i < resultPaths.size() ; i++)
            {
                CDEBUG("resultPath found. = " << resultPaths[i].toString());
                if (resultPaths[i].getHost().size() == 0)
                    resultPaths[i].setHost(host);

                if (resultPaths[i].getNameSpace().isNull())
                    resultPaths[i].setNameSpace(nameSpace);

                handler.deliver(resultPaths[i]);
            }
        }
    }

	// complete processing the request
	handler.complete();

}

void SampleFamilyProvider::references(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler)
{
	CDEBUG("references");
    
    CDEBUG("PropertyList = " << _showPropertyList(propertyList));

    // Get the namespace and host names to create the CIMObjectPath
    //String nameSpace = "root/SampleProvider";
    String host = System::getHostName();

    CIMNamespaceName nameSpace = objectName.getNameSpace();

    handler.processing();

    Array<CIMInstance> returnInstances;
    
    if (resultClass == CIMName("TST_LineageDynamic"))
    {
        // Filter out the required objectpaths from the association list.
        returnInstances = _filterReferenceNames(_instancesLineageDynamic,
                                                   objectName,
                                                   resultClass,
                                                   role);
    

        for (Uint32 i = 0 ; i < returnInstances.size() ; i++)
        {
            CIMObjectPath objectPath =  returnInstances[i].buildPath(_assocClass);
            if (objectPath.getHost().size() == 0)
                objectPath.setHost(host);
    
            if (objectPath.getNameSpace().isNull())
                objectPath.setNameSpace(nameSpace);
            returnInstances[i].setPath(objectPath);
            handler.deliver(returnInstances[i]);
        }
    }


    if (resultClass == CIMName("TST_LabeledLineageDynamic"))
    {
        // Filter out the required objectpaths from the association list.
        returnInstances = _filterReferenceNames(_instancesLineageDynamic,
                                                   objectName,
                                                   resultClass,
                                                   role);
    

        for (Uint32 i = 0 ; i < returnInstances.size() ; i++)
        {
            CIMObjectPath objectPath =  returnInstances[i].buildPath(_assocLabeledClass);
            if (objectPath.getHost().size() == 0)
                objectPath.setHost(host);
    
            if (objectPath.getNameSpace().isNull())
                objectPath.setNameSpace(nameSpace);
            returnInstances[i].setPath(objectPath);
            handler.deliver(returnInstances[i]);
        }
    }

	// complete processing the request
	handler.complete();
}


// Return all references(association instance names) in which the give
// object is involved.

void SampleFamilyProvider::referenceNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	ObjectPathResponseHandler & handler)
{
	CDEBUG("ReferenceNames Operation. objectName= " << objectName.toString() << " resultClass= " << resultClass << " role= " << role);
    CIMNamespaceName nameSpace = objectName.getNameSpace().getString();
    String host = System::getHostName();
    
    Array<CIMInstance> returnInstances;
    
    if (resultClass == CIMName("TST_LineageDynamic"))
    {
        // Filter out the required objectpaths from the association list.
        returnInstances = _filterReferenceNames(_instancesLineageDynamic,
                                                   objectName,
                                                   resultClass,
                                                   role);
    

        for (Uint32 i = 0 ; i < returnInstances.size() ; i++)
        {
            CIMObjectPath objectPath =  returnInstances[i].buildPath(_assocClass);
            if (objectPath.getHost().size() == 0)
                objectPath.setHost(host);
    
            if (objectPath.getNameSpace().isNull())
                objectPath.setNameSpace(nameSpace);
    
            handler.deliver(objectPath);
        }
    }

    if (resultClass == CIMName("TST_LabeledLineageDynamic"))
    {
        // Filter out the required objectpaths from the association list.
        returnInstances = _filterReferenceNames(_instancesLineageDynamic,
                                                   objectName,
                                                   resultClass,
                                                   role);

        for (Uint32 i = 0 ; i < returnInstances.size() ; i++)
        {
            CIMObjectPath objectPath =  returnInstances[i].buildPath(_assocLabeledClass);
            if (objectPath.getHost().size() == 0)
                objectPath.setHost(host);
    
            if (objectPath.getNameSpace().isNull())
                objectPath.setNameSpace(nameSpace);
    
            handler.deliver(objectPath);
        }
    }

	// complete processing the request
	handler.complete();

}


PEGASUS_NAMESPACE_END
