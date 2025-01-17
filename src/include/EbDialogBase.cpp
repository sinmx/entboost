// EbDialogBase.cpp : implementation file
//

#include "stdafx.h"
//#include "EBUIApp.h"
#include "resource.h"
#include "EbDialogBase.h"
#include "colorconver.h"

#define CORNER_SIZE 2
#define IDC_STA_SPLITTER_LEFT	0x15001
#define IDC_STA_SPLITTER_TOP	0x15002
#define IDC_STA_SPLITTER_RIGHT	0x15003
#define IDC_STA_SPLITTER_BOTTOM 0x15004


// CEbDialogBase dialog

IMPLEMENT_DYNAMIC(CEbDialogBase, CDialog)

CEbDialogBase::CEbDialogBase(CWnd* pParent /*=NULL*/)
	//: CDialog(pParent)
	: m_bCircle(true)
{
	init();
}
CEbDialogBase::CEbDialogBase(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
	, m_bCircle(true)
{
	init();
}

CEbDialogBase::~CEbDialogBase()
{
}

void CEbDialogBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEbDialogBase, CDialog)
	//ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(POP_WM_WINDOW_SIZE, OnMessageWindowResize)
	ON_WM_MOUSEMOVE()
	ON_WM_MOVING()
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnTipNotify)
END_MESSAGE_MAP()


// CEbDialogBase message handlers
void CEbDialogBase::GradientFillRect( CDC *pDC, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad )
{
	TRIVERTEX        vert[2];
	GRADIENT_RECT    mesh;

	vert[0].x      = rect.left;
	vert[0].y      = rect.top;
	vert[0].Alpha  = 0x0000;
	vert[0].Blue   = GetBValue(col_from) << 8;
	vert[0].Green  = GetGValue(col_from) << 8;
	vert[0].Red    = GetRValue(col_from) << 8;

	vert[1].x      = rect.right;
	vert[1].y      = rect.bottom; 
	vert[1].Alpha  = 0x0000;
	vert[1].Blue   = GetBValue(col_to) << 8;
	vert[1].Green  = GetGValue(col_to) << 8;
	vert[1].Red    = GetRValue(col_to) << 8;

	mesh.UpperLeft  = 0;
	mesh.LowerRight = 1;
#if _MSC_VER >= 1300  // only VS7 and above has GradientFill as a pDC member
	pDC->GradientFill( vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
		GRADIENT_FILL_RECT_H );
#else
	GradientFill( pDC->m_hDC, vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
		GRADIENT_FILL_RECT_H );
#endif
}
void CEbDialogBase::init()
{
//#ifdef EBUI_EXPORTS
//	m_hIcon = NULL;
//#else
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//#endif
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nTransType = 0;
	m_bMouseMove = TRUE;
	m_bDisabelMoveBottom = FALSE;
	m_nLimitWidth = 220;
	m_nSplitterPressed = 0;
}

BOOL CEbDialogBase::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
LRESULT CEbDialogBase::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STA_SPLITTER_LEFT)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				m_nSplitterPressed = 1;
				CSplitterControl::ChangeWidth(this, -pHdr->delta, CW_RIGHTALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
				m_nSplitterPressed = 0;
			}
		}else if (wParam == IDC_STA_SPLITTER_TOP)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				m_nSplitterPressed = 3;
				CSplitterControl::ChangeHeight(this, -pHdr->delta, CW_BOTTOMALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
				m_nSplitterPressed = 0;
			}
		}else if (wParam == IDC_STA_SPLITTER_RIGHT)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				m_nSplitterPressed = 2;
				CSplitterControl::ChangeWidth(this, pHdr->delta, CW_LEFTALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
				m_nSplitterPressed = 0;
			}
		}else if (wParam == IDC_STA_SPLITTER_BOTTOM)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				m_nSplitterPressed = 4;
				CSplitterControl::ChangeHeight(this, pHdr->delta, CW_TOPALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
				m_nSplitterPressed = 0;
			}
		}
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}
LRESULT CEbDialogBase::OnMessageWindowResize(WPARAM wParam, LPARAM lParam)
{
	SetCircle(m_bCircle);
	this->Invalidate();
	return 0;
}
void CEbDialogBase::SetBgColor(COLORREF colorBg)
{
	m_brush.DeleteObject();
	m_brush.CreateSolidBrush(colorBg);
}

void CEbDialogBase::SetCircle(bool bCircle)
{
	m_bCircle = bCircle;
	if (m_bCircle)
	{
		CRect rectClient;
		this->GetWindowRect(&rectClient);
		int nx = 1;
		int ny = 1;
		int dx = 4;	// 4
		int dy = 4;	// 4
		CRgn m_rgn;
		m_rgn.CreateRoundRectRgn(0, 0, rectClient.Width()+nx, rectClient.Height()+ny, dx, dy);//这里如果ny> =dy就木有下面的圆角啦 
		::SetWindowRgn(GetSafeHwnd(),   (HRGN)m_rgn.Detach(), TRUE);
	}
}

void CEbDialogBase::SetSplitterBorder(int nLimitWidth)
{
	//return;
	m_nLimitWidth = nLimitWidth;
	if (m_wndSplitterLeft.GetSafeHwnd() != NULL) return;
	m_wndSplitterLeft.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_LEFT);
	m_wndSplitterLeft.SetStyle(SPS_VERTICAL);
	m_wndSplitterLeft.SetRange(-200, 800);
	m_wndSplitterLeft.SetTransparent(true);
	m_wndSplitterTop.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_TOP);
	m_wndSplitterTop.SetStyle(SPS_HORIZONTAL);
	m_wndSplitterTop.SetRange(-200, 500);	// 800
	m_wndSplitterTop.SetTransparent(true);
	m_wndSplitterRight.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_RIGHT);
	m_wndSplitterRight.SetStyle(SPS_VERTICAL);
	m_wndSplitterRight.SetRange(200, 800);
	m_wndSplitterRight.SetTransparent(true);
	m_wndSplitterBottom.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_BOTTOM);
	m_wndSplitterBottom.SetStyle(SPS_HORIZONTAL);
	m_wndSplitterBottom.SetRange(200, GetScreenHeight());
	m_wndSplitterBottom.SetTransparent(true);

}
void CEbDialogBase::SetDlgChildFont(int nFontSize, LPCTSTR lpszFace,bool bSetDlgFont)
{
	if (m_childFont.GetSafeHandle()==NULL)
		m_childFont.CreatePointFont(nFontSize,lpszFace);
	EnumChildWindows(this->GetSafeHwnd(), SetChildFont, (LPARAM)m_childFont.GetSafeHandle());
	if (bSetDlgFont)
		this->SetFont(&m_childFont);
}


//void CEbDialogBase::OnPaint()
//{
//	if (!IsIconic())
//	{
//
//		CDialog::OnPaint();
//	}
//}

//
//void CEbDialogBase::OnPaint()
//{
//	if (!IsIconic())
//	{
//		//CPaintDC dc2(this);
//		//DrawPopBg(&dc2, m_colorDefault);
//
//		//CRect rect;
//		//GetClientRect(rect);
//
//		//// 填充背景色
//		////dc2.FillSolidRect(rect, m_colorDefault);
//		//// 渐变效果
//		//float h,s,l;
//		//::RGBtoHSL(GetRValue(m_colorDefault), GetGValue(m_colorDefault), GetBValue(m_colorDefault),&h,&s,&l);
//		//COLORREF m_gradient_bkgd_to = ::HSLtoRGB(h, s, 0.90);
//		//GradientFillRect( &dc2, rect, m_colorDefault, m_gradient_bkgd_to, TRUE );
//
//		////外边框
//		//CPen *oldpen = NULL;
//		////CPen newpen(PS_SOLID, 1, RGB(34,177,76));		// 外边框颜色 绿色
//		//CPen newpen(PS_SOLID, 1, RGB(87,87,87));	// 外边框颜色 浅黑色
//		////CPen newpen(PS_SOLID, 1, RGB(27,147,186));	// 外边框颜色 蓝色
//		////CPen newpen(PS_SOLID, 1, m_colorDefault);
//		//oldpen = dc2.SelectObject(&newpen);
//
//		//dc2.MoveTo(rect.left, CORNER_SIZE);
//		//dc2.LineTo(CORNER_SIZE, rect.top);
//		//dc2.LineTo(rect.right-CORNER_SIZE-1, rect.top);
//		//dc2.LineTo(rect.right-1, CORNER_SIZE);
//		//dc2.LineTo(rect.right-1, rect.bottom-CORNER_SIZE-1);
//		//dc2.LineTo(rect.right-CORNER_SIZE-1, rect.bottom-1);
//		//dc2.LineTo(CORNER_SIZE, rect.bottom-1);
//		//dc2.LineTo(rect.left, rect.bottom-CORNER_SIZE-1);
//		//dc2.LineTo(rect.left, CORNER_SIZE);
//
//		////填充空缺处
//		//dc2.MoveTo(rect.left+1, CORNER_SIZE);
//		//dc2.LineTo(CORNER_SIZE+1, rect.top);
//
//		//dc2.MoveTo(rect.right-CORNER_SIZE-1, rect.top+1);
//		//dc2.LineTo(rect.right-1, CORNER_SIZE+1);
//
//		//dc2.MoveTo(rect.right-2, rect.bottom-CORNER_SIZE-1);
//		//dc2.LineTo(rect.right-CORNER_SIZE-1, rect.bottom-1);
//
//		//dc2.MoveTo(CORNER_SIZE, rect.bottom-2);
//		//dc2.LineTo(rect.left, rect.bottom-CORNER_SIZE-2);
//
//		//dc2.SelectObject(oldpen);
//
//		////内边框
//		//CPen newpen2(PS_SOLID, 1, RGB(196,234,247));
//		//oldpen = dc2.SelectObject(&newpen2);
//
//		//dc2.MoveTo(rect.left+1, CORNER_SIZE+1);
//		//dc2.LineTo(CORNER_SIZE+1, rect.top+1);
//		//dc2.LineTo(rect.right-CORNER_SIZE-2, rect.top+1);
//		//dc2.LineTo(rect.right-2, CORNER_SIZE+1);
//		//dc2.LineTo(rect.right-2, rect.bottom-CORNER_SIZE-2);
//		//dc2.LineTo(rect.right-CORNER_SIZE-2, rect.bottom-2);
//		//dc2.LineTo(CORNER_SIZE+1, rect.bottom-2);
//		//dc2.LineTo(rect.left+1, rect.bottom-CORNER_SIZE-2);
//		//dc2.LineTo(rect.left+1, CORNER_SIZE+1);
//
//		CDialog::OnPaint();
//	}
//}
void CEbDialogBase::DrawPopBg(CDC * pDC, COLORREF colorBg, int nBorder, int nOffsetValue, int nOffsetType,COLORREF colorBorder)
{
	ASSERT (pDC != NULL);
	if (!IsIconic())
	{
		CRect rect;
		GetClientRect(rect);

		//// 填充背景色
		//pDC->FillSolidRect(rect, colorBg);
		//// 渐变效果
		//float h,s,l;
		//::RGBtoHSL(GetRValue(colorBg), GetGValue(colorBg), GetBValue(colorBg),&h,&s,&l);
		////COLORREF m_gradient_bkgd_to = ::HSLtoRGB(h, s, 0.92);
		////GradientFillRect( pDC, rect, colorBg, m_gradient_bkgd_to, TRUE );
		//COLORREF color1 = ::HSLtoRGB(h, s, 0.5);	// 外框 RGB(87,87,87)
		//COLORREF color2 = ::HSLtoRGB(h, s, 0.9);	// 内框 RGB(196,234,247)
		float h,s,l;
		//::RGBtoHSL(GetRValue(colorBg), GetGValue(colorBg), GetBValue(colorBg),&h,&s,&l);
		const COLORREF color1 = colorBorder==-1?theDefaultFlatLine2Color:colorBorder;	// 外框
		//const COLORREF color1 = RGB(72,88,103);			// 外框 RGB(87,87,87)
		::RGBtoHSL(GetRValue(color1), GetGValue(color1), GetBValue(color1),&h,&s,&l);
		const COLORREF color2 = ::HSLtoRGB(h, s, 0.9);	// 内框 RGB(196,234,247)
		if (m_pBgDrawInfo.empty())
		{
			pDC->FillSolidRect(rect, colorBg);
		}else
		{
			int x = 0;
			int y = 0;
			COLORREF crGradientFrom = colorBg;
			for (size_t i=0; i<m_pBgDrawInfo.size(); i++)
			{
				const CEbBackDrawInfo pDrawInfo = m_pBgDrawInfo[i];
				const COLORREF m_gradient_bkgd_to = pDrawInfo.m_nColor!=-1?pDrawInfo.m_nColor:(pDrawInfo.m_fLight>=1.0?colorBg:HSLtoRGB(h, s, pDrawInfo.m_fLight));
				if (pDrawInfo.m_bLeft2Rigth)
				{
					if (pDrawInfo.m_nWidth==0)
					{
						CRect rect1(rect);
						rect1.top = y;
						rect1.left = x;
						pDC->FillSolidRect(rect1, m_gradient_bkgd_to);
						//break;
					}else
					{
						CRect rect1(rect);
						rect1.top = y;
						rect1.left = x;
						rect1.right = rect1.left + pDrawInfo.m_nWidth;
						x += pDrawInfo.m_nWidth;
						if (pDrawInfo.m_bGradient)
							GradientFillRect( pDC, rect1, crGradientFrom, m_gradient_bkgd_to, TRUE );
						else
							pDC->FillSolidRect(rect1, m_gradient_bkgd_to);
						crGradientFrom = m_gradient_bkgd_to;
					}
					continue;
				}
				if (pDrawInfo.m_nHeight == 0)
				{
					CRect rect1(rect);
					rect1.left = x;
					rect1.top = y;
					pDC->FillSolidRect(rect1, m_gradient_bkgd_to);
					//break;
				}else if (pDrawInfo.m_nHeight < 0)
				{
					CRect rect1(rect);
					rect1.left = x;
					rect1.top = rect.bottom+pDrawInfo.m_nHeight;
					y = rect1.top;
					if (pDrawInfo.m_bGradient)
						GradientFillRect( pDC, rect1, crGradientFrom, m_gradient_bkgd_to, TRUE );
					else
						pDC->FillSolidRect(rect1, m_gradient_bkgd_to);
					crGradientFrom = m_gradient_bkgd_to;
				}else
				{
					CRect rect1(rect);
					rect1.left = x;
					rect1.top = y;
					rect1.bottom = rect1.top + pDrawInfo.m_nHeight;
					y += pDrawInfo.m_nHeight;
					if (pDrawInfo.m_bGradient)
						GradientFillRect( pDC, rect1, crGradientFrom, m_gradient_bkgd_to, TRUE );
					else
						pDC->FillSolidRect(rect1, m_gradient_bkgd_to);
					crGradientFrom = m_gradient_bkgd_to;
				}
/*
				const int const_height = 250;
				if (rect.Height() <= const_height)
				{
					GradientFillRect( pDC, rect, colorBg, m_gradient_bkgd_to, TRUE );
				}else
				{
					CRect rect1(rect);
					rect1.bottom = rect1.top + const_height;
					CRect rect2(rect);
					rect2.top = rect1.bottom;
					GradientFillRect( pDC, rect1, colorBg, m_gradient_bkgd_to, TRUE );
					pDC->FillSolidRect(rect2, m_gradient_bkgd_to);
				}*/
			}
		}

		//外边框
		CPen *oldpen = NULL;
		if (nBorder > 0)
		{
			CPen newpen(PS_SOLID, 1, color1);//RGB(87,87,87));	// 外边框颜色 浅黑色
			oldpen = pDC->SelectObject(&newpen);
			if (!m_bCircle)
			{
				pDC->MoveTo(0, rect.top);
				pDC->LineTo(rect.right-1, rect.top);
				pDC->LineTo(rect.right-1, rect.bottom-1);
				pDC->LineTo(0, rect.bottom-1);
				pDC->LineTo(0, rect.top);
				pDC->SelectObject(oldpen);
				return;
			}

			const int nLeftOffset = (nOffsetValue>0&&nOffsetType==1)?nOffsetValue:0;
			const int nTopOffset = (nOffsetValue>0&&nOffsetType==2)?nOffsetValue:0;

			if (nLeftOffset>0)
			{
				pDC->MoveTo(nLeftOffset, rect.top);
				pDC->LineTo(rect.right-CORNER_SIZE-1, rect.top);
				pDC->LineTo(rect.right-1, CORNER_SIZE);
			}else if (nTopOffset>0)
			{
				pDC->MoveTo(rect.right-1, nTopOffset);
			}else
			{
				pDC->MoveTo(rect.left, CORNER_SIZE);
				pDC->LineTo(CORNER_SIZE, rect.top);
				pDC->LineTo(rect.right-CORNER_SIZE-1, rect.top);
				pDC->LineTo(rect.right-1, CORNER_SIZE);
			}
			pDC->LineTo(rect.right-1, rect.bottom-CORNER_SIZE-1);
			pDC->LineTo(rect.right-CORNER_SIZE-1, rect.bottom-1);
			if (nLeftOffset>0)
			{
				pDC->LineTo(nLeftOffset, rect.bottom-1);
				pDC->LineTo(nLeftOffset+1, rect.bottom-1);	// *
			}else if (nTopOffset>0)
			{
				pDC->LineTo(CORNER_SIZE, rect.bottom-1);
				pDC->LineTo(rect.left, rect.bottom-CORNER_SIZE-1);
				pDC->LineTo(rect.left, nTopOffset);
				pDC->LineTo(rect.left, nTopOffset+1);	// *
			}else
			{
				pDC->LineTo(CORNER_SIZE, rect.bottom-1);
				pDC->LineTo(rect.left, rect.bottom-CORNER_SIZE-1);
				pDC->LineTo(rect.left, CORNER_SIZE);
			}

			////填充空缺处
			//if (nTopOffset==0)
			//{
			//	pDC->MoveTo(rect.left+1, CORNER_SIZE);
			//	pDC->LineTo(CORNER_SIZE+1, rect.top);

			//	pDC->MoveTo(rect.right-CORNER_SIZE-1, rect.top+1);
			//	pDC->LineTo(rect.right-1, CORNER_SIZE+1);
			//}

			//pDC->MoveTo(rect.right-2, rect.bottom-CORNER_SIZE-1);
			//pDC->LineTo(rect.right-CORNER_SIZE-1, rect.bottom-1);

			//pDC->MoveTo(CORNER_SIZE, rect.bottom-2);
			//pDC->LineTo(rect.left, rect.bottom-CORNER_SIZE-2);

			pDC->SelectObject(oldpen);
		}

		if (nBorder>1)
		{
			//内边框
			CPen newpen2(PS_SOLID, 1, color2);//RGB(196,234,247));
			oldpen = pDC->SelectObject(&newpen2);

			pDC->MoveTo(rect.left+1, CORNER_SIZE+1);
			pDC->LineTo(CORNER_SIZE+1, rect.top+1);
			pDC->LineTo(rect.right-CORNER_SIZE-2, rect.top+1);
			pDC->LineTo(rect.right-2, CORNER_SIZE+1);
			pDC->LineTo(rect.right-2, rect.bottom-CORNER_SIZE-2);
			pDC->LineTo(rect.right-CORNER_SIZE-2, rect.bottom-2);
			pDC->LineTo(CORNER_SIZE+1, rect.bottom-2);
			pDC->LineTo(rect.left+1, rect.bottom-CORNER_SIZE-2);
			pDC->LineTo(rect.left+1, CORNER_SIZE+1);
			pDC->SelectObject(oldpen);
		}
	}
}

HBRUSH CEbDialogBase::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	//if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_DLG)
	if(((m_nTransType & TT_STATIC) == TT_STATIC && nCtlColor == CTLCOLOR_STATIC) ||
		((m_nTransType & TT_MSGBOX) == TT_MSGBOX && nCtlColor == CTLCOLOR_MSGBOX) || 
		((m_nTransType & TT_BTN) == TT_BTN && nCtlColor == CTLCOLOR_BTN) || 
		((m_nTransType & TT_EDIT) == TT_EDIT && nCtlColor == CTLCOLOR_EDIT) || 
		((m_nTransType & TT_DLG) == TT_DLG && nCtlColor == CTLCOLOR_DLG)
		)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	if (m_brush.GetSafeHandle() != NULL)
		return (HBRUSH)m_brush.m_hObject;
	// TODO:  Return a different brush if the default is not desired
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CEbDialogBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
    if (m_bMouseMove)
        PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	

	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CEbDialogBase::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CEbDialogBase::OnDestroy()
{
	CDialog::OnDestroy();
	if (m_childFont.GetSafeHandle()!=NULL)
		m_childFont.DeleteObject();
	m_wndSplitterRight.DestroyWindow();
}

void CEbDialogBase::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	static int const_max_len = GetSystemMetrics(SM_CXSCREEN);
	static int const_max_height = GetScreenHeight();

	//const int const_max_len = 1300;
	const int const_min_len = m_nLimitWidth;//120;
	const int const_border_width = 2;
	if (m_wndSplitterLeft.GetSafeHwnd())
	{
		m_wndSplitterLeft.MoveWindow(0,0,const_border_width,cy);
		m_wndSplitterLeft.SetRange(-const_max_len, cx-const_min_len);
	}
	if (m_wndSplitterTop.GetSafeHwnd())
	{
		m_wndSplitterTop.MoveWindow(0,0,cx,const_border_width);
		//m_wndSplitterTop.SetRange(-const_max_len, cy-const_min_len);
	}
	if (m_wndSplitterRight.GetSafeHwnd())
	{
		m_wndSplitterRight.MoveWindow(cx-const_border_width,0,const_border_width,cy);
		m_wndSplitterRight.SetRange(const_min_len, const_max_len);
	}
	if (m_wndSplitterBottom.GetSafeHwnd())
	{
		m_wndSplitterBottom.MoveWindow(0,cy-const_border_width,cx,const_border_width);
		//m_wndSplitterBottom.SetRange(const_min_len, const_max_len);
	}

	this->PostMessage(POP_WM_WINDOW_SIZE);
}

void CEbDialogBase::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
	//if (nID==SC_MAXIMIZE || nID==SC_RESTORE)
	//{
	//	this->PostMessage(POP_WM_WINDOW_SIZE);
	//}
}
void CEbDialogBase::SetToolTipText(UINT nID,LPCTSTR sToolTip)
{
	m_pToolTip[nID]=sToolTip;
}
//void CEbDialogBase::SetToolTipColor(COLORREF crText,COLORREF crBkgnd)
//{
//    if(m_tooltip.m_hWnd != NULL)
//    {
//		m_tooltip.SetTipTextColor(crText);
//        m_tooltip.SetTipBkColor(crBkgnd);
//    }
//}

int CEbDialogBase::GetSplitterPressed(void) const
{
	if (m_nSplitterPressed>0)
		return m_nSplitterPressed;
	if (m_wndSplitterLeft.IsPressed())
		return 1;
	if (m_wndSplitterRight.IsPressed())
		return 2;
	if (m_wndSplitterTop.IsPressed())
		return 3;
	if (m_wndSplitterBottom.IsPressed())
		return 4;
	return 0;
}

void CEbDialogBase::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialog::OnMouseMove(nFlags, point);
}

BOOL CEbDialogBase::OnTipNotify(UINT id,NMHDR*pNMHDR,LRESULT*pResult)
{
	TOOLTIPTEXT*pTTT=(TOOLTIPTEXT*)pNMHDR;
	UINT uID=pNMHDR->idFrom;
	if(pTTT->uFlags&TTF_IDISHWND)
		uID=::GetDlgCtrlID((HWND)uID);
	if(uID==NULL) return FALSE;
	CString sToolTip=m_pToolTip[uID];
	if (sToolTip.IsEmpty()) return FALSE;
	pTTT->lpszText=(LPTSTR)(LPCTSTR)sToolTip;
	return TRUE;
}
int CEbDialogBase::GetScreenWidth(void)
{
	return GetSystemMetrics(SM_CXSCREEN); //屏幕宽度
}
int CEbDialogBase::GetScreenHeight(void)
{
	int pHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	pHeight += GetSystemMetrics(SM_CYCAPTION);
	return pHeight;
}

void CEbDialogBase::OnMoving(UINT fwSide, LPRECT pRect)
{
	if (pRect!=NULL)
	{
		const int nScreenWidth = GetScreenWidth();
		const int nScreenHeight = GetScreenHeight();
		if ((pRect->right-pRect->left)==nScreenWidth && (pRect->bottom-pRect->top)==nScreenHeight)
		{
			// * 最大化，限制移动
			pRect->left = 0;
			pRect->right = nScreenWidth;
			pRect->top = 0;
			pRect->bottom = nScreenHeight;
		}else if (m_bDisabelMoveBottom)
		{
			CPoint pCursorPos;
			GetCursorPos(&pCursorPos);
			if (pCursorPos.y>= nScreenHeight)
			{
				// 限制下移
				pRect->top -= (pCursorPos.y-nScreenHeight);
				pRect->bottom -= (pCursorPos.y-nScreenHeight);
				SetCursorPos(pCursorPos.x,nScreenHeight);
			}
		}
		//if (pRect->left<0 && pRect->top>0)
		//{
		//	// 限制左移
		//	pRect->right -= pRect->left;
		//	pRect->left = 0;
		//}else 
		//{
		//	int nScreenWidth = 0;
		//	int nScreenHeight = 0;
		//	theApp.GetScreenSize(nScreenWidth, nScreenHeight);
		//	if (pRect->left>0 && pRect->right>nScreenWidth)
		//	{
		//		// 限制右移
		//		pRect->left -= (pRect->right-nScreenWidth);
		//		pRect->right = nScreenWidth;
		//	}
		//}
	}
	CDialog::OnMoving(fwSide, pRect);

	// TODO: Add your message handler code here
}

//void CEbDialogBase::OnPaint()
//{
//	//CPaintDC dc(this); // device context for painting
//	// TODO: Add your message handler code here
//	// Do not call CDialog::OnPaint() for painting messages
//
//	if (IsIconic())
//	{
//		CPaintDC dc(this); // 用于绘制的设备上下文
//
//		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
//		// 使图标在工作区矩形中居中
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
//		CRect rect;
//		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
//		int y = (rect.Height() - cyIcon + 1) / 2;
//
//		// 绘制图标
//		dc.DrawIcon(x, y, m_hIcon);
//	}
//	else
//	{
//		//CEbDialogBase::OnPaint();
//		//DrawInfo();
//		OnDraw();
//	}
//
//}
