/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#include <precomp.h>
#include <ary/idl/i_singleton.hxx>


// NOT FULLY DECLARED SERVICES
#include <cosv/tpl/processor.hxx>
#include <ary/idl/ik_singleton.hxx>
#include <sci_impl.hxx>


namespace ary
{
namespace idl
{

Singleton::Singleton( const String &      i_sName,
                  Ce_id               i_nOwner )
    :   sName(i_sName),
        nOwner(i_nOwner),
        nService()
{
}

Singleton::~Singleton()
{
}

void
Singleton::do_Accept( csv::ProcessorIfc & io_processor ) const
{
    csv::CheckedCall(io_processor, *this);
}

ClassId
Singleton::get_AryClass() const
{
    return class_id;
}

const String &
Singleton::inq_LocalName() const
{
    return sName;
}

Ce_id
Singleton::inq_NameRoom() const
{
    return nOwner;
}

Ce_id
Singleton::inq_Owner() const
{
    return nOwner;
}

E_SightLevel
Singleton::inq_SightLevel() const
{
    return sl_File;
}


namespace ifc_singleton
{

inline const Singleton &
singleton_cast( const CodeEntity &  i_ce )
{
    csv_assert( i_ce.AryClass() == Singleton::class_id );
	return static_cast< const Singleton& >(i_ce);
}

Type_id
attr::AssociatedService( const CodeEntity & i_ce )
{
    return singleton_cast(i_ce).nService;
}

} // namespace ifc_singleton


}   //  namespace   idl
}   //  namespace   ary
