local t={
  ImGui_ImplGlfw_CharCallback={
    [1]={
      args="(GLFWwindow* window,unsigned int c)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="c",
          type="unsigned int"}},
      argsoriginal="(GLFWwindow* window,unsigned int c)",
      call_args="(window,c)",
      cimguiname="ImGui_ImplGlfw_CharCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_CharCallback",
      location="imgui_impl_glfw:61",
      ov_cimguiname="ImGui_ImplGlfw_CharCallback",
      ret="void",
      signature="(GLFWwindow*,unsigned int)",
      stname=""},
    ["(GLFWwindow*,unsigned int)"]=nil},
  ImGui_ImplGlfw_CursorEnterCallback={
    [1]={
      args="(GLFWwindow* window,int entered)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="entered",
          type="int"}},
      argsoriginal="(GLFWwindow* window,int entered)",
      call_args="(window,entered)",
      cimguiname="ImGui_ImplGlfw_CursorEnterCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_CursorEnterCallback",
      location="imgui_impl_glfw:56",
      ov_cimguiname="ImGui_ImplGlfw_CursorEnterCallback",
      ret="void",
      signature="(GLFWwindow*,int)",
      stname=""},
    ["(GLFWwindow*,int)"]=nil},
  ImGui_ImplGlfw_CursorPosCallback={
    [1]={
      args="(GLFWwindow* window,double x,double y)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="x",
          type="double"},
        [3]={
          name="y",
          type="double"}},
      argsoriginal="(GLFWwindow* window,double x,double y)",
      call_args="(window,x,y)",
      cimguiname="ImGui_ImplGlfw_CursorPosCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_CursorPosCallback",
      location="imgui_impl_glfw:57",
      ov_cimguiname="ImGui_ImplGlfw_CursorPosCallback",
      ret="void",
      signature="(GLFWwindow*,double,double)",
      stname=""},
    ["(GLFWwindow*,double,double)"]=nil},
  ImGui_ImplGlfw_InitForOpenGL={
    [1]={
      args="(GLFWwindow* window,bool install_callbacks)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="install_callbacks",
          type="bool"}},
      argsoriginal="(GLFWwindow* window,bool install_callbacks)",
      call_args="(window,install_callbacks)",
      cimguiname="ImGui_ImplGlfw_InitForOpenGL",
      defaults={},
      funcname="ImGui_ImplGlfw_InitForOpenGL",
      location="imgui_impl_glfw:32",
      ov_cimguiname="ImGui_ImplGlfw_InitForOpenGL",
      ret="bool",
      signature="(GLFWwindow*,bool)",
      stname=""},
    ["(GLFWwindow*,bool)"]=nil},
  ImGui_ImplGlfw_InitForOther={
    [1]={
      args="(GLFWwindow* window,bool install_callbacks)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="install_callbacks",
          type="bool"}},
      argsoriginal="(GLFWwindow* window,bool install_callbacks)",
      call_args="(window,install_callbacks)",
      cimguiname="ImGui_ImplGlfw_InitForOther",
      defaults={},
      funcname="ImGui_ImplGlfw_InitForOther",
      location="imgui_impl_glfw:34",
      ov_cimguiname="ImGui_ImplGlfw_InitForOther",
      ret="bool",
      signature="(GLFWwindow*,bool)",
      stname=""},
    ["(GLFWwindow*,bool)"]=nil},
  ImGui_ImplGlfw_InitForVulkan={
    [1]={
      args="(GLFWwindow* window,bool install_callbacks)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="install_callbacks",
          type="bool"}},
      argsoriginal="(GLFWwindow* window,bool install_callbacks)",
      call_args="(window,install_callbacks)",
      cimguiname="ImGui_ImplGlfw_InitForVulkan",
      defaults={},
      funcname="ImGui_ImplGlfw_InitForVulkan",
      location="imgui_impl_glfw:33",
      ov_cimguiname="ImGui_ImplGlfw_InitForVulkan",
      ret="bool",
      signature="(GLFWwindow*,bool)",
      stname=""},
    ["(GLFWwindow*,bool)"]=nil},
  ImGui_ImplGlfw_InstallCallbacks={
    [1]={
      args="(GLFWwindow* window)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"}},
      argsoriginal="(GLFWwindow* window)",
      call_args="(window)",
      cimguiname="ImGui_ImplGlfw_InstallCallbacks",
      defaults={},
      funcname="ImGui_ImplGlfw_InstallCallbacks",
      location="imgui_impl_glfw:47",
      ov_cimguiname="ImGui_ImplGlfw_InstallCallbacks",
      ret="void",
      signature="(GLFWwindow*)",
      stname=""},
    ["(GLFWwindow*)"]=nil},
  ImGui_ImplGlfw_KeyCallback={
    [1]={
      args="(GLFWwindow* window,int key,int scancode,int action,int mods)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="key",
          type="int"},
        [3]={
          name="scancode",
          type="int"},
        [4]={
          name="action",
          type="int"},
        [5]={
          name="mods",
          type="int"}},
      argsoriginal="(GLFWwindow* window,int key,int scancode,int action,int mods)",
      call_args="(window,key,scancode,action,mods)",
      cimguiname="ImGui_ImplGlfw_KeyCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_KeyCallback",
      location="imgui_impl_glfw:60",
      ov_cimguiname="ImGui_ImplGlfw_KeyCallback",
      ret="void",
      signature="(GLFWwindow*,int,int,int,int)",
      stname=""},
    ["(GLFWwindow*,int,int,int,int)"]=nil},
  ImGui_ImplGlfw_MonitorCallback={
    [1]={
      args="(GLFWmonitor* monitor,int event)",
      argsT={
        [1]={
          name="monitor",
          type="GLFWmonitor*"},
        [2]={
          name="event",
          type="int"}},
      argsoriginal="(GLFWmonitor* monitor,int event)",
      call_args="(monitor,event)",
      cimguiname="ImGui_ImplGlfw_MonitorCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_MonitorCallback",
      location="imgui_impl_glfw:62",
      ov_cimguiname="ImGui_ImplGlfw_MonitorCallback",
      ret="void",
      signature="(GLFWmonitor*,int)",
      stname=""},
    ["(GLFWmonitor*,int)"]=nil},
  ImGui_ImplGlfw_MouseButtonCallback={
    [1]={
      args="(GLFWwindow* window,int button,int action,int mods)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="button",
          type="int"},
        [3]={
          name="action",
          type="int"},
        [4]={
          name="mods",
          type="int"}},
      argsoriginal="(GLFWwindow* window,int button,int action,int mods)",
      call_args="(window,button,action,mods)",
      cimguiname="ImGui_ImplGlfw_MouseButtonCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_MouseButtonCallback",
      location="imgui_impl_glfw:58",
      ov_cimguiname="ImGui_ImplGlfw_MouseButtonCallback",
      ret="void",
      signature="(GLFWwindow*,int,int,int)",
      stname=""},
    ["(GLFWwindow*,int,int,int)"]=nil},
  ImGui_ImplGlfw_NewFrame={
    [1]={
      args="()",
      argsT={},
      argsoriginal="()",
      call_args="()",
      cimguiname="ImGui_ImplGlfw_NewFrame",
      defaults={},
      funcname="ImGui_ImplGlfw_NewFrame",
      location="imgui_impl_glfw:36",
      ov_cimguiname="ImGui_ImplGlfw_NewFrame",
      ret="void",
      signature="()",
      stname=""},
    ["()"]=nil},
  ImGui_ImplGlfw_RestoreCallbacks={
    [1]={
      args="(GLFWwindow* window)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"}},
      argsoriginal="(GLFWwindow* window)",
      call_args="(window)",
      cimguiname="ImGui_ImplGlfw_RestoreCallbacks",
      defaults={},
      funcname="ImGui_ImplGlfw_RestoreCallbacks",
      location="imgui_impl_glfw:48",
      ov_cimguiname="ImGui_ImplGlfw_RestoreCallbacks",
      ret="void",
      signature="(GLFWwindow*)",
      stname=""},
    ["(GLFWwindow*)"]=nil},
  ImGui_ImplGlfw_ScrollCallback={
    [1]={
      args="(GLFWwindow* window,double xoffset,double yoffset)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="xoffset",
          type="double"},
        [3]={
          name="yoffset",
          type="double"}},
      argsoriginal="(GLFWwindow* window,double xoffset,double yoffset)",
      call_args="(window,xoffset,yoffset)",
      cimguiname="ImGui_ImplGlfw_ScrollCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_ScrollCallback",
      location="imgui_impl_glfw:59",
      ov_cimguiname="ImGui_ImplGlfw_ScrollCallback",
      ret="void",
      signature="(GLFWwindow*,double,double)",
      stname=""},
    ["(GLFWwindow*,double,double)"]=nil},
  ImGui_ImplGlfw_SetCallbacksChainForAllWindows={
    [1]={
      args="(bool chain_for_all_windows)",
      argsT={
        [1]={
          name="chain_for_all_windows",
          type="bool"}},
      argsoriginal="(bool chain_for_all_windows)",
      call_args="(chain_for_all_windows)",
      cimguiname="ImGui_ImplGlfw_SetCallbacksChainForAllWindows",
      defaults={},
      funcname="ImGui_ImplGlfw_SetCallbacksChainForAllWindows",
      location="imgui_impl_glfw:52",
      ov_cimguiname="ImGui_ImplGlfw_SetCallbacksChainForAllWindows",
      ret="void",
      signature="(bool)",
      stname=""},
    ["(bool)"]=nil},
  ImGui_ImplGlfw_Shutdown={
    [1]={
      args="()",
      argsT={},
      argsoriginal="()",
      call_args="()",
      cimguiname="ImGui_ImplGlfw_Shutdown",
      defaults={},
      funcname="ImGui_ImplGlfw_Shutdown",
      location="imgui_impl_glfw:35",
      ov_cimguiname="ImGui_ImplGlfw_Shutdown",
      ret="void",
      signature="()",
      stname=""},
    ["()"]=nil},
  ImGui_ImplGlfw_Sleep={
    [1]={
      args="(int milliseconds)",
      argsT={
        [1]={
          name="milliseconds",
          type="int"}},
      argsoriginal="(int milliseconds)",
      call_args="(milliseconds)",
      cimguiname="ImGui_ImplGlfw_Sleep",
      defaults={},
      funcname="ImGui_ImplGlfw_Sleep",
      location="imgui_impl_glfw:65",
      ov_cimguiname="ImGui_ImplGlfw_Sleep",
      ret="void",
      signature="(int)",
      stname=""},
    ["(int)"]=nil},
  ImGui_ImplGlfw_WindowFocusCallback={
    [1]={
      args="(GLFWwindow* window,int focused)",
      argsT={
        [1]={
          name="window",
          type="GLFWwindow*"},
        [2]={
          name="focused",
          type="int"}},
      argsoriginal="(GLFWwindow* window,int focused)",
      call_args="(window,focused)",
      cimguiname="ImGui_ImplGlfw_WindowFocusCallback",
      defaults={},
      funcname="ImGui_ImplGlfw_WindowFocusCallback",
      location="imgui_impl_glfw:55",
      ov_cimguiname="ImGui_ImplGlfw_WindowFocusCallback",
      ret="void",
      signature="(GLFWwindow*,int)",
      stname=""},
    ["(GLFWwindow*,int)"]=nil},
  ImGui_ImplWGPU_CreateDeviceObjects={
    [1]={
      args="()",
      argsT={},
      argsoriginal="()",
      call_args="()",
      cimguiname="ImGui_ImplWGPU_CreateDeviceObjects",
      defaults={},
      funcname="ImGui_ImplWGPU_CreateDeviceObjects",
      location="imgui_impl_wgpu:57",
      ov_cimguiname="ImGui_ImplWGPU_CreateDeviceObjects",
      ret="bool",
      signature="()",
      stname=""},
    ["()"]=nil},
  ImGui_ImplWGPU_Init={
    [1]={
      args="(ImGui_ImplWGPU_InitInfo* init_info)",
      argsT={
        [1]={
          name="init_info",
          type="ImGui_ImplWGPU_InitInfo*"}},
      argsoriginal="(ImGui_ImplWGPU_InitInfo* init_info)",
      call_args="(init_info)",
      cimguiname="ImGui_ImplWGPU_Init",
      defaults={},
      funcname="ImGui_ImplWGPU_Init",
      location="imgui_impl_wgpu:51",
      ov_cimguiname="ImGui_ImplWGPU_Init",
      ret="bool",
      signature="(ImGui_ImplWGPU_InitInfo*)",
      stname=""},
    ["(ImGui_ImplWGPU_InitInfo*)"]=nil},
  ImGui_ImplWGPU_InitInfo_ImGui_ImplWGPU_InitInfo={
    [1]={
      args="()",
      argsT={},
      argsoriginal="()",
      call_args="()",
      cimguiname="ImGui_ImplWGPU_InitInfo_ImGui_ImplWGPU_InitInfo",
      constructor=true,
      defaults={},
      funcname="ImGui_ImplWGPU_InitInfo",
      location="imgui_impl_wgpu:42",
      ov_cimguiname="ImGui_ImplWGPU_InitInfo_ImGui_ImplWGPU_InitInfo",
      signature="()",
      stname="ImGui_ImplWGPU_InitInfo"},
    ["()"]=nil},
  ImGui_ImplWGPU_InitInfo_destroy={
    [1]={
      args="(ImGui_ImplWGPU_InitInfo* self)",
      argsT={
        [1]={
          name="self",
          type="ImGui_ImplWGPU_InitInfo*"}},
      call_args="(self)",
      cimguiname="ImGui_ImplWGPU_InitInfo_destroy",
      defaults={},
      destructor=true,
      location="imgui_impl_wgpu:42",
      ov_cimguiname="ImGui_ImplWGPU_InitInfo_destroy",
      ret="void",
      signature="(ImGui_ImplWGPU_InitInfo*)",
      stname="ImGui_ImplWGPU_InitInfo"},
    ["(ImGui_ImplWGPU_InitInfo*)"]=nil},
  ImGui_ImplWGPU_InvalidateDeviceObjects={
    [1]={
      args="()",
      argsT={},
      argsoriginal="()",
      call_args="()",
      cimguiname="ImGui_ImplWGPU_InvalidateDeviceObjects",
      defaults={},
      funcname="ImGui_ImplWGPU_InvalidateDeviceObjects",
      location="imgui_impl_wgpu:58",
      ov_cimguiname="ImGui_ImplWGPU_InvalidateDeviceObjects",
      ret="void",
      signature="()",
      stname=""},
    ["()"]=nil},
  ImGui_ImplWGPU_NewFrame={
    [1]={
      args="()",
      argsT={},
      argsoriginal="()",
      call_args="()",
      cimguiname="ImGui_ImplWGPU_NewFrame",
      defaults={},
      funcname="ImGui_ImplWGPU_NewFrame",
      location="imgui_impl_wgpu:53",
      ov_cimguiname="ImGui_ImplWGPU_NewFrame",
      ret="void",
      signature="()",
      stname=""},
    ["()"]=nil},
  ImGui_ImplWGPU_RenderDrawData={
    [1]={
      args="(ImDrawData* draw_data,WGPURenderPassEncoder pass_encoder)",
      argsT={
        [1]={
          name="draw_data",
          type="ImDrawData*"},
        [2]={
          name="pass_encoder",
          type="WGPURenderPassEncoder"}},
      argsoriginal="(ImDrawData* draw_data,WGPURenderPassEncoder pass_encoder)",
      call_args="(draw_data,pass_encoder)",
      cimguiname="ImGui_ImplWGPU_RenderDrawData",
      defaults={},
      funcname="ImGui_ImplWGPU_RenderDrawData",
      location="imgui_impl_wgpu:54",
      ov_cimguiname="ImGui_ImplWGPU_RenderDrawData",
      ret="void",
      signature="(ImDrawData*,WGPURenderPassEncoder)",
      stname=""},
    ["(ImDrawData*,WGPURenderPassEncoder)"]=nil},
  ImGui_ImplWGPU_Shutdown={
    [1]={
      args="()",
      argsT={},
      argsoriginal="()",
      call_args="()",
      cimguiname="ImGui_ImplWGPU_Shutdown",
      defaults={},
      funcname="ImGui_ImplWGPU_Shutdown",
      location="imgui_impl_wgpu:52",
      ov_cimguiname="ImGui_ImplWGPU_Shutdown",
      ret="void",
      signature="()",
      stname=""},
    ["()"]=nil}}
t.ImGui_ImplGlfw_CharCallback["(GLFWwindow*,unsigned int)"]=t.ImGui_ImplGlfw_CharCallback[1]
t.ImGui_ImplGlfw_CursorEnterCallback["(GLFWwindow*,int)"]=t.ImGui_ImplGlfw_CursorEnterCallback[1]
t.ImGui_ImplGlfw_CursorPosCallback["(GLFWwindow*,double,double)"]=t.ImGui_ImplGlfw_CursorPosCallback[1]
t.ImGui_ImplGlfw_InitForOpenGL["(GLFWwindow*,bool)"]=t.ImGui_ImplGlfw_InitForOpenGL[1]
t.ImGui_ImplGlfw_InitForOther["(GLFWwindow*,bool)"]=t.ImGui_ImplGlfw_InitForOther[1]
t.ImGui_ImplGlfw_InitForVulkan["(GLFWwindow*,bool)"]=t.ImGui_ImplGlfw_InitForVulkan[1]
t.ImGui_ImplGlfw_InstallCallbacks["(GLFWwindow*)"]=t.ImGui_ImplGlfw_InstallCallbacks[1]
t.ImGui_ImplGlfw_KeyCallback["(GLFWwindow*,int,int,int,int)"]=t.ImGui_ImplGlfw_KeyCallback[1]
t.ImGui_ImplGlfw_MonitorCallback["(GLFWmonitor*,int)"]=t.ImGui_ImplGlfw_MonitorCallback[1]
t.ImGui_ImplGlfw_MouseButtonCallback["(GLFWwindow*,int,int,int)"]=t.ImGui_ImplGlfw_MouseButtonCallback[1]
t.ImGui_ImplGlfw_NewFrame["()"]=t.ImGui_ImplGlfw_NewFrame[1]
t.ImGui_ImplGlfw_RestoreCallbacks["(GLFWwindow*)"]=t.ImGui_ImplGlfw_RestoreCallbacks[1]
t.ImGui_ImplGlfw_ScrollCallback["(GLFWwindow*,double,double)"]=t.ImGui_ImplGlfw_ScrollCallback[1]
t.ImGui_ImplGlfw_SetCallbacksChainForAllWindows["(bool)"]=t.ImGui_ImplGlfw_SetCallbacksChainForAllWindows[1]
t.ImGui_ImplGlfw_Shutdown["()"]=t.ImGui_ImplGlfw_Shutdown[1]
t.ImGui_ImplGlfw_Sleep["(int)"]=t.ImGui_ImplGlfw_Sleep[1]
t.ImGui_ImplGlfw_WindowFocusCallback["(GLFWwindow*,int)"]=t.ImGui_ImplGlfw_WindowFocusCallback[1]
t.ImGui_ImplWGPU_CreateDeviceObjects["()"]=t.ImGui_ImplWGPU_CreateDeviceObjects[1]
t.ImGui_ImplWGPU_Init["(ImGui_ImplWGPU_InitInfo*)"]=t.ImGui_ImplWGPU_Init[1]
t.ImGui_ImplWGPU_InitInfo_ImGui_ImplWGPU_InitInfo["()"]=t.ImGui_ImplWGPU_InitInfo_ImGui_ImplWGPU_InitInfo[1]
t.ImGui_ImplWGPU_InitInfo_destroy["(ImGui_ImplWGPU_InitInfo*)"]=t.ImGui_ImplWGPU_InitInfo_destroy[1]
t.ImGui_ImplWGPU_InvalidateDeviceObjects["()"]=t.ImGui_ImplWGPU_InvalidateDeviceObjects[1]
t.ImGui_ImplWGPU_NewFrame["()"]=t.ImGui_ImplWGPU_NewFrame[1]
t.ImGui_ImplWGPU_RenderDrawData["(ImDrawData*,WGPURenderPassEncoder)"]=t.ImGui_ImplWGPU_RenderDrawData[1]
t.ImGui_ImplWGPU_Shutdown["()"]=t.ImGui_ImplWGPU_Shutdown[1]
 return t