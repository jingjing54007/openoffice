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

#ifndef SFX_SIDEBAR_TOOLBOX_HXX
#define SFX_SIDEBAR_TOOLBOX_HXX

#include "vcl/toolbox.hxx"


namespace sfx2 { namespace sidebar {

class ToolBox
    : public ::ToolBox
{
public:
    MenuButton (Window* pParentWindow);
    virtual ~MenuButton (void);

    virtual void Paint (const Rectangle& rUpdateArea);
    virtual void MouseMove (const MouseEvent& rEvent);
    virtual void MouseButtonDown (const MouseEvent& rMouseEvent);
    virtual void MouseButtonUp (const MouseEvent& rMouseEvent);

protected:
    using CheckBox::FillLayoutData;
    
private:
    bool mbIsLeftButtonDown;
    enum PaintType {
        PT_Native,
        PT_Theme
    } mePaintType;
};


} } // end of namespace sfx2::sidebar

#endif
