/** 
 * =====================================================================================
 *        Filename:  cobject.h
 *     Description:  Header file containing definition of IProperty and CObject classes.
 *         Created:  01/18/2006 
 *        Revision:  none
 *          Author:  jmisutka (06/01/19), 
 * 			
 * 			2006/01     constructor, getStringRepresentation, observer stuff, writeValue,
 *			2006/02/04	started implementing release () function (not trivial)
 *						implemented several easy functions for complex types
 *			2006/02/08  for the past several days i have had a battle with g++ and ls + collect
 *						i think that i have won at last...
 *			2006/02/20	changed constructor, addProperty has to be more clever
 *						getStringRepresentation,writeValue,release finished
 *			2006/02/21	changed getPropertyValue to non - template, bacause we can't partially specialize member functions
 *					also the way of passing return from return value to function argument, because
 *						* do not want to alloc something in kernel and dealloc elsewhere
 *						* pass by stack copying can be tricky when copying big amounts and we do not know 
 *						  in pdf
 *			2006/02/23	cobjectsimple implementation finished
 *			2006/02/23	cobjectcomplex 
 *					- getStringRepresentation
 *					- writeValue
 *					- release?, getPropertyCount
 *					- getAllPropertyNames -- problem templates virtual
 *			2006/02/24	- implementing addProperty for
 *					IProperty*. (template f. can't be virtual)
 *					all other functions, problems witch Object internal structure
 *					  not public functions etc., etc., etc.
 *					- several questions has arisen, that i cannot decide by myself
 *					- delProperty
 *
 * ------------ 2006/03/06 21:42 ----------- FUCK xpdf
 *  				- remove use of XPDF as value holders, question is Stream, but better 1 bad class
 *  				than 7.
 *  				03:24 -- CObjectSimple implemented withou XPDF
 *  				23:11 -- CObjectComplex 1st version
 * 
 *			TODO:
 *					testing
 *					?? INDIRECT -- refcounting
 *					!! CPdf::addObject and addIndObject to replace mapping* functions
 *					!! CPdf::getIndObject functions
 *					can't add IProperty when pdf is not NULL (just to simplify things)
 *					   no real obstruction for removing this
 *					allow adding indirect values (directly :))
 *					better public/protected dividing
 *					setPropertyValue -- simple (stupid) implementation
 *					release is in "undefined" behaviour :)
 *					addProperty 10x na tu istu polozku -- pdf spec. undefined value, but possible
 *					function for getting unused Ref
 *		
 *			REMARKS:
 *					-- from pRef to IProperty -- CPdf::getObject (Ref)
 *
 * =====================================================================================
 */
#ifndef COBJECT_H
#define COBJECT_H

#include <string>
#include <list>
#include <vector>
#include <deque>
#include <iostream>

#include "debug.h"
#include "iproperty.h"


//=====================================================================================
namespace pdfobjects
{


/**
 * Additional information that identifies variable type, e.g. for writeValue function.
 *
 * If someone tries to use unsupported type (pCmd,....), she should get compile error
 * because PropertyTraitSimple<> has no body.
 *
 * REMARK: BE CAREFUL when manipulating this ones.
 * It is used in setStringRepresentation(), writeValue(), getPropertyValue()
 * 
 */
template<PropertyType T> struct PropertyTraitSimple; 
template<> struct PropertyTraitSimple<pNull>
{	public: typedef NullType		value;
	public: typedef NullType		writeType; 
};
template<> struct PropertyTraitSimple<pBool>
{	public: typedef bool			value;
	public: typedef bool			writeType; 
};
template<> struct PropertyTraitSimple<pInt>	
{	public: typedef int				value;
	public: typedef int				writeType; 
};
template<> struct PropertyTraitSimple<pReal>	
{	public: typedef double			value;
	public: typedef double 			writeType; 
};
template<> struct PropertyTraitSimple<pString> 
{	public: typedef std::string			value;
	public: typedef const std::string& 	writeType; 
};
template<> struct PropertyTraitSimple<pName>	
{	public: typedef std::string			value;
	public: typedef const std::string& 	writeType; 
};
template<> struct PropertyTraitSimple<pRef> 	
{	public: typedef IndiRef				value;
	public: typedef const IndiRef&	 	writeType; 
};


//=====================================================================================
// CObjectSimple
//


/** 
 * Template class representing simple PDF objects from specification v1.5.
 *
 * This class represents simple objects like null, string, number etc.
 * It does not have special functions like CObjectComplex.
 */
template <PropertyType Tp>
class CObjectSimple : public IProperty
{
public:
	/** Write type for writeValue function. */
	typedef typename PropertyTraitSimple<Tp>::writeType	 WriteType;
	/** Value holder. */
	typedef typename PropertyTraitSimple<Tp>::value 	 Value;  
			
private:
	/** Object's value. */
	Value value;
	
private:
	/**
	 * Copy constructor
	 */
	CObjectSimple (const CObjectSimple&);
	
public/*protected*/:
	/**
	 * Constructor. Only kernel can call this constructor. It depends on the object, that we have
	 * parsed.
	 *
	 * @param p		Pointer to pdf object.
	 * @param o		Xpdf object. 
	 * @param ref	Indirect id and gen id.
	 * @param isDirect	Indicates whether this is a direct/indirect object.
	 */
	CObjectSimple (CPdf& p, Object& o, const IndiRef& ref, bool isDirect);


public:	

	/**
	 * Public constructor. Can be used to creted direct/indirect objects.
	 *
	 * @param p		Pointer to pdf object in which this object will exist.
	 * @param isDirect	Indicates whether this is a direct/indirect object.
	 */
	CObjectSimple (CPdf& p, bool isDirect);

#ifdef DEBUG
CObjectSimple () : value(Value()) {};
#endif
	
	
	/**
	 * Returns string representation of (x)pdf object. 
	 * 
	 * @param str 	After successful call, it will hold the string representation 
	 * 				of current object.
	 */
	void getStringRepresentation (std::string& str) const;
	
	
	/**
	 * Make object from string.
	 * <exception cref="..." />
	 *
	 * @param str0 Object in a text form.
	 */
	void setStringRepresentation (const std::string& strO);

	
	/**
	 * Change the value of an object. The variable type depends
	 * on CObject type. For complex types, it is equal to setStringRepresentation().
	 * 
	 * We can define the best to represent an pdf object.
	 *
 	 * <a cref="ObjBadValueE" /> Thrown when the string, can't be parsed correctly.
	 *
	 * @param val	Value that will be set.
	 */
	void writeValue (WriteType val);
 

	/**
	 * Return property value. Each property has its own return type.
	 *
	 * @param val Out parameter where property value will be stored.
	 */
	void getPropertyValue (Value& val) const;
	
	/**
	 * Indicate that you do not want to use this object again.
	 * 
	 * If it is an indirect object, we have to notify CXref.
	 */
  	virtual void release ();
	

//protected: 
/*DEBUG*/public:	
	/**
	 * Destructor
	 */
	~CObjectSimple () {};
	
protected:
	/**
 	 * Notify Writer object that this object has changed. We have to call this
	 * function to make changes visible.
	 *
	 * It is necessary for undo operation, that means saving previous state of the object.
	 * We obtain reference to CXref from CObject::pdf.
	 */
	void dispatchChange () const; 


	//
	// Helper functions
	//
protected:
	/**
	 * Make xpdf Object from this object. This function allocates xpdf object, caller has to free it.
	 *
	 * @return Xpdf object representing actual value of this simple object.
	 */
	Object*	_makeXpdfObject () const;
	
private:
	/**
	 * Indicate that the object has changed.
	 */
	inline void
	_objectChanged ()
	{
		// Dispatch the change
		dispatchChange ();
		
		// Notify everybody about this change
		IProperty::notifyObservers ();
	}

};




//=====================================================================================
// CObjectComplex
//


//
// Helper objects, functors, ...
//
class ArrayIdxComparator
{
private:
	unsigned int pos;
	IProperty* ip;
public:
		ArrayIdxComparator (unsigned int p) : pos(p),ip(NULL) {};
		
		inline IProperty* getIProperty () {return ip;};
		
		bool operator() (IProperty* _ip)
		{	
			if (0 == pos)
			{
				ip = _ip;
				return true;
			}
			pos--;
			return false;
		}
};

class DictIdxComparator
{
private:
	std::string str;
	IProperty* ip;
public:
		DictIdxComparator (const std::string& s) : str(s),ip(NULL) {};
		
		inline IProperty* getIProperty () {return ip;};
		
		bool operator() (std::pair<std::string,IProperty*> item)
		{	
			if (item.first == str)
			{
				ip = item.second;
				return true;
			}
			
			return false;
		};
};

/**
 * Additional information that identifies variable type, e.g. for writeValue function.
 *
 * If someone tries to use unsupported type (pCmd,....), she should get compile error
 * because PropertyTrait<> has no body.
 */
template<PropertyType T> struct PropertyTraitComplex; 
template<> struct PropertyTraitComplex<pArray>	
{	public: 
		typedef std::vector<IProperty*>	value; 
		typedef const std::string& 	writeType; 
		typedef unsigned int	 	propertyId;
		typedef class ArrayIdxComparator	indexComparator;
};
template<> struct PropertyTraitComplex<pStream> 
{	public: 
		typedef std::list<std::pair<std::string,IProperty*> >	value; 
		typedef const std::string& 	writeType; 
		typedef const std::string& 	propertyId;
		typedef class DictIdxComparator	indexComparator;
};
template<> struct PropertyTraitComplex<pDict>	
{	public: 
		typedef std::list<std::pair<std::string,IProperty*> >	value; 
		typedef const std::string& 	writeType; 
		typedef const std::string& 	propertyId;
		typedef class DictIdxComparator	indexComparator;
};


/** 
 * Template class representing complex PDF objects from specification v1.5.
 *
 * The specific features are implemented using c++ feature called Incomplete Instantiation.
 * It means that, when it is not used, it is not instatiated, so e.g. CArray won't have
 * addProperty (IProperty& newIp, const std::string& propertyName) method.
 *
 * This class can be both, a final class (no child objects) or a parent to a special class.
 *
 * When it is not a final class, it is a special object (CPdf, CPage,...). We can
 * find this out by calling virtual method getSpecialObjType(). 
 * This can be helpful for example for special manipulation with content stream, xobjects, ...
 */
template <PropertyType Tp>
class CObjectComplex : public IProperty
{
public:
	/** Index identifying position in Array.*/
	typedef unsigned int	PropertyIndex;
	/** String identifying position in Dict/Stream.*/
	typedef std::string		PropertyName;
	/** Write type for writeValue function. */
	typedef typename PropertyTraitComplex<Tp>::writeType  		WriteType;
	/** This type identifies a property. */
	typedef typename PropertyTraitComplex<Tp>::propertyId 		PropertyId;
	/** This functor can find an item in the value holder. */
	typedef typename PropertyTraitComplex<Tp>::indexComparator	IndexComparator;
			
	/** Value holder. */
	typedef typename PropertyTraitComplex<Tp>::value 	  Value;  

private:
	/** Object's value. */
	Value value;

private:
	/**
	 * Copy constructor
	 */
	CObjectComplex (const CObjectComplex&) {};
	

protected:
	/**
	 * Pdf constructor.
	 */
	CObjectComplex ();

/*debug*/public:
	/**
	 * Constructor. Only kernel can call this constructor
	 *
	 * @param p		Pointer to pdf object.
	 * @param o		Xpdf object. 
	 * @param ref	Indirect id and gen id.
	 * @param isDirect	Indicates whether this is a direct/indirect object.
	 */
	CObjectComplex (CPdf& p, Object& o, const IndiRef& ref, bool isDirect);


public:	

	/**
	 * Public constructor. Can be used to create direct/indirect objects.
	 *
	 * @param p	Pointer to pdf object in which this object will exist.
	 * @param isDirect	Indicates whether this object is direct or not.
	 */
	CObjectComplex (CPdf& p, bool isDirect);

#ifdef DEBUG
CObjectComplex (int /*i*/) : value(Value()){};
#endif

	
	/**
	 * Returns string representation of (x)pdf object. 
	 * 
	 * @param str 	After successful call, it will hold string representation 
	 * 				of current object.
	 */
	void getStringRepresentation (std::string& str) const;
	
	
	/**
	 * Try to make an (x)pdf object from string.
	 * <exception cref="..." />
	 *
	 * @param str0 Object in a text form.
	 */
	void setStringRepresentation (const std::string& strO);

	
	/**
	 * Change the value of an object. The variable type depends
	 * on CObject type. 
	 * For complex types, it is equal to setStringRepresentation().
	 * 
	 * <exception cref="ObjBadValueE "/> Thrown When a value cannot be set due to bad value e.g. in complex types.
	 *
	 * @param val	Value that will be set.
	 */
	void writeValue (WriteType val);
  
	
	/**
	 * Returns if it is one of special objects CPdf,CPage etc.
	 *
	 * @return Type of special object.
	 */
	virtual SpecialObjectType getSpecialObjType() const {return sNone;};

	
	/**
	 * Returns pointer to special object.
	 *
	 * @return Pointer to special child object specified by template argument. 
	 */
	template<typename T>
	T* getSpecialObjectPtr () const
	{
		STATIC_CHECK(sizeof(T)>=sizeof(CObjectComplex<Tp>),DESTINATION_TYPE_TOO_NARROW); 
		return dynamic_cast<T*>(this);
	}

	
	/**
	 * Indicate that you do not want to use this object again.
	 * If it is an indirect object, we have to notify CXref.
	 */
  	virtual void release ();
	

//protected:
/*DEBUG*/public:
	/**
	 * Destructor
	 */
	~CObjectComplex ()	{};

protected:
	/**
 	 * Notify Writer object that this object has changed. We have to call this
	 * function to make changes visible.
	 *
	 * It is necessary for undo operation, that means saving previous state of the object.
	 * We obtain reference to CXref from CObject::pdf.
	 */
	void dispatchChange () const; 


	//
	//
	// Specific features by Incomplete Instantiation
	//
	//
public:
	/** 
	 * Returns property count.
	 * 
	 * @return Property count.
	 */
	inline size_t getPropertyCount () const {return value.size();};
 

	/**
	 * Inserts all property names to container supplied by caller. 
	 * 
	 * REMARK: Specific for pDict and pStream.
     	 *
	 * @param container Container of string objects. STL vector,list,deque.
	 */
	virtual void getAllPropertyNames (std::list<std::string>& container) const;


	/**
	 * Returns value of property identified by its name/position depending on type of this object.
   	 *
   	 * @param 	id 	Variable identifying position of the property.
	 * @return	Variable where the value will be stored.
   	 */
   	IProperty* getPropertyValue (PropertyId id) const;

	
	/**
	 * Returns property type of an item identified by name/position.
	 *
	 * <exception cref="ObjInvalidPositionInComplex "/> When the id does not correctly identify an item.
	 *
	 * @param	name	Name of the property.
	 * @return		Property type.	
	 */
	PropertyType getPropertyType (PropertyId id) const 
		{return getPropertyValue(id)->getType();};

	
	/**
	 * Sets property type of an item.
	 *
	 * @param	id		Name/Index of property
	 * @param	value	Value, for simple types (int,string,...) and for complex types IProperty*
	 */
	virtual IProperty* setPropertyValue (PropertyId id, IProperty& ip);
	
	/**
	 * Adds property to array/dict/stream. The property that should be added will
	 * automaticly be associated with the pdf this object lives in.
	 * Finally indicate that this object has changed.
	 *
	 * @param newIp 	New property.
	 * @param propertyName 	Name of the created property
	 */
	void addProperty (IProperty& newIp);
	void addProperty (const std::string& propertyName, IProperty& newIp);
	
	
	/**
	 * Remove property from array/dict/stream. If the xpdf Object to be removed is 
	 * associated with an IProperty call release(). Otherwise just free the memory
	 * occupied by the xpdf object.
	 * Finally indicate that this object has changed.
	 *
	 * <exception cref="ObjInvalidPositionInComplex "/> When the id does not correctly identify an item.
	 * 
	 * @param id Name/Index of property
	 */
	void delProperty (PropertyId /*id*/);



	//
	// Helper functions
	//
protected:
	/**
	 * Make xpdf Object from this object.
	 *
	 * @return Xpdf object representing actual value of this simple object.
	 */
	Object*	_makeXpdfObject () const;

private:
	/**
	 * Make everything needed to indicate that this object has changed.
	 * Notifies all obervers associated with this property.
	 */
	inline void
	_objectChanged ()
	{
		// Dispatch the change
		dispatchChange ();
		// Notify everybody about this change
		IProperty::notifyObservers ();
	}
};



//=====================================================================================
//
// CObject types
//

typedef CObjectSimple<pNull>	CNull;
typedef CObjectSimple<pBool>	CBool;
typedef CObjectSimple<pInt>		CInt;
typedef CObjectSimple<pReal>	CReal;
typedef CObjectSimple<pString> 	CString;
typedef CObjectSimple<pName> 	CName;
typedef CObjectSimple<pRef> 	CRef;

typedef CObjectComplex<pStream> CStream;
typedef CObjectComplex<pArray>	CArray;
typedef CObjectComplex<pDict>	CDict;





//=====================================================================================
//
//  Helper functions located in cobject.cc
//
namespace utils {


		
/**
 * Returns simple xpdf object (null,number,string...) in string representation.
 * 
 * REMARK: String can represent more different objects, so we have to distinguish among them.
 * This is done at compile time with use of templates, but because of this we have to
 * make other functions also template.
 *
 * @param Value that will be converted to string.
 * @param Output string
 */
template <PropertyType Tp> void simpleValueToString (bool val,std::string& str);
template <PropertyType Tp> void simpleValueToString (int val,std::string& str);
template <PropertyType Tp> void simpleValueToString (double val,std::string& str);
template <PropertyType Tp> void simpleValueToString (const std::string& val,std::string& str);
template <PropertyType Tp> void simpleValueToString (const NullType& val,std::string& str);
template <PropertyType Tp> void simpleValueToString (const IndiRef& val,std::string& str);

/**
 * Returns complex xpdf object (null,number,string...) in string representation.
 *
 * @param Value that will be converted to string.
 * @param Output string
 */
template <PropertyType Tp> void complexValueToString (const std::vector<IProperty*>& val,std::string& str);
template <PropertyType Tp> void complexValueToString (const std::list<std::pair<std::string,IProperty*> >& val,std::string& str);

/**
 * Saves real xpdf object value to val.
 * 
 * @param obj	Xpdf object which holds the value.
 * @param val	Variable where the value will be stored.
 */
template <PropertyType Tp,typename T> void simpleValueFromXpdfObj (Object& obj, T val);
template <PropertyType Tp,typename T> void complexValueFromXpdfObj (IProperty& ip, Object& obj, T val);

/**
 * Creates xpdf Object which represents value.
 * 
 * @param obj	Value where the value is stored.
 * @return 		Xpdf object where the value is stored.
 */
template <PropertyType Tp,typename T> Object* simpleValueToXpdfObj (T val);

/**
 * Template functions can't be virutal, so this is a helper
 * function that has the same functionality as getAllPropertyNames() but
 * can take as parameter any container type that supports push_back function.
 *
 * @param container Container to which all names will be added.
 */
template<typename T>
inline void 
getAllNames (T& container, const std::list<std::pair<std::string,IProperty*> >& store)
{
	for (std::list<std::pair<std::string,IProperty*> >::const_iterator it = store.begin();
		it != store.end(); it++)
	{
			container.push_back ((*it).first);
	}
}


/**
 * Parses string to get simple values like int, name, bool etc.
 * 
 * <a cref="ObjBadValueE" /> Thrown when the string, can't be parsed correctly.
 * 
 * @param strO	String to be parsed.
 * @param val	Desired value.
 */
void simpleValueFromString (const std::string& str, bool& val);
void simpleValueFromString (const std::string& str, int& val);
void simpleValueFromString (const std::string& str, double& val);
void simpleValueFromString (const std::string& str, std::string& val);
void simpleValueFromString (const std::string& str, IndiRef& val);


/**
 * Free an object. We assume that all child objects (if any)
 * have been already freed.
 *
 * Copy & paste from Object.h
 * 
 * @param obj	Object to be freed.
 */
void freeXpdfObject (Object* obj);

/**
 * Create xpdf object from string.
 *
 * @param str String that should represent an xpdf object.
 */
Object* xpdfObjFromString (const std::string& str);


} /* namespace utils */
} /* namespace pdfobjects */




//
// Include the actual implementation of CObject class
//
#include "cobjectI.h"


#endif // COBJECT_H

