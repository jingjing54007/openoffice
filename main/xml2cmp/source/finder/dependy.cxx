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




#include "dependy.hxx"
#include <iostream>
#include "../support/syshelp.hxx"
#include "../support/list.hxx"
#include "../xcd/xmltree.hxx"
#include "../xcd/parse.hxx"



Simstr 				ShortName(const Simstr & i_rService);



Service::Service( const char * i_sName )
	:	sName(i_sName)
		// aImplementations
{
}

ServiceInfo &
Service::AddImplementation( const char * i_sLibrary )
{
	ServiceInfo * ret = new ServiceInfo(i_sLibrary);
	aImplementations.push_back(ret);
	return *ret;
}

ServiceInfo::ServiceInfo( const char * i_sLibrary )
	:	sImplementingLibrary(i_sLibrary)
		// aNeededServices
{
}

void
ServiceInfo::AddDependency( const char * i_sNeededService )
{
	aNeededServices.push_back(i_sNeededService);
}

DependencyFinder::DependencyFinder()
{
}

DependencyFinder::~DependencyFinder()
{
}

void
DependencyFinder::GatherData( const char * i_sSearchDirectory )
{
	List<Simstr> aFiles;
	GatherFileNames( aFiles, i_sSearchDirectory );

	for ( unsigned i = 0; i < aFiles.size(); ++i )
	{
		ReadFile( aFiles[i].str() );
	}
}

void
DependencyFinder::FindNeededServices( StringVector &		o_rLibraries,
									  StringVector &		o_rServices,
									  const Simstr &		i_rService )
{
	Map_Services::const_iterator itService = aServices.find(i_rService);
	if ( itService == aServices.end() )
	{
		std::cerr << "Error: Service \""
				  << i_rService.str()
				  << "\" not found."
				  << std::endl;
		return ;
	}

	aResult_Libraries.erase( aResult_Libraries.begin(), aResult_Libraries.end() );
	aResult_Services.erase( aResult_Services.begin(), aResult_Services.end() );

//	const ServiceInfo & rSInfo = (*itService).second->FirstImplementation();
	Add2Result( *(*itService).second );

	for ( std::set< Simstr >::const_iterator il = aResult_Libraries.begin();
		  il != aResult_Libraries.end();
		  ++il )
	{
		o_rLibraries.push_back(*il);
	}

	for ( std::set< Simstr >::const_iterator is = aResult_Services.begin();
		  is != aResult_Services.end();
		  ++is )
	{
		o_rServices.push_back(*is);
	}
}

void
DependencyFinder::ReadFile(  const char * i_sFilename )
{
	ModuleDescription	aModule;
	X2CParser			aParser(aModule);

	if ( !aParser.Parse(i_sFilename) )
	{
		std::cerr << "Error: File \""
			 << i_sFilename
			 << "\" could not be parsed."
			 << std::endl;
		return;
	}

	// GetResults:
	Simstr sModule = aModule.ModuleName();

	List < const MultipleTextElement* > aImplServices;
	List < const MultipleTextElement* > aNeededServices;

	aModule.Get_SupportedServices(aImplServices);
	aModule.Get_ServiceDependencies(aNeededServices);

	unsigned nImplServicesSize = aImplServices.size();
	unsigned nNeededServicesSize = aNeededServices.size();

	for ( unsigned i = 0; i < nImplServicesSize; ++i )
	{
		const MultipleTextElement & rImpl = *aImplServices[i];

		unsigned nImplDataSize = rImpl.Size();
		for ( unsigned di = 0; di < nImplDataSize; ++di )
		{
			Simstr sService = ShortName(rImpl.Data(di));
			Service * pService = aServices[sService];
			if (pService == 0)
			{
				pService = new Service(rImpl.Data(di));
                aServices[sService] = pService;
			}
			ServiceInfo & rSInfo = pService->AddImplementation(sModule);

			for ( unsigned n = 0; n < nNeededServicesSize; ++n )
			{
				unsigned nNeededDataSize = aNeededServices[n]->Size();
				for ( unsigned dn = 0; dn < nNeededDataSize; ++dn )
				{
					if (! aNeededServices[n]->Data(dn).is_no_text())
						rSInfo.AddDependency( ShortName(aNeededServices[n]->Data(dn)) );
				}	// end for dn
			}	// end for n
		}	//	end for di
	}	// end for i
}

void
DependencyFinder::Add2Result( const Service & i_rService )
{
	const ServiceInfo & rSInfo = i_rService.FirstImplementation();
	aResult_Libraries.insert(rSInfo.Library());

	const ServiceInfo::List_NeededServices & rNeededs
			= rSInfo.NeededServices();
	for ( StringVector::const_iterator it = rNeededs.begin();
		  it != rNeededs.end();
		  ++it )
	{
		std::pair< std::set< Simstr >::iterator, bool > aInsertResult
				= aResult_Services.insert(*it);
		if (aInsertResult.second)
		{	// Needed service not yet known
			Map_Services::const_iterator itFound = aServices.find(*it);
			if ( itFound == aServices.end() )
			{
				std::cerr << "Needed service \""
						  << (*it).str()
						  << "\" not found,"
						  << std::endl;
			}
			else
			{
				Add2Result( *(*itFound).second );
			}
		} 	// endif (! aInsertResult.second)
	} 	// end for (it)
}



Simstr
ShortName(const Simstr & i_rService)
{
	const char * pStart = i_rService.str();
	const char * pEnd = strchr(pStart,' ');
	if (pEnd != 0)
		return Simstr(pStart, 0, int(pEnd-pStart));
	else
		return i_rService;
}

