/*
    Copyright (C) 1998 by Jorrit Tyberghein

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __CS_IUTIL_PLUGINCONFIG_H__
#define __CS_IUTIL_PLUGINCONFIG_H__

/**\file
 * Plugin configuration interface and variant types
 */
/**\addtogroup util
 * @{ */
#include "csgeom/matrix3.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csgeom/vector4.h"
#include "csutil/cscolor.h"
#include "csutil/scf.h"
#include "csutil/scfstr.h"
#include "csutil/array.h"

/// Type of the values that can be contained within a csVariant.
enum csVariantType
{
  // Signed 32-bit integer (long)
  CSVAR_LONG,
  /// Boolean type
  CSVAR_BOOL,
  /// A command. A command has no value, it is just a flag which can be set or not.
  //CSVAR_CMD,
  /// Float type
  CSVAR_FLOAT,
  /// Double-precision floating point number
  //CSVAR_DOUBLE,
  /// String type
  CSVAR_STRING,
  /// csColor type
  CSVAR_COLOR,
  /// csColor4 type
  CSVAR_COLOR4,
  /// csVector2 type
  CSVAR_VECTOR2,
  /// csVector3 type
  CSVAR_VECTOR3,
  /// csVector4 type
  CSVAR_VECTOR4,
  /// A key-value pair
  //CSVAR_KEYVAL, // put on hold
  /// A list of key-value pairs, such as a hashmap
  //CSVAR_KEYVALLIST, // put on hold
  /// A VFS path
  //CSVAR_VFSPATH,
  /// csMatrix3 type
  CSVAR_MATRIX3,
  /// csTransform type
  CSVAR_TRANSFORM,
  /// An iBase entity
  CSVAR_IBASE,
  /// An array type
  CSVAR_ARRAY
};


/**
 * Variant, ie a value whose type is set at runtime.
 * \warning Requesting another type than the contained one
 * will trigger an assertion
 */
struct csVariant
{
private:
  csVariantType type;
  union value
  {
    long l;
    bool b;
    // TODO: this could be more optimized...
    float f[4];
    iString* s;
    csMatrix3* m;
    csTransform* t;
    iBase* ib;
    csArray<csVariant>* a;
  } val;

  void Clear()
  {
    if ((type == CSVAR_STRING)
	&& (val.s != 0)) val.s->DecRef();
    else if ((type == CSVAR_MATRIX3) && (val.m != 0))
      delete val.m;
    else if ((type == CSVAR_TRANSFORM) && (val.t != 0))
      delete val.t;
    else if ((type == CSVAR_IBASE) && (val.ib != 0))
      val.ib->DecRef();
    else if ((type == CSVAR_ARRAY) && (val.a != 0))
      delete val.a;
  }

public:
  /** Blank constructor; Set the variant to CSVAR_LONG and 0; 
   *  Usually not used. Where it is needed, the type and value
   *  should generally be overwritten anyway.
   */
  csVariant () { type = CSVAR_LONG; val.l = 0; }
  /// Constructor initialized with a value of type CSVAR_LONG
  csVariant (int i) { type = CSVAR_LONG; val.l = i; }
  /// Constructor initialized with a value of type CSVAR_LONG
  csVariant (long l) { type = CSVAR_LONG; val.l = l; }
  /// Constructor initialized with a value of type CSVAR_BOOL
  csVariant (bool b) { type = CSVAR_BOOL; val.b = b; }
  /// Constructor initialized with a value of type CSVAR_FLOAT
  csVariant (float f) { type = CSVAR_FLOAT; val.f[0] = f; }
  /// Constructor initialized with a value of type CSVAR_STRING (from a char*)
  csVariant (const char* s) { type = CSVAR_STRING; val.s = s ? new scfString (s) : nullptr; }
  /// Constructor initialized with a value of type CSVAR_STRING (from an scfString)
  csVariant (scfString& s) { type = CSVAR_STRING; val.s = s ? new scfString(s) : nullptr; }
  /// Constructor initialized with a value of type CSVAR_COLOR
  csVariant (const csColor& c) { type = CSVAR_COLOR; val.f[0] = c[0]; val.f[1] = c[1]; val.f[2] = c[2]; }
  /// Constructor initialized with a value of type CSVAR_COLOR4
  csVariant (const csColor4& c) { type = CSVAR_COLOR4; val.f[0] = c[0]; val.f[1] = c[1]; val.f[2] = c[2]; val.f[3] = c[3]; }
  /// Constructor initialized with a value of type CSVAR_VECTOR2
  csVariant (const csVector2& v) { type = CSVAR_VECTOR2; val.f[0] = v[0]; val.f[1] = v[1]; }
  /// Constructor initialized with a value of type CSVAR_VECTOR3
  csVariant (const csVector3& v) { type = CSVAR_VECTOR3; val.f[0] = v[0]; val.f[1] = v[1]; val.f[2] = v[2]; }
  /// Constructor initialized with a value of type CSVAR_VECTOR4
  csVariant (const csVector4& v) { type = CSVAR_VECTOR4; val.f[0] = v[0]; val.f[1] = v[1]; val.f[2] = v[2]; val.f[3] = v[3]; }
  /// Constructor initialized with a value of type CSVAR_MATRIX3
  csVariant (const csMatrix3& m) { type = CSVAR_MATRIX3; val.m = new csMatrix3 (m); }
  /// Constructor initialized with a value of type CSVAR_TRANSFORM
  csVariant (const csTransform& t) { type = CSVAR_TRANSFORM; val.t = new csTransform (t); }
  /// Constructor initialized with a csArray
  csVariant (const csArray<csVariant>& a) { type = CSVAR_ARRAY; val.a = new csArray<csVariant>(a); }
  /// Constructor initialized with an iBase
  // TODO

  /// Copy constructor.
  csVariant (const csVariant& var)
  {
    memset (&val, 0, sizeof (val));
    
    type = var.type;
    val = var.val;
    if ((type == CSVAR_STRING)
	&& (val.s != 0)) val.s->IncRef(); 

    if ((type == CSVAR_IBASE)
      && (val.ib != 0)) val.ib->IncRef();
  }

  ~csVariant () { Clear(); }

  /// Assignment operator.
  const csVariant& operator = (const csVariant& var)
  {
    Clear ();
    type = var.type;
    val = var.val;
    if ((type == CSVAR_STRING)
	&& (val.s != 0)) val.s->IncRef ();

    if ((type == CSVAR_IBASE)
      && (val.ib != 0)) val.ib->IncRef();

    return var;
  }
  
  /// Assign a long
  void SetLong (long l)
  {
    Clear();
    type = CSVAR_LONG;
    val.l = l;
  }
  /// Assign a bool
  void SetBool (bool b)
  {
    Clear();
    type = CSVAR_BOOL;
    val.b = b;
  }
  /// Assign a float
  void SetFloat (float f)
  {
    Clear();
    type = CSVAR_FLOAT;
    val.f[0] = f;
  }
  /// Assign a string
  void SetString (const char* s)
  {
    Clear();
    type = CSVAR_STRING;
    if (s)
      val.s = new scfString (s);
    else
      val.s = 0;
  }
  /// Assign a csColor
  void SetColor (const csColor& c)
  {
    Clear();
    type = CSVAR_COLOR;
    val.f[0] = c[0];
    val.f[1] = c[1];
    val.f[2] = c[2];
  }
  /// Assign a csColor4
  void SetColor4 (const csColor4& c)
  {
    Clear();
    type = CSVAR_COLOR4;
    val.f[0] = c[0];
    val.f[1] = c[1];
    val.f[2] = c[2];
    val.f[3] = c[3];
  }
  /// Assign a csVector2
  void SetVector2 (const csVector2& v)
  {
    Clear();
    type = CSVAR_VECTOR2;
    val.f[0] = v[0];
    val.f[1] = v[1];
  }
  /// Assign a csVector3
  void SetVector3 (const csVector3& v)
  {
    Clear();
    type = CSVAR_VECTOR3;
    val.f[0] = v[0];
    val.f[1] = v[1];
    val.f[2] = v[2];
  }
  /// Assign a csVector4
  void SetVector4 (const csVector4& v)
  {
    Clear();
    type = CSVAR_VECTOR4;
    val.f[0] = v[0];
    val.f[1] = v[1];
    val.f[2] = v[2];
    val.f[3] = v[3];
  }
  /// Assign a csMatrix3
  void SetMatrix3 (const csMatrix3& m)
  {
    Clear();
    type = CSVAR_MATRIX3;
    val.m = new csMatrix3 (m);
  }
  /// Assign a csTransform
  void SetTransform (const csTransform& t)
  {
    Clear();
    type = CSVAR_TRANSFORM;
    val.t = new csTransform (t);
  }
  /// Assign an iBase
  // TODO
  /// Assign a csArray
  void SetArray ( const csArray<csVariant>& a)
  {
    Clear();
    type = CSVAR_ARRAY;
    val.a = new csArray<csVariant>(a);
  }

  /// Retrieve a long
  long GetLong () const
  {
    CS_ASSERT (type == CSVAR_LONG);
    return val.l;
  }
  /// Retrieve a bool
  bool GetBool () const
  {
    CS_ASSERT (type == CSVAR_BOOL);
    return val.b;
  }
  /// Retrieve a float
  float GetFloat () const
  {
    CS_ASSERT (type == CSVAR_FLOAT);
    return val.f[0];
  }
  /// Retrieve a string
  const char* GetString () const
  {
    CS_ASSERT (type == CSVAR_STRING);
    return val.s->GetData();
  }
  /// Retrieve a csColor
  csColor GetColor () const
  {
    CS_ASSERT (type == CSVAR_COLOR);
    return csColor (val.f[0], val.f[1], val.f[2]);
  }
  /// Retrieve a csColor4
  csColor4 GetColor4 () const
  {
    CS_ASSERT (type == CSVAR_COLOR4);
    return csColor4 (val.f[0], val.f[1], val.f[2], val.f[3]);
  }
  /// Retrieve a csVector2
  csVector2 GetVector2 () const
  {
    CS_ASSERT (type == CSVAR_VECTOR2);
    return csVector2 (val.f[0], val.f[1]);
  }
  /// Retrieve a csVector3
  csVector3 GetVector3 () const
  {
    CS_ASSERT (type == CSVAR_VECTOR3);
    return csVector3 (val.f[0], val.f[1], val.f[2]);
  }
  /// Retrieve a csVector4
  csVector4 GetVector4 () const
  {
    CS_ASSERT (type == CSVAR_VECTOR4);
    return csVector4 (val.f[0], val.f[1], val.f[2], val.f[3]);
  }
  /// Retrieve a csMatrix3
  csMatrix3 GetMatrix3 () const
  {
    CS_ASSERT (type == CSVAR_MATRIX3);
    return *val.m;
  }
  /// Retrieve a csTransform
  csTransform GetTransform () const
  {
    CS_ASSERT (type == CSVAR_TRANSFORM);
    return *val.t;
  }
  /// Retrieve an iBase
  // TODO
  /// Retrieve a csArray
  csArray<csVariant> GetArray() const 
  {
    CS_ASSERT (type == CSVAR_ARRAY);
    return *val.a;
  }

  /// Get the type of the contained value. The default value is CSVAR_CMD.
  csVariantType GetType () const { return type; }
};

/// Description of a configuration option, to be used by the iPluginConfig interfaces
struct csOptionDescription
{
  /// Description index (or ID)
  int id;
  /// Short name of this option.
  csString name;
  /// Description for this option.
  csString description;
  /// Type to use for this option.
  csVariantType type;

  /// Constructor
  csOptionDescription () {}

  /**
   * Constructor
   * \param id Description index (or ID)
   * \param name Short name of this option.
   * \param description Description for this option.
   * \param type Type to use for this option.
   */
  csOptionDescription (int id, const char* name, const char* description, csVariantType type)
  : id (id), name (name), description (description), type (type) {}

  /**
   * Constructor
   * \param name Short name of this option.
   * \param description Description for this option.
   * \param type Type to use for this option.
   * \warning The \a id is initialized to 0 in this constructor!
   */
  csOptionDescription (const char* name, const char* description, csVariantType type)
  : id (0), name (name), description (description), type (type) {}

  ~csOptionDescription () {}
};

/**
 * Interface to a configurator object. If a SCF module
 * has an object implementing this interface then this can
 * be used to query or set configuration options.
 *
 * Main creators of instances implementing this interface:
 * - Some plugins implement this.
 * 
 * Main ways to get pointers to this interface:
 * - scfQueryInterface() from a plugin instance.
 * 
 * Main users of this interface:
 * - csCommandLineHelper
 * - csPluginManager
 */
struct iPluginConfig : public virtual iBase
{
  SCF_INTERFACE (iPluginConfig,2,1,0);

  /**
   * Get the description of the option of index \a idx. Return \a false if this
   * option does not exist, true otherwise.
   * \param index The index of the option
   * \param option The returned description of the option
   */
  virtual bool GetOptionDescription (int index, csOptionDescription* option) = 0;

  /**
   * Set the value of the option of index \a idx. Return \a false if this
   * option does not exist, true otherwise.
   * \param index The index of the option
   * \param value The new value to be set for the option
   */
  virtual bool SetOption (int index, csVariant* value) = 0;

  /**
   * Get the value of the option of index \a idx. Return \a false if this
   * option does not exist, true otherwise.
   * \param index The index of the option
   * \param value A variant where to store the value of the option
   */
  virtual bool GetOption (int index, csVariant* value) = 0;
};
/** @} */

#endif // __CS_IUTIL_PLUGINCONFIG_H__
