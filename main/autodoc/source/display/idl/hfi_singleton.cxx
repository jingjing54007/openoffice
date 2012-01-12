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
#include "hfi_singleton.hxx"


// NOT FULLY DEFINED SERVICES
#include <ary/idl/i_ce.hxx>
#include <ary/idl/ik_singleton.hxx>
#include <ary/idl/ik_sisingleton.hxx>
#include <toolkit/hf_docentry.hxx>
#include <toolkit/hf_linachain.hxx>
#include <toolkit/hf_title.hxx>
#include "hfi_navibar.hxx"
#include "hfi_typetext.hxx"
#include "hi_linkhelper.hxx"



extern const String
    C_sCePrefix_Singleton("singleton");

const String
    C_sAssociatedService("Associated Service");
const String
    C_sImplementedInterface("Supported Interface");



HF_IdlSingleton::HF_IdlSingleton( Environment &         io_rEnv,
                                  Xml::Element &        o_rOut )
    :   HtmlFactory_Idl(io_rEnv, &o_rOut)
{
}

HF_IdlSingleton::~HF_IdlSingleton()
{

}

typedef ::ary::idl::ifc_singleton::attr         SingletonAttr;
typedef ::ary::idl::ifc_sglifcsingleton::attr   SglIfcSingletonAttr;

void
HF_IdlSingleton::Produce_byData_ServiceBased( const client & i_ce ) const
{
    make_Navibar(i_ce);

    HF_TitleTable
        aTitle(CurOut());

    HF_LinkedNameChain
        aNameChain(aTitle.Add_Row());

    aNameChain.Produce_CompleteChain(Env().CurPosition(), nameChainLinker);
    produce_Title(aTitle, C_sCePrefix_Singleton, i_ce);

    HF_DocEntryList
        aTopList( aTitle.Add_Row() );
    aTopList.Produce_Term(C_sAssociatedService);

    HF_IdlTypeText
        aAssociatedService( Env(), aTopList.Produce_Definition(), true );
    aAssociatedService.Produce_byData( SingletonAttr::AssociatedService(i_ce) );

    CurOut() << new Html::HorizontalLine;

    write_Docu(aTitle.Add_Row(), i_ce);
    CurOut() << new Html::HorizontalLine();
}

void
HF_IdlSingleton::Produce_byData_InterfaceBased( const client & i_ce ) const
{
    make_Navibar(i_ce);

    HF_TitleTable
        aTitle(CurOut());

    HF_LinkedNameChain
        aNameChain(aTitle.Add_Row());

    aNameChain.Produce_CompleteChain(Env().CurPosition(), nameChainLinker);
    produce_Title(aTitle, C_sCePrefix_Singleton, i_ce);

    HF_DocEntryList
        aTopList( aTitle.Add_Row() );
    aTopList.Produce_Term(C_sImplementedInterface);

    HF_IdlTypeText
        aImplementedInterface( Env(), aTopList.Produce_Definition(), true );
    aImplementedInterface.Produce_byData( SglIfcSingletonAttr::BaseInterface(i_ce) );

    CurOut() << new Html::HorizontalLine;

    write_Docu(aTitle.Add_Row(), i_ce);
    CurOut() << new Html::HorizontalLine();
}

void
HF_IdlSingleton::make_Navibar( const client & i_ce ) const
{
    HF_IdlNavigationBar
        aNaviBar(Env(), CurOut());
    aNaviBar.Produce_CeMainRow(i_ce,true);  // true := avoid link to Use-page.

    CurOut() << new Html::HorizontalLine();
}
