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



#include "box.hxx"

#include <tools/debug.hxx>
#include <sal/macros.h>

#include <com/sun/star/awt/XWindow2.hpp>

// fixed point precision for distributing error
#define FIXED_PT 16

namespace layoutimpl
{

using namespace css;

Box_Base::ChildData::ChildData( uno::Reference< awt::XLayoutConstrains > const& xChild )
    : mxChild( xChild )
    , mxProps()
    , maRequisition()
{
}

static bool isVisible( uno::Reference< awt::XLayoutConstrains > xWidget )
{
    if ( !xWidget.is() )
    {
        DBG_ERROR( "FIXME: invalid child !" );
        return true;
    }

    uno::Reference< awt::XWindow2 > xWindow( xWidget, uno::UNO_QUERY );
    if ( xWindow.is() && !xWindow->isVisible() )
        return false;

    uno::Reference< awt::XLayoutContainer > xContainer( xWidget, uno::UNO_QUERY );
    if ( xContainer.is() )
    {
        uno::Sequence< uno::Reference< awt::XLayoutConstrains > > aChildren
            = xContainer->getChildren();

        if (!aChildren.getLength ())
            if (Container *c = dynamic_cast <Container*> (xWidget.get ()))
                return c->emptyVisible ();

        for ( int i = 0; i < aChildren.getLength(); i++ )
            if ( isVisible( aChildren[i] ) )
                return true;
        return false; // this would kill flow without workaround above
    }

    return true;
}
    
bool Box_Base::ChildData::isVisible()
{
    // FIXME: call the 'isVisible' method on it ?
    return layoutimpl::isVisible( mxChild );
}

void
Box_Base::AddChild (uno::Reference <awt::XLayoutConstrains> const& xChild)
{
    ChildData *pData = createChild (xChild);
    maChildren.push_back (pData);
    queueResize ();
}

void SAL_CALL
Box_Base::addChild (uno::Reference <awt::XLayoutConstrains> const& xChild)
    throw (uno::RuntimeException, awt::MaxChildrenException)
{
    if (xChild.is ())
    {
        AddChild (xChild);
        setChildParent (xChild);
    }
}

Box_Base::ChildData*
Box_Base::removeChildData( std::list< ChildData* >& lst, css::uno::Reference< css::awt::XLayoutConstrains > const& xChild )
{
    for ( std::list< ChildData* >::iterator it = lst.begin();
          it != lst.end(); it++ )
    {
        if ( (*it)->mxChild == xChild )
        {
            ChildData* pRet = *it;
            lst.erase( it );
            return pRet;
        }
    }
    return 0;
}

void SAL_CALL
Box_Base::removeChild( const uno::Reference< awt::XLayoutConstrains >& xChild )
    throw (uno::RuntimeException)
{
    if ( ChildData* p = removeChildData( maChildren, xChild ) )
    {
        delete p;
        unsetChildParent( xChild );
        queueResize();
    }
    else
    {
        DBG_ERROR( "Box_Base: removeChild: no such child" );
    }
}

uno::Sequence< uno::Reference < awt::XLayoutConstrains > > SAL_CALL
Box_Base::getChildren()
    throw (uno::RuntimeException)
{
    uno::Sequence< uno::Reference< awt::XLayoutConstrains > > children( maChildren.size() );
    unsigned int index = 0;
    for ( std::list< ChildData* >::iterator it = maChildren.begin();
          it != maChildren.end(); it++, index++ )
        children[index] = ( *it )->mxChild;

    return children;
}

uno::Reference< beans::XPropertySet > SAL_CALL
Box_Base::getChildProperties( const uno::Reference< awt::XLayoutConstrains >& xChild )
    throw (uno::RuntimeException)
{
    
    for ( std::list< ChildData * >::iterator it = maChildren.begin();
          it != maChildren.end(); it++)
    {
        if ( ( *it )->mxChild == xChild )
        {
            if ( !( *it )->mxProps.is() )
            {
                PropHelper *pProps = createChildProps( *it );
                pProps->setChangeListener( this );
                ( *it )->mxProps = pProps;
            }
            return (*it)->mxProps;
        }
    }
    return uno::Reference< beans::XPropertySet >();
}

} // namespace layoutimpl
