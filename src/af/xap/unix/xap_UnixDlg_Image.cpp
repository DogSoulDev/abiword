/* AbiWord
 * Copyright (C) 2001 Dom Lachowicz
 * Copyright (C) 2002 Martin Sevior
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */

#include <stdlib.h>
#include <time.h>
#include <glade/glade.h>

#include "ut_string.h"
#include "ut_assert.h"
#include "ut_debugmsg.h"

#include "xap_UnixDialogHelper.h"

#include "xap_App.h"
#include "xap_UnixApp.h"
#include "xap_Frame.h"

#include "xap_Strings.h"
#include "xap_Dialog_Id.h"
#include "xap_Dlg_Image.h"
#include "xap_UnixDlg_Image.h"

/*****************************************************************/

void XAP_UnixDialog_Image::s_HeightSpin_changed(GtkWidget * widget, XAP_UnixDialog_Image *dlg)
{
	UT_return_if_fail(widget && dlg);
	dlg->doHeightSpin();
}

void XAP_UnixDialog_Image::s_WidthSpin_changed(GtkWidget * widget, XAP_UnixDialog_Image *dlg)
{
	UT_return_if_fail(widget && dlg);
	dlg->doWidthSpin();
}

void XAP_UnixDialog_Image::s_HeightEntry_changed(GtkWidget * widget, XAP_UnixDialog_Image *dlg)
{
	UT_return_if_fail(widget && dlg);
	dlg->doHeightEntry();
}

void XAP_UnixDialog_Image::s_WidthEntry_changed(GtkWidget * widget, XAP_UnixDialog_Image *dlg)
{
	UT_return_if_fail(widget && dlg);
	dlg->doWidthEntry();
}

void XAP_UnixDialog_Image::s_aspect_clicked(GtkWidget * widget, XAP_UnixDialog_Image * dlg)
{
	UT_return_if_fail(widget && dlg);
	dlg->aspectCheckbox();
}

void XAP_UnixDialog_Image::s_wrapping_changed(GtkWidget * widget, XAP_UnixDialog_Image * dlg)
{
	UT_return_if_fail(widget && dlg);
	dlg->wrappingChanged();
}

void XAP_UnixDialog_Image::wrappingChanged(void)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbInLine)))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbPlaceParagraph),TRUE);
		gtk_widget_set_sensitive(m_wPlaceTable,FALSE);
		gtk_widget_set_sensitive(m_wrbPlaceParagraph,FALSE);
		gtk_widget_set_sensitive(m_wrbPlaceColumn,FALSE);
		gtk_widget_set_sensitive(m_wrbPlacePage,FALSE);

		return;
	}
	gtk_widget_set_sensitive(m_wPlaceTable,TRUE);
	gtk_widget_set_sensitive(m_wrbPlaceParagraph,TRUE);
	gtk_widget_set_sensitive(m_wrbPlaceColumn,TRUE);
	gtk_widget_set_sensitive(m_wrbPlacePage,TRUE);
}

void XAP_UnixDialog_Image::event_Ok ()
{
	setAnswer(XAP_Dialog_Image::a_OK);
	setTitle (gtk_entry_get_text (GTK_ENTRY(m_wTitleEntry)));
	setAlt (gtk_entry_get_text (GTK_ENTRY(m_wAltEntry)));
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbInLine)))
	{
		setWrapping(WRAP_INLINE);
	}
	else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbWrappedRight)))
	{
		setWrapping(WRAP_TEXTRIGHT);
	}
	else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbWrappedLeft)))
	{
		setWrapping(WRAP_TEXTLEFT);
	}
	else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbWrappedBoth)))
	{
		setWrapping(WRAP_TEXTBOTH);
	}

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbPlaceParagraph)))
	{
		setPositionTo(POSITION_TO_PARAGRAPH);
	}
	else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbPlaceColumn)))
	{
		setPositionTo(POSITION_TO_COLUMN);
	}
	else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_wrbPlacePage)))
	{
		setPositionTo(POSITION_TO_PAGE);
	}
}
void XAP_UnixDialog_Image::event_Cancel ()
{  
	setAnswer(XAP_Dialog_Image::a_Cancel);
}

void XAP_UnixDialog_Image::aspectCheckbox()
{
	if(GTK_TOGGLE_BUTTON( m_wAspectCheck)->active && (m_dHeightWidth > 0.0001))
		m_bAspect = true;
	else
		m_bAspect = false;
	setPreserveAspect( m_bAspect );
}

void XAP_UnixDialog_Image::doHeightSpin(void)
{
	bool bIncrement = true;
	UT_sint32 val = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(m_wHeightSpin));
	if (val == m_iHeight)
		return;
	if(val < m_iHeight)
		bIncrement = false;

	m_iHeight = val;
	incrementHeight(bIncrement);
	adjustWidthForAspect();
	gtk_entry_set_text( GTK_ENTRY(m_wHeightEntry),getHeightString() );
}


void XAP_UnixDialog_Image::doWidthSpin(void)
{
	bool bIncrement = true;
	UT_sint32 val = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(m_wWidthSpin));
	if (val == m_iWidth)
		return;	
	if(val < m_iWidth)
		bIncrement = false;
	m_iWidth = val;
	incrementWidth(bIncrement);
	adjustHeightForAspect();
	gtk_entry_set_text( GTK_ENTRY(m_wWidthEntry),getWidthString() );
}

void XAP_UnixDialog_Image::doHeightEntry(void)
{
	const char * szHeight = gtk_entry_get_text(GTK_ENTRY(m_wHeightEntry));
	if(UT_determineDimension(szHeight,DIM_none) != DIM_none)
	{
		setHeight(szHeight);

		g_signal_handler_block(G_OBJECT(m_wHeightEntry), m_iHeightID);
		int pos = gtk_editable_get_position(GTK_EDITABLE(m_wHeightEntry));
		gtk_entry_set_text( GTK_ENTRY(m_wHeightEntry),getHeightString() );
		gtk_editable_set_position(GTK_EDITABLE(m_wHeightEntry), pos);
		g_signal_handler_unblock(G_OBJECT(m_wHeightEntry), m_iHeightID);
	}
	adjustWidthForAspect();
}

void XAP_UnixDialog_Image::setHeightEntry(void)
{
	g_signal_handler_block(G_OBJECT(m_wHeightEntry), m_iHeightID);
	int pos = gtk_editable_get_position(GTK_EDITABLE(m_wHeightEntry));
	gtk_entry_set_text( GTK_ENTRY(m_wHeightEntry),getHeightString() );
	gtk_editable_set_position(GTK_EDITABLE(m_wHeightEntry), pos);
	g_signal_handler_unblock(G_OBJECT(m_wHeightEntry), m_iHeightID);
}

void XAP_UnixDialog_Image::setWidthEntry(void)
{
	g_signal_handler_block(G_OBJECT(m_wWidthEntry), m_iWidthID);
	int pos = gtk_editable_get_position(GTK_EDITABLE(m_wWidthEntry));
	gtk_entry_set_text( GTK_ENTRY(m_wWidthEntry),getWidthString() );
	gtk_editable_set_position(GTK_EDITABLE(m_wWidthEntry), pos);
	g_signal_handler_unblock(G_OBJECT(m_wWidthEntry), m_iWidthID);
}

void XAP_UnixDialog_Image::doWidthEntry(void)
{
	const char * szWidth = gtk_entry_get_text(GTK_ENTRY(m_wWidthEntry));
	if(UT_determineDimension(szWidth,DIM_none) != DIM_none)
	{
		setWidth(szWidth);
		
		g_signal_handler_block(G_OBJECT(m_wWidthEntry), m_iWidthID);
		int pos = gtk_editable_get_position(GTK_EDITABLE(m_wWidthEntry));
		gtk_entry_set_text( GTK_ENTRY(m_wWidthEntry),getWidthString() );
		gtk_editable_set_position(GTK_EDITABLE(m_wWidthEntry), pos);
		g_signal_handler_unblock(G_OBJECT(m_wWidthEntry), m_iWidthID);
	}
	adjustHeightForAspect();
}


void XAP_UnixDialog_Image::adjustHeightForAspect(void)
{
	if(m_bAspect)
		setHeightEntry();
}

void XAP_UnixDialog_Image::adjustWidthForAspect(void)
{
	if(m_bAspect)
		setWidthEntry();
}

/***********************************************************************/

XAP_Dialog * XAP_UnixDialog_Image::static_constructor(XAP_DialogFactory * pFactory,
													  XAP_Dialog_Id id)
{
	return new XAP_UnixDialog_Image(pFactory,id);
}

XAP_UnixDialog_Image::XAP_UnixDialog_Image(XAP_DialogFactory * pDlgFactory,
										   XAP_Dialog_Id id)
	: XAP_Dialog_Image(pDlgFactory,id)
{
}

XAP_UnixDialog_Image::~XAP_UnixDialog_Image(void)
{
}

void XAP_UnixDialog_Image::setWrappingGUI()
{
	if(getWrapping() == WRAP_INLINE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbInLine),TRUE);
	}
	else if(getWrapping() == WRAP_TEXTRIGHT)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbWrappedRight),TRUE);
	}
	else if(getWrapping() == WRAP_TEXTLEFT)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbWrappedLeft),TRUE);
	}
	else if(getWrapping() == WRAP_TEXTBOTH)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbWrappedBoth),TRUE);
	}
}


void XAP_UnixDialog_Image::setPositionToGUI()
{
	if(getPositionTo() == POSITION_TO_PARAGRAPH)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbPlaceParagraph),TRUE);
	}
	else if(getPositionTo() == POSITION_TO_COLUMN)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbPlaceColumn),TRUE);
	}
	else if(getPositionTo() == POSITION_TO_PAGE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wrbPlacePage),TRUE);
	}
}

void XAP_UnixDialog_Image::runModal(XAP_Frame * pFrame)
{
	// build the dialog
	GtkWidget * cf = _constructWindow();
	UT_return_if_fail(cf);	
	
	setHeightEntry();
	setWidthEntry();
	double height = UT_convertToInches(getHeightString());
	double width = UT_convertToInches(getWidthString());
	
	if((height > 0.0001) && (width > 0.0001))
		m_dHeightWidth = height/width;
	else
	{
		m_dHeightWidth = 0.0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_wAspectCheck), FALSE);
	}	  
	
	setWrappingGUI();
	setPositionToGUI();
	wrappingChanged();

	switch ( abiRunModalDialog ( GTK_DIALOG(cf), pFrame, this, BUTTON_CANCEL, false ) )
    {
    case GTK_RESPONSE_OK:
		event_Ok (); break;
    default:
		event_Cancel (); break;
    }

	abiDestroyWidget (cf);
}

void XAP_UnixDialog_Image::_connectSignals (void)
{
  g_signal_connect(G_OBJECT(m_wHeightSpin),
				   "changed",
				   G_CALLBACK(s_HeightSpin_changed),
				   static_cast<gpointer>(this));
  
  m_iHeightID = g_signal_connect(G_OBJECT(m_wHeightEntry),
								 "changed",
								 G_CALLBACK(s_HeightEntry_changed),
								 static_cast<gpointer>(this));

  g_signal_connect(G_OBJECT(m_wWidthSpin),
				   "changed",
				   G_CALLBACK(s_WidthSpin_changed),
				   static_cast<gpointer>(this));

  g_signal_connect(G_OBJECT(m_wrbInLine),
				   "clicked",
				   G_CALLBACK(s_wrapping_changed),
				   static_cast<gpointer>(this));


  g_signal_connect(G_OBJECT(m_wrbWrappedRight),
				   "clicked",
				   G_CALLBACK(s_wrapping_changed),
				   static_cast<gpointer>(this));


  g_signal_connect(G_OBJECT(m_wrbWrappedLeft),
				   "clicked",
				   G_CALLBACK(s_wrapping_changed),
				   static_cast<gpointer>(this));


  g_signal_connect(G_OBJECT(m_wrbWrappedBoth),
				   "clicked",
				   G_CALLBACK(s_wrapping_changed),
				   static_cast<gpointer>(this));
  
  m_iWidthID = g_signal_connect(G_OBJECT(m_wWidthEntry),
								"changed",
								G_CALLBACK(s_WidthEntry_changed),
								static_cast<gpointer>(this));
  
  g_signal_connect(G_OBJECT(m_wAspectCheck),
				   "clicked",
				   G_CALLBACK(s_aspect_clicked),
				   static_cast<gpointer>(this));
}

GtkWidget * XAP_UnixDialog_Image::_constructWindow ()
{
	const XAP_StringSet * pSS = m_pApp->getStringSet();
	
	// get the path where our glade file is located
	XAP_UnixApp * pApp = static_cast<XAP_UnixApp*>(m_pApp);
	UT_String glade_path( pApp->getAbiSuiteAppGladeDir() );
	glade_path += "/xap_UnixDlg_Image.glade";
	
	// load the dialog from the glade file
	GladeXML *xml = abiDialogNewFromXML( glade_path.c_str() );
	if (!xml)
		return NULL;
	
	mMainWindow = glade_xml_get_widget(xml, "xap_UnixDlg_Image");
	m_wHeightSpin = glade_xml_get_widget(xml, "spinHeight");
	m_wHeightEntry = glade_xml_get_widget(xml, "entryHeight");
	m_wWidthSpin = glade_xml_get_widget(xml, "spinWidth");
	m_wWidthEntry = glade_xml_get_widget(xml, "entryWidth");
	m_wAspectCheck = glade_xml_get_widget(xml, "ckAspect");

	m_wTitleEntry = glade_xml_get_widget(xml, "entryTitle");
	m_wAltEntry = glade_xml_get_widget(xml, "entryAlt");

	m_bAspect = getPreserveAspect();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_wAspectCheck), m_bAspect);
	
	m_oHeightSpin_adj = gtk_adjustment_new( 1,-2000, 2000, 1, 1, 10);
   	gtk_widget_set_size_request(m_wHeightSpin,13,-1);  
	gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON(m_wHeightSpin), GTK_ADJUSTMENT(m_oHeightSpin_adj));
	
	m_oWidthSpin_adj = gtk_adjustment_new( 1,-2000, 2000, 1, 1, 10);
	gtk_widget_set_size_request(m_wWidthSpin,13,-1);  
	gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON(m_wWidthSpin), GTK_ADJUSTMENT(m_oWidthSpin_adj));
	
	UT_UTF8String s;
	pSS->getValueUTF8(XAP_STRING_ID_DLG_Image_Title,s);
	abiDialogSetTitle(mMainWindow, s.utf8_str());

	localizeLabelMarkup(glade_xml_get_widget(xml, "lblSize"), pSS, XAP_STRING_ID_DLG_Image_ImageSize);
	localizeLabelMarkup(glade_xml_get_widget(xml, "lblImageDescription"), pSS, XAP_STRING_ID_DLG_Image_ImageDesc);
	localizeLabelMarkup(glade_xml_get_widget(xml, "lblTextWrapping"), pSS, XAP_STRING_ID_DLG_Image_TextWrapping);
	localizeLabelMarkup(glade_xml_get_widget(xml, "lblImagePlacement"), pSS, XAP_STRING_ID_DLG_Image_Placement);
	
	localizeLabel(glade_xml_get_widget(xml,"lblHeight"), pSS, XAP_STRING_ID_DLG_Image_Height);
	localizeLabel(glade_xml_get_widget(xml,"lblWidth"), pSS, XAP_STRING_ID_DLG_Image_Width);
	localizeLabel(glade_xml_get_widget(xml,"lblTitle"), pSS, XAP_STRING_ID_DLG_Image_LblTitle);
	localizeLabel(glade_xml_get_widget(xml,"lblAlt"), pSS, XAP_STRING_ID_DLG_Image_LblAlt);

	localizeLabel(glade_xml_get_widget(xml,"lblInLine"), pSS, XAP_STRING_ID_DLG_Image_InLine);
	localizeLabel(glade_xml_get_widget(xml,"lblWrappedRight"), pSS, XAP_STRING_ID_DLG_Image_WrappedRight);
	localizeLabel(glade_xml_get_widget(xml,"lblWrappedLeft"), pSS, XAP_STRING_ID_DLG_Image_WrappedLeft);
	localizeLabel(glade_xml_get_widget(xml,"lblWrappedBoth"), pSS, XAP_STRING_ID_DLG_Image_WrappedBoth);

	localizeLabel(glade_xml_get_widget(xml,"lblPlaceParagraph"), pSS, XAP_STRING_ID_DLG_Image_PlaceParagraph);
	localizeLabel(glade_xml_get_widget(xml,"lblPlaceColumn"), pSS, XAP_STRING_ID_DLG_Image_PlaceColumn);
	localizeLabel(glade_xml_get_widget(xml,"lblPlacePage"), pSS, XAP_STRING_ID_DLG_Image_PlacePage);

	m_wPlaceTable = glade_xml_get_widget(xml,"tbPlacement");
	m_wrbInLine = glade_xml_get_widget(xml,"rbInLine");
	m_wrbWrappedRight = glade_xml_get_widget(xml,"rbWrappedRight");
	m_wrbWrappedLeft = glade_xml_get_widget(xml,"rbWrappedLeft");
	m_wrbWrappedBoth = glade_xml_get_widget(xml,"rbWrappedBoth");

	m_wrbPlaceParagraph = glade_xml_get_widget(xml,"rbPlaceParagraph");
	m_wrbPlaceColumn = glade_xml_get_widget(xml,"rbPlaceColumn");
	m_wrbPlacePage = glade_xml_get_widget(xml,"rbPlacePage");

// the check button already contains a label. We have to remove this
// before we can localize it

	gtk_container_remove(GTK_CONTAINER(m_wAspectCheck), gtk_bin_get_child(GTK_BIN(m_wAspectCheck)));
	pSS->getValueUTF8 (XAP_STRING_ID_DLG_Image_Aspect,s);
	gtk_button_set_label(GTK_BUTTON(m_wAspectCheck), s.utf8_str());

	m_iWidth = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(m_wWidthSpin));
	m_iHeight = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(m_wHeightSpin));

	gtk_entry_set_text (GTK_ENTRY(m_wTitleEntry), getTitle().utf8_str());
	gtk_entry_set_text (GTK_ENTRY(m_wAltEntry), getAlt().utf8_str());

	_connectSignals ();
	
	return mMainWindow;
}
