#pragma once

#include <wx/wx.h>

// Send from thread on exit
wxDECLARE_EVENT(nxRENDERER_EXIT_EVENT, wxCommandEvent);
wxDECLARE_EVENT(nxSCHEDULER_EXIT_EVENT, wxCommandEvent); 
wxDECLARE_EVENT(nxPROGRAM_ADDED_EVENT, wxCommandEvent);