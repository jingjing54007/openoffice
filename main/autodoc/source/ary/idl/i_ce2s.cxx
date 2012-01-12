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
#include <ary/idl/i_ce.hxx>


// NOT FULLY DEFINED SERVICES
#include <cosv/tpl/tpltools.hxx>
#include <ary/doc/d_oldidldocu.hxx>
#include <ary/getncast.hxx>


namespace ary
{
namespace idl
{

namespace
{
const std::vector<Ce_id> C_sNullVector_Ce_ids;
}


Ce_2s::~Ce_2s()
{
    csv::erase_container_of_heap_ptrs(aXrefLists);
}

DYN Ce_2s *
Ce_2s::Create_( ClassId )
{
    return new Ce_2s;
}


std::vector<Ce_id> &
Ce_2s::Access_List( int i_indexOfList )
{
    csv_assert(i_indexOfList >= 0 AND i_indexOfList < 1000);

    while (i_indexOfList >= (int) aXrefLists.size())
    {
        aXrefLists.push_back(new std::vector<Ce_id>);
    }
    return *aXrefLists[i_indexOfList];
}

const std::vector<Ce_id> &
Ce_2s::List( int i_indexOfList ) const
{
    if (uintt(i_indexOfList) < aXrefLists.size())
        return *aXrefLists[i_indexOfList];
    else
        return C_sNullVector_Ce_ids;
}


}   // namespace idl
}   // namespace ary
