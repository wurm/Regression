////////////////////////////////////////////////////////////////////////////
// This file is automatically generated by VC++ 6.0 MFC Wizard
////////////////////////////////////////////////////////////////////////////

// mfcdemoDoc.cpp : implementation of the CMfcdemoDoc class
//

#include "stdafx.h"
#include "mfcdemo.h"

#include "mfcdemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfcdemoDoc

IMPLEMENT_DYNCREATE(CMfcdemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CMfcdemoDoc, CDocument)
	//{{AFX_MSG_MAP(CMfcdemoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcdemoDoc construction/destruction

CMfcdemoDoc::CMfcdemoDoc()
{
	// TODO: add one-time construction code here
}

CMfcdemoDoc::~CMfcdemoDoc()
{
}

BOOL CMfcdemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMfcdemoDoc serialization

void CMfcdemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMfcdemoDoc diagnostics

#ifdef _DEBUG
void CMfcdemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMfcdemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcdemoDoc commands