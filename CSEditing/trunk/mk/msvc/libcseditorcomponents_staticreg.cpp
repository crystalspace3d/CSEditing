// This file is automatically generated.
#include "cssysdef.h"
#include "csutil/scf.h"

// Put static linking stuff into own section.
// The idea is that this allows the section to be swapped out but not
// swapped in again b/c something else in it was needed.
#if !defined(CS_DEBUG) && defined(CS_COMPILER_MSVC)
#pragma const_seg(".CSmetai")
#pragma comment(linker, "/section:.CSmetai,r")
#pragma code_seg(".CSmeta")
#pragma comment(linker, "/section:.CSmeta,er")
#pragma comment(linker, "/merge:.CSmetai=.CSmeta")
#endif

namespace csStaticPluginInit
{
static char const metainfo_cseditorcomponents[] =
"<?xml version=\"1.0\"?>"
"<!-- cseditorcomponents.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>crystalspace.editor.component.2dinfo</name>"
"        <implementation>CS2DInfo</implementation>"
"        <description>2D Info</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.engine</name>"
"        <implementation>CS3DEngine</implementation>"
"        <description>Base engine and menu entries</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.exit</name>"
"        <implementation>Exit</implementation>"
"        <description>Management of application exit</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.logger</name>"
"        <implementation>Logger</implementation>"
"        <description>Logging of report messages</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.maploader</name>"
"        <implementation>MapLoader</implementation>"
"        <description>Default map and library loader</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.perspectives</name>"
"        <implementation>Perspectives</implementation>"
"        <description>Perspective management</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.scenemanager</name>"
"        <implementation>SceneManager</implementation>"
"        <description>Scene and camera management</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.translation</name>"
"        <implementation>EditorTranslation</implementation>"
"        <description>Management of the translation component</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.editor.component.undoredo</name>"
"        <implementation>UndoRedo</implementation>"
"        <description>Undo and Redo operations</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef CS2DInfo_FACTORY_REGISTER_DEFINED 
  #define CS2DInfo_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(CS2DInfo) 
  #endif
  #ifndef CS3DEngine_FACTORY_REGISTER_DEFINED 
  #define CS3DEngine_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(CS3DEngine) 
  #endif
  #ifndef Exit_FACTORY_REGISTER_DEFINED 
  #define Exit_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(Exit) 
  #endif
  #ifndef Logger_FACTORY_REGISTER_DEFINED 
  #define Logger_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(Logger) 
  #endif
  #ifndef MapLoader_FACTORY_REGISTER_DEFINED 
  #define MapLoader_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(MapLoader) 
  #endif
  #ifndef Perspectives_FACTORY_REGISTER_DEFINED 
  #define Perspectives_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(Perspectives) 
  #endif
  #ifndef SceneManager_FACTORY_REGISTER_DEFINED 
  #define SceneManager_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(SceneManager) 
  #endif
  #ifndef EditorTranslation_FACTORY_REGISTER_DEFINED 
  #define EditorTranslation_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(EditorTranslation) 
  #endif
  #ifndef UndoRedo_FACTORY_REGISTER_DEFINED 
  #define UndoRedo_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(UndoRedo) 
  #endif

class cseditorcomponents
{
SCF_REGISTER_STATIC_LIBRARY(cseditorcomponents,metainfo_cseditorcomponents)
  #ifndef CS2DInfo_FACTORY_REGISTERED 
  #define CS2DInfo_FACTORY_REGISTERED 
    CS2DInfo_StaticInit CS2DInfo_static_init__; 
  #endif
  #ifndef CS3DEngine_FACTORY_REGISTERED 
  #define CS3DEngine_FACTORY_REGISTERED 
    CS3DEngine_StaticInit CS3DEngine_static_init__; 
  #endif
  #ifndef Exit_FACTORY_REGISTERED 
  #define Exit_FACTORY_REGISTERED 
    Exit_StaticInit Exit_static_init__; 
  #endif
  #ifndef Logger_FACTORY_REGISTERED 
  #define Logger_FACTORY_REGISTERED 
    Logger_StaticInit Logger_static_init__; 
  #endif
  #ifndef MapLoader_FACTORY_REGISTERED 
  #define MapLoader_FACTORY_REGISTERED 
    MapLoader_StaticInit MapLoader_static_init__; 
  #endif
  #ifndef Perspectives_FACTORY_REGISTERED 
  #define Perspectives_FACTORY_REGISTERED 
    Perspectives_StaticInit Perspectives_static_init__; 
  #endif
  #ifndef SceneManager_FACTORY_REGISTERED 
  #define SceneManager_FACTORY_REGISTERED 
    SceneManager_StaticInit SceneManager_static_init__; 
  #endif
  #ifndef EditorTranslation_FACTORY_REGISTERED 
  #define EditorTranslation_FACTORY_REGISTERED 
    EditorTranslation_StaticInit EditorTranslation_static_init__; 
  #endif
  #ifndef UndoRedo_FACTORY_REGISTERED 
  #define UndoRedo_FACTORY_REGISTERED 
    UndoRedo_StaticInit UndoRedo_static_init__; 
  #endif
public:
 cseditorcomponents();
};
cseditorcomponents::cseditorcomponents() {}

}
