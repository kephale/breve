/////////////////////////////////////////////////////////////////////////////
// Name:		BreveRender.cpp
// Purpose:	 
// Author:	  
// Modified by: 
// Created:	 Tue 01 Mar 2005 10:16:35 PM EST
// RCS-ID:	  
// Copyright:   
// Licence:	 
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "BreveRender.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <unistd.h>

#include <wx/file.h>
#include <wx/filename.h>
#include <wx/tglbtn.h>
#include <wx/dir.h>
#include <wx/confbase.h>

#include "BreveRender.h"
#include "BreveCanvas.h"
#include "BreveInterface.h"
#include "LogWindow.h"
#include "Main.h"
#include "steve.h"
#include "SimInstance.h"
#include "Inspector.h"
#include "gldraw.h"

#include "images/stop.xpm"
#include "images/play.xpm"
#include "images/pause.xpm"

IMPLEMENT_CLASS( BreveRender, wxFrame )

BEGIN_EVENT_TABLE( BreveRender, wxFrame )
	EVT_KEY_UP(BreveRender::OnKeyUp)
	EVT_KEY_DOWN(BreveRender::OnKeyDown)
	EVT_BUTTON( ID_RENDER_RUN, BreveRender::OnRenderRunClick )
	EVT_BUTTON( ID_RENDER_STOP, BreveRender::OnRenderStopClick )
	EVT_TOGGLEBUTTON( ID_ROTATE,	BreveRender::OnRotateClick )
	EVT_TOGGLEBUTTON( ID_ZOOM,		BreveRender::OnZoomClick )
	EVT_TOGGLEBUTTON( ID_MOVE,		BreveRender::OnMoveClick )
	EVT_TOGGLEBUTTON( ID_SELECT,	BreveRender::OnSelectClick )
	EVT_MENU( BREVE_FILEMENU_NEW, BreveRender::OnMenuNew )
	EVT_MENU( BREVE_FILEMENU_NEW_FROM_TEMPLATE, BreveRender::OnMenuNewFromTemplate )
	EVT_MENU( BREVE_FILEMENU_OPEN, BreveRender::OnMenuOpen )
	EVT_MENU( BREVE_FILEMENU_QUIT, BreveRender::OnMenuQuit )
	EVT_IDLE(BreveRender::OnIdle)
	EVT_MENU( BREVE_WINDOWMENU_LOG, BreveRender::OnMenuLogWindow )
	EVT_MENU( BREVE_WINDOWMENU_INSPECTOR, BreveRender::OnMenuInspector )
	EVT_MENU( BREVE_WINDOWMENU_FULLSCREEN, BreveRender::OnFullScreen )
	EVT_MENU( BREVE_FULLSPEED, BreveRender::OnMenuSpeed )
	EVT_MENU( BREVE_MEDIUMSPEED, BreveRender::OnMenuSpeed )
	EVT_MENU( BREVE_SLOWSPEED, BreveRender::OnMenuSpeed )
	EVT_MENU_RANGE( BREVE_DEMOMENU + 1500, BREVE_DEMOMENU + 2000, BreveRender::OnMenuSim )
	EVT_MENU_RANGE( BREVE_SIMMENU, BREVE_SIMMENU + 1000, BreveRender::OnSimMenu )
	EVT_MENU_RANGE( BREVE_DOCMENU, BREVE_DOCMENU + 1000, BreveRender::OnDocMenu )
	EVT_CLOSE (BreveRender::OnClose)
	EVT_CHOICE(ID_SIM_SELECT, BreveRender::OnSimSelect)
	EVT_MOVE(BreveRender::OnMove)
	EVT_SIZE(BreveRender::OnSize)
END_EVENT_TABLE()

BreveRender::BreveRender( )
{
	for( int i = 0; i < 256; i++ ) mKeysDown[ i ] = false;

	mSleepMS = 0;
}

BreveRender::BreveRender( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
	int zero = 1;
	char *argv[] = { "breveIDE" };
	glutInit( &zero, argv );

	mSleepMS = 0;
	Create( parent, id, caption, pos, size, style );
}

BreveRender::~BreveRender() {
	SimInstance * s,* n;

	if( GetSimulation() ) GetSimulation()->RegenSim();

	for (s = simlist; s != NULL; s = n) {
		n = s->GetNext();

		delete s;
	}

	simlist = NULL;
}

bool BreveRender::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	bool loadedvalues = FALSE;
	wxSize tsize;
	wxPoint tpoint;

	tsize = size;
	tpoint = pos;

	wxConfigBase * config = wxConfigBase::Get();

	if (config != NULL)
	{
		int x, y, w, h;

		if (config->Read("BreveRenderX", &x) && config->Read("BreveRenderY", &y) &&
			config->Read("BreveRenderWidth", &w) && config->Read("BreveRenderHeight", &h)) {
			tpoint.x = x;
			tpoint.y = y;
			tsize.SetHeight(h);
			tsize.SetWidth(w);
			loadedvalues = TRUE;
		}
	}

	wxFrame::Create( parent, id, caption, tpoint, tsize, style );

#ifdef __WXMSW__
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
#endif

	this->simlist = NULL;
	this->cursim = 0;

	mousemode = 0;

	CreateControls();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	//Centre();

	if (!loadedvalues)
	Move(0, 0);
	else
	{
	SetSize(tsize);
	Move(tpoint);
	}

	{
	wxRect m;

	m = GetRect();

	logwindow = new LogWindow(this);

	if (!loadedvalues)
		logwindow->Move(m.x, m.y + m.height + 30);

	inspector = new Inspector(this);

	if (!loadedvalues)
		inspector->Move(m.x + m.width + 10, m.y);
	}

	Show(TRUE);

	return TRUE;
}

void BreveRender::CreateControls()
{	
	BreveRender* itemFrame1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemFrame1->SetSizer(itemBoxSizer2);

	canvas = new BreveCanvas(this);

	wxWindow* itemWindow3 = canvas;
	wxASSERT( itemWindow3 != NULL );

#ifdef __WXMSW__
	itemBoxSizer2->Add(itemWindow3, 1, wxEXPAND|wxGROW|wxALL|wxADJUST_MINSIZE, 0);
#else
	itemBoxSizer2->Add(itemWindow3, 1, wxEXPAND|wxGROW|wxALL|wxADJUST_MINSIZE, 5);
#endif

	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);

#ifdef __WXMSW__
	itemBoxSizer2->Add(itemBoxSizer4, 0, wxEXPAND|wxGROW|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5);
#else
	itemBoxSizer2->Add(itemBoxSizer4, 0, wxEXPAND|wxGROW|wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM|wxLEFT, 5);
#endif

	{
		rotatebutton = new wxToggleButton(this, ID_ROTATE, "Rotate");
#ifdef __WXGTK__
		rotatebutton->SetFont(*wxSMALL_FONT);
#endif
		rotatebutton->SetMinSize(wxSize(rotatebutton->GetSize().GetWidth() / 2, rotatebutton->GetSize().GetHeight()));
		itemBoxSizer4->Add(rotatebutton, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL, 5);
		rotatebutton->SetValue(TRUE);

		zoombutton = new wxToggleButton(this, ID_ZOOM, "Zoom");
#ifdef __WXGTK__
		zoombutton->SetFont(*wxSMALL_FONT);
#endif
		zoombutton->SetMinSize(wxSize(zoombutton->GetSize().GetWidth() / 2, zoombutton->GetSize().GetHeight()));
		itemBoxSizer4->Add(zoombutton, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL, 5);

		movebutton = new wxToggleButton(this, ID_MOVE, "Move");
#ifdef __WXGTK__
		movebutton->SetFont(*wxSMALL_FONT);
#endif
		movebutton->SetMinSize(wxSize(movebutton->GetSize().GetWidth() / 2, movebutton->GetSize().GetHeight()));
		itemBoxSizer4->Add(movebutton, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL, 5);

		selectbutton = new wxToggleButton(this, ID_SELECT, "Select");
#ifdef __WXGTK__
		selectbutton->SetFont(*wxSMALL_FONT);
#endif
		selectbutton->SetMinSize(wxSize(selectbutton->GetSize().GetWidth() * 2 / 3, selectbutton->GetSize().GetHeight()));
		itemBoxSizer4->Add(selectbutton, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
		}
	
		pausebitmap = wxBitmap(pause_xpm);
		playbitmap = wxBitmap(play_xpm);
		stopbitmap = wxBitmap(stop_xpm);
	
		wxBitmapButton* itemButton6 = new wxBitmapButton( itemFrame1, ID_RENDER_STOP, stopbitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _("Stop") );
		itemBoxSizer4->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
		stopbutton = itemButton6;
	
		wxBitmapButton* itemButton5 = new wxBitmapButton( itemFrame1, ID_RENDER_RUN, playbitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _("Run") );
		itemBoxSizer4->Add(itemButton5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
		runbutton = itemButton5;
	
		simselect = new wxChoice(this, ID_SIM_SELECT, wxDefaultPosition, wxDefaultSize, 0 );
		itemBoxSizer4->Add(simselect, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
	
		menubar = new wxMenuBar();
	
		{
		wxMenu * filemenu = new wxMenu;
	
		filemenu->Append(BREVE_FILEMENU_NEW, "&New\tCTRL-N");
		filemenu->Append(BREVE_FILEMENU_NEW_FROM_TEMPLATE, "New From &Template\tCTRL-T");
		filemenu->Append(BREVE_FILEMENU_OPEN, "&Open\tCTRL-O");
		filemenu->Append(BREVE_FILEMENU_QUIT, "E&xit\tCTRL-X");
	
		wxMenu * windowmenu = new wxMenu;
	
		windowmenu->Append(BREVE_WINDOWMENU_LOG, "Log Window");
		windowmenu->Append(BREVE_WINDOWMENU_INSPECTOR, "Object Inspector");
		windowmenu->Append(BREVE_WINDOWMENU_FULLSCREEN, "Fullscreen");
	
		wxMenu * demomenu = new wxMenu;
	
		if (!app->GetBreveDir().IsEmpty()) {
			wxString str;
	
			str = app->GetBreveDir();
	
			str << "demos";
	
			wxDir scan(str);
	
			if (scan.IsOpened()) {
				wxString filename;
				bool cont;
				int count = 1501;
	
				cont = scan.GetFirst(&filename, "*.tz", wxDIR_FILES);
	
				while (cont) {
					demomenu->Append( BREVE_DEMOMENU + count++, filename );
					cont = scan.GetNext(&filename);
				}
	
				cont = scan.GetFirst(&filename, "*.py", wxDIR_FILES);
		
				while (cont) {
					demomenu->Append(BREVE_DEMOMENU + count++, filename);
					cont = scan.GetNext(&filename);
				}
		
				cont = scan.GetFirst(&filename, "", wxDIR_DIRS);
		
				while (cont) {
					str = app->GetBreveDir();
		
					str << "demos" << FILE_SEP_PATH << filename;
		
					wxDir lscan(str);
		
					if (lscan.IsOpened()) {
						wxString subfilename;
						bool cont;
						int reserved = 0;
		
						wxMenu * submenu = new wxMenu;
						reserved = count++;
		
						cont = lscan.GetFirst(&subfilename, "", wxDIR_FILES);
		
						while (cont) {
							wxFileName f = wxFileName( subfilename );

							if( f.GetExt().Cmp( "py" ) || f.GetExt().Cmp( "tz" ) ) {
								submenu->Append(BREVE_DEMOMENU + count++, subfilename);
								cont = lscan.GetNext(&subfilename);
							}
						}
		
						demomenu->Append(BREVE_DEMOMENU + reserved, filename, submenu);
					}
	
				cont = scan.GetNext(&filename);
				}
			}
		}
	
		defsimmenu = new wxMenu;
	
		menubar->Append(filemenu, "&File");
		menubar->Append(windowmenu, "&Window");
		menubar->Append(demomenu, "&Demos");

		wxMenu * simspeed = new wxMenu;
		simspeed->Append( BREVE_FULLSPEED, "Full Speed" );
		simspeed->Append( BREVE_MEDIUMSPEED, "Medium" );
		simspeed->Append( BREVE_SLOWSPEED, "Slow" );
		menubar->Append( simspeed, "Speed" );

		// breve Documentation

		wxMenu *help= MakeDocumentationMenu();
		menubar->Append( help, "&Help" );

		// Placeholder simulation menu

		menubar->Append( defsimmenu, "&Simulation" );

	}

	SetMenuBar(menubar);

	runbutton->SetFocus();

	canvas->Connect(-1, wxEVT_KEY_DOWN, wxKeyEventHandler(BreveRender::KeyDown));
	canvas->Connect(-1, wxEVT_KEY_UP, wxKeyEventHandler(BreveRender::KeyUp));
}

wxMenu *BreveRender::MakeDocumentationMenu() {
	_docFiles.clear();

	wxMenu *docs = new wxMenu;

	wxString index = app->GetBreveDir() << "docs" << FILE_SEP_PATH << "index.html";

	wxString stclasses = app->GetBreveDir() << "docs" << FILE_SEP_PATH << "steveclasses";
	wxString pyclasses = app->GetBreveDir() << "docs" << FILE_SEP_PATH << "pythonclasses";

	docs->Append( BREVE_DOCMENU + _docFiles.size(), "breve Documentation" );
	_docFiles.push_back( index );

	wxMenu *stClassMenu = new wxMenu;
	wxMenu *pyClassMenu = new wxMenu;

	wxDir stscan( stclasses );
	if ( stscan.IsOpened() ) {
		wxString filename;
		bool cont = stscan.GetFirst( &filename, "*.html", wxDIR_FILES );
                        
		while ( cont ) {
			wxString path = app->GetBreveDir() << "docs" << FILE_SEP_PATH << "steveclasses" << FILE_SEP_PATH << filename;

			stClassMenu->Append( BREVE_DOCMENU + _docFiles.size(), filename );
			_docFiles.push_back( path );

			cont = stscan.GetNext( &filename );
		}
	}

	wxDir pyscan( pyclasses );
	if ( pyscan.IsOpened() ) {
		wxString filename;
		bool cont = pyscan.GetFirst( &filename, "*.html", wxDIR_FILES );
                        
		while ( cont ) {
			wxString path = app->GetBreveDir() << "docs" << FILE_SEP_PATH << "pythonclasses" << FILE_SEP_PATH << filename;

			pyClassMenu->Append( BREVE_DOCMENU + _docFiles.size(), filename );
			_docFiles.push_back( path );

			cont = pyscan.GetNext( &filename );
		}
	}


	docs->Append( BREVE_DOCMENU, "Class Documentation (steve)", stClassMenu );
	docs->Append( BREVE_DOCMENU, "Class Documentation (Python)", pyClassMenu );

	return docs;
}

int BreveRender::GetSimInt(SimInstance * s) {
	SimInstance * i;
	int n = 0;

	for (i = simlist; i != NULL; i = i->GetNext())
	{
	if (i == s)
		return n;

	n++;
	}

	return -1;
}

SimInstance * BreveRender::GetSimulation(int num)
{
	SimInstance * s = NULL;
	int i = 0;

	if (num == -1)
	num = cursim;

	for (s = simlist; s != NULL; s = s->GetNext())
	{
	if (i == num)
		return s;

	i++;
	}

	return NULL;
}

void BreveRender::KillSimulation(int num)
{
	SimInstance * s, *n, *l = NULL;
	int i = 0;
	int wascur = 0;

	if (num == -1)
	num = cursim;

	for (s = simlist; s != NULL; s = n)
	{
	n = s->GetNext();

	if (i == num)
	{
		if (i == cursim)
		{
		if (GetSimulation() != NULL)
			GetSimulation()->RegenSim();

		cursim = -1;
		wascur = 1;

		ResetSim();

		canvas->Refresh(TRUE, NULL);
		}
		else if (i < cursim)
		{
		cursim--;
		wascur = -1;
		}

		if (l == NULL)
		simlist = n;
		else
		l->Rechain(n);

		delete s;

		simselect->Delete(num);

		simselect->SetSelection(cursim);

		break;
	}

	i++;
	l = s;
	}

	if (wascur == 1)
	{
	if (GetSimulation(i) != NULL)
		cursim = i;
	else if (GetSimulation(i - 1) != NULL)
		cursim = i - 1;
	else if (GetSimulation(0) != NULL)
		cursim = 0;

	ResetSim();

	if (GetSimulation() != NULL)
		GetSimulation()->RegenSim();
	}
}

void BreveRender::ResetSim( int sim ) {
	SimInstance * s = NULL;

	if (sim != -2)
		cursim = sim;

	menubar->Remove( menubar->FindMenu( "Simulation" ) );

	if (cursim > -1) {
		s = GetSimulation();

		if (s == NULL)
			cursim = -1;
	}

	menubar->Append(defsimmenu, "Simulation");

	simselect->SetSelection(cursim);

	runbutton->SetBitmapLabel( playbitmap );
	runbutton->Refresh( TRUE, NULL );

	canvas->ResetSelection();
}

void BreveRender::SetMenu( int mode ) {
	menubar->Remove( menubar->FindMenu( "Simulation" ) );

	SimInstance *s = GetSimulation();

	if ( !s || mode == 0 )
		menubar->Append(defsimmenu, "Simulation");
	else
		menubar->Append(s->GetInterface()->GetMenu(), "Simulation");
}

void BreveRender::NewSimulation()
{
	char buf[2048];
	SimInstance * s;

	if (GetSimulation() != NULL)
	GetSimulation()->RegenSim();

	s = new SimInstance("", "Untitled", "");

	{
	SimInstance * n;

	s->Rechain(NULL);

	if (simlist == NULL)
		simlist = s;
	else
	{
		for (n = simlist; n->GetNext() != NULL; n = n->GetNext());

		n->Rechain(s);
	}
	}

	cursim = GetSimInt(s);

	simselect->Append("Untitled");

	if (logwindow != NULL)
	logwindow->Append("[New Simulation]\n");

	ResetSim();
}

void BreveRender::LoadSimulation(wxString simdir, wxString filename, wxString text)
{
	SimInstance * s;

	if (GetSimulation() != NULL)
	GetSimulation()->RegenSim();

	s = new SimInstance(simdir, filename, text);

	{
	SimInstance * n;

	s->Rechain(NULL);

	if (simlist == NULL)
		simlist = s;
	else
	{
		for (n = simlist; n->GetNext() != NULL; n = n->GetNext());

		n->Rechain(s);
	}
	}

	cursim = GetSimInt(s);

	{
	wxString tmp;

	if (filename.Find('.') == -1)
		tmp = filename;
	else
		tmp = filename.BeforeLast('.');

	tmp += " (" + simdir;

	tmp += FILE_SEP_PATH;

	tmp += filename + ")";

	simselect->Append(tmp);
	}

	if (logwindow != NULL)
	{
	wxString tmp;

	tmp = "[" + filename + "]\n";

	logwindow->Append(tmp);
	}

	ResetSim();
}

void BreveRender::UpdateChoice(int i)
{
	SimInstance * sim = GetSimulation(i);

	if (sim == NULL)
	return;

	{
	wxString tmp;
	wxString filename = sim->GetFilename();

	if (filename.Find('.') == -1)
		tmp = filename;
	else
		tmp = filename.BeforeLast('.');

	tmp += " (" + sim->GetDir();

	tmp += FILE_SEP_PATH;

	tmp += filename + ")";

	simselect->SetString(i, tmp);
	}

}

bool BreveRender::ShowToolTips()
{
	return TRUE;
}

wxBitmap BreveRender::GetBitmapResource( const wxString& name )
{
	return wxNullBitmap;
}

wxIcon BreveRender::GetIconResource( const wxString& name )
{
	return wxNullIcon;
}

void BreveRender::OnClose(wxCloseEvent& event)
{
	cursim = -1;
	ResetSim();

	event.Skip();
}

void BreveRender::LoadSimFile(wxString ffile)
{
	char buf[1024];
	wxString filename;
	wxString fileloc;
	wxString text;
	wxString fileext;

	wxFileName::SplitPath(ffile, &fileloc, &filename, &fileext, wxPATH_NATIVE);

	if (fileloc.IsEmpty())
	{
	char xbuf[2048];

	getcwd((char*)&xbuf, 2048);

	fileloc = xbuf;
	}

	{
	wxFile file;

	file.Open(ffile, wxFile::read);

	if (!file.IsOpened())
	{
		text << "Failed to open " << ffile << " for reading.";
		wxMessageBox(text);
		return;
	}
	else
	{
		int i;

		while (file.Tell() < file.Length())
		{
		i = 1023;

		if (file.Length() - file.Tell() < 1023)
			i = file.Length() - file.Tell();

		i = file.Read(&buf, i);

		if (i < 1)
		{
			text = "";
			text << "Reading file failed with error " << i;
			wxMessageBox(text);
			return;
		}

		buf[i] = '\0';

		text.Append(buf);
		}

		if (!fileext.IsEmpty())
		filename << '.' << fileext;

		LoadSimulation(fileloc, filename, text);
	}
	}
}

void BreveRender::OnMenuOpen(wxCommandEvent& event)
{
	wxFileDialog d(this, "Please select a simulation", "", "", "steve files (*.tz)|*.tz|Python files (*.py)|*.py", wxOPEN);

	if (d.ShowModal() == wxID_OK) {
		wxString filename = d.GetFilename();
		wxString fileloc = d.GetDirectory();

		fileloc += FILE_SEP_PATH;

		LoadSimFile(fileloc + filename);
	}
}

void BreveRender::OnMenuNew(wxCommandEvent& event) {
	NewSimulation();
}

void BreveRender::OnMenuNewFromTemplate(wxCommandEvent& event) {
	wxString str;

	str << app->GetBreveDir() << "lib" << FILE_SEP_PATH << "Template.tz";

	LoadSimFile( str );
}

void BreveRender::OnMenuLogWindow(wxCommandEvent& event)
{
	if (logwindow != NULL)
	{
	if (logwindow->IsShown())
		logwindow->Hide();
	else
		logwindow->Show(TRUE);
	}
}

void BreveRender::OnFullScreen(wxCommandEvent& event)
{
	ShowFullScreen(!IsFullScreen(), wxFULLSCREEN_ALL);

	if (IsFullScreen())
	{
	GetSizer()->Show((unsigned int)1, FALSE);
	canvas->SetFocus();
	SetMenuBar(NULL);

#ifndef __WXMSW__
	GetSizer()->Detach(canvas);
	GetSizer()->Prepend(canvas, 1, wxEXPAND|wxGROW|wxALL|wxADJUST_MINSIZE, 0);
#endif
	}
	else
	{
	GetSizer()->Show((unsigned int)1, TRUE);
	SetMenuBar(menubar);

#ifndef __WXMSW__
	GetSizer()->Detach(canvas);
	GetSizer()->Prepend(canvas, 1, wxEXPAND|wxGROW|wxALL|wxADJUST_MINSIZE, 5);
#endif
	}

	canvas->Refresh(TRUE, NULL);
}

void BreveRender::OnMenuInspector(wxCommandEvent& event)
{
	if (inspector != NULL)
	{
	if (inspector->IsShown())
		inspector->Hide();
	else
		inspector->Show(TRUE);
	}
}

void BreveRender::OnMenuSpeed(wxCommandEvent &event) {
	// wxMenuItem *item = menubar->FindItem( event.m_id );

	// if( !item ) return;

	long index = event.m_id;

	switch( index ) {
		case BREVE_FULLSPEED:
			mSleepMS = 0;
			break;
		case BREVE_MEDIUMSPEED:
			mSleepMS = 20;
			break;
		case BREVE_SLOWSPEED:
			mSleepMS = 50;
			break;

		default:
			mSleepMS = 0;
			break;
	}
}


void BreveRender::OnMenuSim(wxCommandEvent &event)
{
	wxMenuItem * item;

	item = menubar->FindItem(event.m_id);

	if (item)
	{
	wxMenuItem * mitem;
	int i = 0;

	for (i = event.m_id - 1; i >= BREVE_DEMOMENU + 1500; i--)
	{
		mitem = menubar->FindItem(i);

		if (mitem != NULL && mitem->IsSubMenu())
		break;
	}

	{
		wxString str;

		str << app->GetBreveDir() << "demos" << FILE_SEP_PATH;

		if (mitem != NULL)
		str << mitem->GetLabel() << FILE_SEP_PATH;

		str << item->GetLabel();

		gBreverender->LoadSimFile(str);
	}
	}
}

void BreveRender::OnMenuQuit(wxCommandEvent& event)
{
	Close();
}

void BreveRender::OnSimSelect(wxCommandEvent& event)
{
	int sel;

	sel = simselect->GetSelection();

	if (sel != wxNOT_FOUND && sel != cursim)
	{
	if (cursim != -1)
		OnRenderStopClick(event);

	ResetSim(sel);

	if (cursim != -1)
		OnRenderStopClick(event);

	if (logwindow != NULL)
	{
		wxString tmp;

		tmp = "[" + simselect->GetStringSelection() + "]\n";

		logwindow->Append(tmp);
	}
	}
}

void BreveRender::OnRenderRunClick( wxCommandEvent& event ) {
	SimInstance *sim = GetSimulation();

	if ( sim == NULL ) return;

	if( !sim->GetInterface()->Initialized() ) {
		sim->UpdateSimCode();

		if( !sim->GetInterface()->Initialize() ) 
			return;

		int w, h;

		SetMenu( 1 );

		canvas->GetClientSize(&w, &h);
		sim->GetInterface()->ResizeView(w, h);
	}

	sim->GetInterface()->Pause(3);

	if (sim->GetInterface()->Paused())
		runbutton->SetBitmapLabel( playbitmap );
	else
		runbutton->SetBitmapLabel( pausebitmap );

	runbutton->Refresh(TRUE, NULL);
}

void BreveRender::OnIdle( wxIdleEvent&event ) {
}


void BreveRender::OnRenderStopClick( wxCommandEvent& event )
{
	SimInstance * sim;

	sim = GetSimulation();

	if (sim == NULL)
	return;

	sim->RegenSim();

	canvas->Refresh(TRUE, NULL);
}

void BreveRender::OnDocMenu( wxCommandEvent &event ) {
	int index = event.m_id - BREVE_DOCMENU;

	wxString &docfile = _docFiles[ index ];
	wxString url = wxString( "file://" ) << docfile;

#ifdef WINDOWS
	::ShellExecute( NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL );
#else 
	// wxLaunchDefaultBrowser( docfile );
	// Oh WxWidgets.  You really do suck.
	//

	char *browser = getenv( "BROWSER" );

	if( !browser || strlen( browser ) == 0 ) {
		slMessage( DEBUG_ALL, "Warning: could not open documentation because $BROWSER variable not set\n" );
	} else {
		wxString command = wxString( "$BROWSER " ) << url; 
		system( command.c_str() );
	}

#endif
}

void BreveRender::OnSimMenu(wxCommandEvent &event) {

	if ( GetSimulation() == NULL || !GetSimulation()->GetInterface()->Initialized() ) 
		return;

	GetSimulation()->GetMutex()->Lock();

	GetSimulation()->GetInterface()->RunMenu( event.m_id - BREVE_SIMMENU, GetSimulation()->GetInterface()->GetEngine()->controller );

	GetSimulation()->GetMutex()->Unlock();
}

void BreveRender::OnRotateClick(wxCommandEvent&event)
{
	if (!rotatebutton->GetValue())
		rotatebutton->SetValue(TRUE);

	mousemode = 0;
	zoombutton->SetValue(FALSE);
	movebutton->SetValue(FALSE);
	selectbutton->SetValue(FALSE);
	event.Skip();
}

void BreveRender::OnZoomClick(wxCommandEvent&event)
{
	if (!zoombutton->GetValue())
		zoombutton->SetValue(TRUE);

	mousemode = 1;
	rotatebutton->SetValue(FALSE);
	movebutton->SetValue(FALSE);
	selectbutton->SetValue(FALSE);
	event.Skip();
}

void BreveRender::OnMoveClick(wxCommandEvent&event)
{
	if (!movebutton->GetValue())
		movebutton->SetValue(TRUE);

	mousemode = 2;
	rotatebutton->SetValue(FALSE);
	zoombutton->SetValue(FALSE);
	selectbutton->SetValue(FALSE);
	event.Skip();
}

void BreveRender::KeyUp(wxKeyEvent &event)
{
	gBreverender->OnKeyUp(event);
}

void BreveRender::OnKeyUp(wxKeyEvent&event)
{
	event.Skip();

	if (GetSimulation() != NULL && GetSimulation()->GetInterface()->Initialized()) {
		int keycode = event.GetKeyCode();

		if(isalpha(keycode) && ! event.m_shiftDown) keycode = tolower(keycode);

		GetSimulation()->GetMutex()->Lock();

		mKeysDown[ keycode ] = false;

		switch(keycode) {
			case WXK_LEFT:
				brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "left", 0);
				break;
			case WXK_RIGHT:
				brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "right", 0);
				break;
			case WXK_UP:
				brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "up", 0);
				break;
			case WXK_DOWN:
				brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "down", 0);
				break;
			default:
				brKeyCallback(GetSimulation()->GetInterface()->GetEngine(), keycode, 0);
				break;
		}

		GetSimulation()->GetMutex()->Unlock();
	}
}

void BreveRender::KeyDown(wxKeyEvent &event)
{
	gBreverender->OnKeyDown(event);
}

void BreveRender::OnKeyDown(wxKeyEvent&event)
{
	if (event.m_keyCode == WXK_ESCAPE && IsFullScreen())
	{
	OnFullScreen((*(wxCommandEvent*)NULL));
	return;
	}

	// event.Skip();

	if (GetSimulation() != NULL && GetSimulation()->GetInterface()->Initialized() && !GetSimulation()->GetInterface()->Paused()) {
		int keycode = event.GetKeyCode();

		if(isalpha(keycode) && ! event.m_shiftDown) keycode = tolower(keycode);

		if( mKeysDown[ keycode ] ) return;

		mKeysDown[ keycode ] = true;

		GetSimulation()->GetMutex()->Lock();

		switch(keycode) {
		case WXK_LEFT:
			brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "left", 1);
			break;
		case WXK_RIGHT:
			brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "right", 1);
			break;
		case WXK_UP:
			brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "up", 1);
			break;
		case WXK_DOWN:
			brSpecialKeyCallback(GetSimulation()->GetInterface()->GetEngine(), "down", 1);
			break;
		default:
			brKeyCallback(GetSimulation()->GetInterface()->GetEngine(), keycode, 1);
			break;
		}

		GetSimulation()->GetMutex()->Unlock();
	}
}

void BreveRender::OnSelectClick(wxCommandEvent&event)
{
	if (!selectbutton->GetValue())
	selectbutton->SetValue(TRUE);

	mousemode = 3;
	rotatebutton->SetValue(FALSE);
	zoombutton->SetValue(FALSE);
	movebutton->SetValue(FALSE);
	event.Skip();
}

void BreveRender::OnSize(wxSizeEvent &event)
{
	if (!IsMaximized() && !IsFullScreen()) {
		wxConfigBase * config = wxConfigBase::Get();

		if (config != NULL) {
			config->Write("BreveRenderWidth", event.GetSize().GetWidth());
			config->Write("BreveRenderHeight", event.GetSize().GetHeight());
		}
	}

	event.Skip();
}

void BreveRender::OnMove(wxMoveEvent &event)
{
	{
		wxConfigBase * config = wxConfigBase::Get();

		if (config != NULL) {
			wxPoint position = GetPosition();
			config->Write("BreveRenderX", position.x);
			config->Write("BreveRenderY", position.y);
		}
	}

	event.Skip();
}

