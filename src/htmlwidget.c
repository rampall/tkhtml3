/*
** The main routine for the HTML widget for Tcl/Tk
** $Revision: 1.1 $
**
** Copyright (C) 1997,1998 D. Richard Hipp
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
** Author contact information:
**   drh@acm.org
**   http://www.hwaci.com/drh/
*/
#define panic bogus_procedure_name
#include <tkPort.h>
#undef panic
#include <default.h>
#include <tclInt.h>
#include "htmlwidget.h"

/*
** This global variable is used for tracing the operation of
** the Html formatter.
*/
int HtmlTraceMask = 0;

/*
** Information used for argv parsing.
*/
Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_STRING, "-appletcommand", "appletCommand", "HtmlCallback",
        DEF_HTML_CALLBACK, Tk_Offset(HtmlWidget, zAppletCommand), 0},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_HTML_BG_COLOR, Tk_Offset(HtmlWidget, border),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_HTML_BG_MONO, Tk_Offset(HtmlWidget, border),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_STRING, "-boldfontfamily", "fontFamily", "FontFamily",
	DEF_HTML_BOLD_FONT_FAMILY, 
        Tk_Offset(HtmlWidget, boldFontFamily), 0},
    {TK_CONFIG_STRING, "-boldfontsize", "fontSize", "FontSize",
	DEF_HTML_BOLD_FONT_SIZE, 
        Tk_Offset(HtmlWidget, boldFontSize), 0},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_HTML_BORDER_WIDTH, Tk_Offset(HtmlWidget, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_HTML_CURSOR, Tk_Offset(HtmlWidget, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-cwfontfamily", "fontFamily", "FontFamily",
	DEF_HTML_CW_FONT_FAMILY, 
        Tk_Offset(HtmlWidget, cwFontFamily), 0},
    {TK_CONFIG_STRING, "-cwfontsize", "fontSize", "FontSize",
	DEF_HTML_CW_FONT_SIZE, 
        Tk_Offset(HtmlWidget, cwFontSize), 0},
    {TK_CONFIG_BOOLEAN, "-exportselection", "exportSelection","ExportSelection",
        DEF_HTML_EXPORT_SEL, Tk_Offset(HtmlWidget, exportSelection), 0},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_STRING, "-fontfamily", "fontFamily", "FontFamily",
	DEF_HTML_NORMAL_FONT_FAMILY, 
        Tk_Offset(HtmlWidget, normalFontFamily), 0},
    {TK_CONFIG_STRING, "-fontsize", "fontSize", "FontSize",
	DEF_HTML_NORMAL_FONT_SIZE, 
        Tk_Offset(HtmlWidget, normalFontSize), 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_HTML_FG, Tk_Offset(HtmlWidget, fgColor), 0},
    {TK_CONFIG_STRING, "-formcommand", "formlCommand", "HtmlCallback",
        DEF_HTML_CALLBACK, Tk_Offset(HtmlWidget, zFormCommand), 0},
    {TK_CONFIG_STRING, "-framecommand", "frameCommand", "HtmlCallback",
        DEF_HTML_CALLBACK, Tk_Offset(HtmlWidget, zFrameCommand), 0},
    {TK_CONFIG_PIXELS, "-height", "height", "Hidth",
	DEF_HTML_HEIGHT, Tk_Offset(HtmlWidget, height), 0},
    {TK_CONFIG_COLOR, "-highlightbackground", "highlightBackground",
	"HighlightBackground", DEF_HTML_HIGHLIGHT_BG,
	Tk_Offset(HtmlWidget, highlightBgColorPtr), 0},
    {TK_CONFIG_COLOR, "-highlightcolor", "highlightColor", "HighlightColor",
	DEF_HTML_HIGHLIGHT, Tk_Offset(HtmlWidget, highlightColorPtr), 0},
    {TK_CONFIG_PIXELS, "-highlightthickness", "highlightThickness",
	"HighlightThickness",
	DEF_HTML_HIGHLIGHT_WIDTH, Tk_Offset(HtmlWidget, highlightWidth), 0},
    {TK_CONFIG_STRING, "-hyperlinkcommand", "hyperlinkCommand", "HtmlCallback",
        DEF_HTML_CALLBACK, Tk_Offset(HtmlWidget, zHyperlinkCommand), 0},
    {TK_CONFIG_STRING, "-imagecommand", "imageCommand", "HtmlCallback",
        DEF_HTML_CALLBACK, Tk_Offset(HtmlWidget, zGetImage), 0},
    {TK_CONFIG_INT, "-insertofftime", "insertOffTime", "OffTime",
        DEF_HTML_INSERT_OFF_TIME, Tk_Offset(HtmlWidget, insOffTime), 0},
    {TK_CONFIG_INT, "-insertontime", "insertOnTime", "OnTime",
        DEF_HTML_INSERT_ON_TIME, Tk_Offset(HtmlWidget, insOnTime), 0},
    {TK_CONFIG_STRING, "-isvisitedcommand", "isVisitedCommand", "HtmlCallback",
        DEF_HTML_CALLBACK, Tk_Offset(HtmlWidget, zIsVisited), 0},
    {TK_CONFIG_STRING, "-italicfontfamily", "fontFamily", "FontFamily",
	DEF_HTML_ITALIC_FONT_FAMILY, 
        Tk_Offset(HtmlWidget, italicFontFamily), 0},
    {TK_CONFIG_STRING, "-italicfontsize", "fontSize", "FontSize",
	DEF_HTML_ITALIC_FONT_SIZE, 
        Tk_Offset(HtmlWidget, italicFontSize), 0},
    {TK_CONFIG_PIXELS, "-padx", "padX", "Pad",
	DEF_HTML_PADX, Tk_Offset(HtmlWidget, padx), 0},
    {TK_CONFIG_PIXELS, "-pady", "padY", "Pad",
	DEF_HTML_PADY, Tk_Offset(HtmlWidget, pady), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_HTML_RELIEF, Tk_Offset(HtmlWidget, relief), 0},
    {TK_CONFIG_COLOR, "-selectioncolor", "background", "Background",
	DEF_HTML_SELECTION_COLOR, Tk_Offset(HtmlWidget, selectionColor), 0},
    {TK_CONFIG_STRING, "-takefocus", "takeFocus", "TakeFocus",
	DEF_HTML_TAKE_FOCUS, Tk_Offset(HtmlWidget, takeFocus),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_COLOR, "-unvisitedcolor", "foreground", "Foreground",
	DEF_HTML_UNVISITED, Tk_Offset(HtmlWidget, newLinkColor), 0},
    {TK_CONFIG_COLOR, "-visitedcolor", "foreground", "Foreground",
	DEF_HTML_VISITED, Tk_Offset(HtmlWidget, oldLinkColor), 0},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_HTML_WIDTH, Tk_Offset(HtmlWidget, width), 0},
    {TK_CONFIG_STRING, "-xscrollcommand", "xScrollCommand", "ScrollCommand",
	DEF_HTML_SCROLL_COMMAND, Tk_Offset(HtmlWidget, xScrollCmd),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-yscrollcommand", "yScrollCommand", "ScrollCommand",
	DEF_HTML_SCROLL_COMMAND, Tk_Offset(HtmlWidget, yScrollCmd),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
** Find the width of the usable drawing area in pixels.  If the window isn't
** mapped, use the size requested by the user.
**
** The usable drawing area is the area available for displaying rendered
** HTML.  The usable drawing area does not include the 3D border or the
** padx and pady boundry within the 3D border.  The usable drawing area
** is the size of the clipping window.
*/
int HtmlUsableWidth(HtmlWidget *htmlPtr){
  int w;
  Tk_Window tkwin = htmlPtr->tkwin;
  if( tkwin && Tk_IsMapped(tkwin) ){
    w = Tk_Width(tkwin) - 2*(htmlPtr->padx + htmlPtr->inset);
    TestPoint(0);
  }else{
    w = htmlPtr->width;
    TestPoint(0);
  }
  return w;
}

/*
** Find the height of the usable drawing area in pixels.  If the window isn't
** mapped, use the size requested by the user.
**
** The usable drawing area is the area available for displaying rendered
** HTML.  The usable drawing area does not include the 3D border or the
** padx and pady boundry within the 3D border.  The usable drawing area
** is the size of the clipping window.
*/
int HtmlUsableHeight(HtmlWidget *htmlPtr){
  int h;
  Tk_Window tkwin = htmlPtr->tkwin;
  if( tkwin && Tk_IsMapped(tkwin) ){
    h = Tk_Height(tkwin) - 2*(htmlPtr->pady + htmlPtr->inset);
    TestPoint(0);
  }else{
    h = htmlPtr->height;
    TestPoint(0);
  }
  return h;
}

/*
** Compute a pair of floating point numbers that describe the current
** vertical scroll position.  The first number is the fraction of
** the document that is off the top of the visible region and the second 
** number is the fraction that is beyond the end of the visible region.
*/
void HtmlComputeVerticalPosition(
  HtmlWidget *htmlPtr,
  char *buf               /* Write the two floating point values here */
){
  int actual;              /* Size of the viewing area */
  double frac1, frac2;

  actual = HtmlUsableHeight(htmlPtr);
  if( htmlPtr->maxY <= 0 ){
    frac1 = 0.0;
    frac2 = 1.0;
    TestPoint(0);
  }else{
    frac1 = (double)htmlPtr->yOffset/(double)htmlPtr->maxY;
    if( frac1 > 1.0 ){
      frac1 = 1.0;
      TestPoint(0);
    }else if( frac1 < 0.0 ){
      frac1 = 0.0;
      TestPoint(0);
    }
    frac2 = (double)(htmlPtr->yOffset+actual)/(double)htmlPtr->maxY;
    if( frac2 > 1.0 ){
      frac2 = 1.0;
      TestPoint(0);
    }else if( frac2 < 0.0 ){
      frac2 = 0.0;
      TestPoint(0);
    }
  }
  sprintf(buf,"%g %g",frac1, frac2);
}

/*
** Do the same thing for the horizontal direction
*/
void HtmlComputeHorizontalPosition(
  HtmlWidget *htmlPtr,
  char *buf               /* Write the two floating point values here */
){
  int actual;             /* Size of the viewing area */
  double frac1, frac2;

  actual = HtmlUsableWidth(htmlPtr);
  if( htmlPtr->maxX <= 0 ){
    frac1 = 0.0;
    frac2 = 1.0;
    TestPoint(0);
  }else{
    frac1 = (double)htmlPtr->xOffset/(double)htmlPtr->maxX;
    if( frac1 > 1.0 ){
      frac1 = 1.0;
      TestPoint(0);
    }else if( frac1 < 0.0 ){
      frac1 = 0.0;
      TestPoint(0);
    }
    frac2 = (double)(htmlPtr->xOffset+actual)/(double)htmlPtr->maxX;
    if( frac2 > 1.0 ){
      frac2 = 1.0;
      TestPoint(0);
    }else if( frac2 < 0.0 ){
      frac2 = 0.0;
      TestPoint(0);
    }
  }
  sprintf(buf,"%g %g",frac1, frac2);
}

/*
** Clear the cache of GCs
*/
static void ClearGcCache(HtmlWidget *htmlPtr){
  int i;
  for(i=0; i<N_CACHE_GC; i++){
    if( htmlPtr->aGcCache[i].index ){
      Tk_FreeGC(htmlPtr->display, htmlPtr->aGcCache[i].gc);
      htmlPtr->aGcCache[i].index = 0;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
  }
}
  

/*
** This routine is called when the widget command is deleted.  If the
** widget isn't already in the process of being destroyed, this command
** starts that process rolling.
**
** This routine can be called in two ways.  
**
**   (1) The window is destroyed, which causes the command to be deleted.
**       In this case, we don't have to do anything.
**
**   (2) The command only is deleted (ex: "rename .html {}").  In that
**       case we need to destroy the window.
*/
static void HtmlCmdDeletedProc(ClientData clientData){
  HtmlWidget *htmlPtr = (HtmlWidget*) clientData;
  if (htmlPtr != NULL && htmlPtr->tkwin!=NULL ) {
    Tk_Window tkwin = htmlPtr->tkwin;
    htmlPtr->tkwin = NULL;
    Tk_DestroyWindow(tkwin);
    TestPoint(0);
  }else{
    TestPoint(0);
  }
}

/*
** Reset the main layout context in the main widget.  This happens
** before we redo the layout, or just before deleting the widget.
*/
static void ResetLayoutContext(HtmlWidget *htmlPtr){
  htmlPtr->layoutContext.headRoom = 0;
  htmlPtr->layoutContext.top = 0;
  htmlPtr->layoutContext.bottom = 0;
  while( htmlPtr->layoutContext.leftMargin ){
    HtmlPopMargin( &htmlPtr->layoutContext.leftMargin );
    TestPoint(0);
  }
  while( htmlPtr->layoutContext.rightMargin ){
    HtmlPopMargin( &htmlPtr->layoutContext.rightMargin );
    TestPoint(0);
  }
}

/*
** This routine is invoked in order to redraw all or part of the HTML
** widget.  This might happen because the display has changed, or in
** response to an expose event.  In all cases, though, this routine
** is called by an idle callback.
*/
static void HtmlRedrawCallback(ClientData clientData){
  HtmlWidget *htmlPtr = (HtmlWidget*)clientData;
  Tk_Window tkwin = htmlPtr->tkwin;
  Tk_Window clipwin = htmlPtr->clipwin;
  Pixmap pixmap;           /* The buffer on which to render HTML */
  int x, y, w, h;          /* Virtual canvas coordinates of area to draw */
  int hw;                  /* highlight thickness */
  int insetX, insetY;      /* Total highlight thickness, border width and 
                           ** padx/y */
  int clipwinH, clipwinW;  /* Width and height of the clipping window */
  GC gc;                   /* Needed by XCopyArea */
  HtmlBlock *pBlock;       /* For looping over blocks to be drawn */
  int redoSelection = 0;   /* True to recompute the selection */
  

  if( tkwin==0 || !Tk_IsMapped(tkwin) ){
    TestPoint(0);
    return;
  }

  /*
  ** Recompute the layout, if necessary or requested.
  **
  ** Calling HtmlLayout() is tricky because HtmlLayout() may invoke one
  ** or more callbacks (thru the "-imagecommand" callback, for instance)
  ** and these callbacks could, in theory, do nasty things like delete 
  ** or unmap this widget.  So we have to take precautions:
  **
  **   *  Don't remove the REDRAW_PENDING flag until after HtmlLayout()
  **      has been called, to prevent a recursive call to HtmlRedrawCallback().
  **
  **   *  Call Tcl_Preserve() on the htmlPtr structure to prevent it from
  **      being deleted out from under us.
  **
  **   *  After HtmlLayout() returns, check to see tkwin==0.  If there was
  **      an attempt to delete the widget in a callback, then tkwin will have
  **      been set to zero.  If that is the case, we can bail out without
  **      bothering to do any additional work.
  **
  */
  if( (htmlPtr->flags & RESIZE_ELEMENTS)!=0 ){
    HtmlImage *pImage;
    for(pImage=htmlPtr->imageList; pImage; pImage=pImage->pNext){
      pImage->pList = 0;
      TestPoint(0);
    }
    htmlPtr->lastSized = 0;
    htmlPtr->flags &= ~RESIZE_ELEMENTS;
    htmlPtr->flags |= RELAYOUT;
  }else{
    TestPoint(0);
  }

  /* We used to make a distinction between RELAYOUT and EXTEND_LAYOUT.
  ** RELAYOUT would be used when the widget was resized, but the
  ** less compute-intensive EXTEND_LAYOUT would be used when new
  ** text was appended.
  **
  ** Unfortunately, EXTEND_LAYOUT has some problem that arise when
  ** tables are used.  The quick fix is to make an EXTEND_LAYOUT do
  ** a complete RELAYOUT.  Someday, we need to fix EXTEND_LAYOUT so
  ** that it works right...
  */
  if( (htmlPtr->flags & (RELAYOUT|EXTEND_LAYOUT))!=0 ){
    htmlPtr->nextPlaced = 0;
    htmlPtr->nInput = 0;
#if 0
    HtmlDeleteControls(htmlPtr);
#endif
    htmlPtr->varId = 0;
    htmlPtr->maxX = 0;
    htmlPtr->maxY = 0;
    ResetLayoutContext(htmlPtr);
    htmlPtr->firstBlock = 0;
    htmlPtr->lastBlock = 0;
    redoSelection = 1;
    htmlPtr->flags &= ~RELAYOUT;
    htmlPtr->flags |= HSCROLL | VSCROLL | REDRAW_TEXT | EXTEND_LAYOUT;
    TestPoint(0);
  }else{
    TestPoint(0);
  }
  if( (htmlPtr->flags & EXTEND_LAYOUT) && htmlPtr->pFirst!=0 ){
    Tcl_Preserve(htmlPtr);
    HtmlLayout(htmlPtr);
    tkwin = htmlPtr->tkwin;
    Tcl_Release(htmlPtr);
    if( tkwin==0 ) return;
    htmlPtr->flags &= ~EXTEND_LAYOUT;
    HtmlFormBlocks(htmlPtr);
    HtmlMapControls(htmlPtr);
    if( redoSelection && htmlPtr->selBegin.p && htmlPtr->selEnd.p ){
      HtmlUpdateSelection(htmlPtr,1);
      HtmlUpdateInsert(htmlPtr);
      TestPoint(0);
    }else{
      TestPoint(0);
    }
  }else{
    TestPoint(0);
  }
  htmlPtr->flags &= ~REDRAW_PENDING;

  /* Redraw the scrollbars.  Take care here, since the scrollbar
  ** update command could (in theory) delete the html widget, or
  ** even the whole interpreter.  Preserve critical data structures,
  ** and check to see if we are still alive before continuing.
  */
  if( (htmlPtr->flags & (HSCROLL|VSCROLL)) != 0 ){
    Tcl_Interp *interp = htmlPtr->interp;
    int result;
    char buf[200];

    Tcl_Preserve(htmlPtr);
    if( (htmlPtr->flags & HSCROLL)!=0 ){
      if( htmlPtr->xScrollCmd && htmlPtr->xScrollCmd[0] ){
        HtmlComputeHorizontalPosition(htmlPtr,buf);
        Tcl_Preserve(interp);
        result = Tcl_VarEval(interp, htmlPtr->xScrollCmd, " ", buf, 0);
        tkwin = htmlPtr->tkwin;
        if (result != TCL_OK) {
          Tcl_AddErrorInfo(interp,
             "\n    (horizontal scrolling command executed by html widget)");
          Tcl_BackgroundError(interp);
          TestPoint(0);
        }else{
          TestPoint(0);
        }
        Tcl_Release(interp);
        if( tkwin==0 ){ TestPoint(0); return; }
      }else{
        TestPoint(0);
      }
      htmlPtr->flags &= ~HSCROLL;
    }else{
      TestPoint(0);
    }
    if( (htmlPtr->flags & VSCROLL)!=0 && tkwin && Tk_IsMapped(tkwin) ){
      if( htmlPtr->yScrollCmd && htmlPtr->yScrollCmd[0] ){
        Tcl_Interp *interp = htmlPtr->interp;
        int result;
        char buf[200];
        HtmlComputeVerticalPosition(htmlPtr,buf);
        Tcl_Preserve(interp);
        result = Tcl_VarEval(interp, htmlPtr->yScrollCmd, " ", buf, 0);
        tkwin = htmlPtr->tkwin;
        if (result != TCL_OK) {
          Tcl_AddErrorInfo(interp,
             "\n    (vertical scrolling command executed by html widget)");
          Tcl_BackgroundError(interp);
          TestPoint(0);
        }else{
          TestPoint(0);
        }
        Tcl_Release(interp);
        if( tkwin==0 ) return;
      }else{
        TestPoint(0);
      }
      htmlPtr->flags &= ~VSCROLL;
    }else{
      TestPoint(0);
    }
    Tcl_Release(htmlPtr);
    if( tkwin==0 || !Tk_IsMapped(tkwin) ){ TestPoint(0); return; }
    if( htmlPtr->flags & REDRAW_PENDING ){ TestPoint(0); return; }
    clipwin = htmlPtr->clipwin;
    if( clipwin==0 ){ TestPoint(0); return; }
  }

  /* Redraw the focus highlight, if requested */
  hw = htmlPtr->highlightWidth;
  if( htmlPtr->flags & REDRAW_FOCUS ){
    if( hw>0 ){
      GC gc;
      Tk_Window tkwin = htmlPtr->tkwin;

      if( htmlPtr->flags & GOT_FOCUS ){
        gc = Tk_GCForColor(htmlPtr->highlightColorPtr, Tk_WindowId(tkwin));
        TestPoint(0);
      }else{
        gc = Tk_GCForColor(htmlPtr->highlightBgColorPtr, Tk_WindowId(tkwin));
        TestPoint(0);
      }
      Tk_DrawFocusHighlight(tkwin, gc, hw, Tk_WindowId(tkwin));
    }else{
      TestPoint(0);
    }
    htmlPtr->flags &= ~REDRAW_FOCUS;
  }else{
    TestPoint(0);
  }

  /* Draw the borders around the parameter of the window.  This is
  ** drawn directly -- it is not double buffered.
  */
  if( htmlPtr->flags & REDRAW_BORDER ){
    htmlPtr->flags &= ~REDRAW_BORDER;
    Tk_Fill3DRectangle(tkwin, Tk_WindowId(tkwin), htmlPtr->border,
          hw,                        /* x */
          hw,                        /* y */
          Tk_Width(tkwin) - 2*hw,    /* width */
          Tk_Height(tkwin) - 2*hw,   /* height */
          htmlPtr->borderWidth, htmlPtr->relief);
    TestPoint(0);
  }else{
    TestPoint(0);
  }

  /*
  ** If we don't have a clipping window, then something is seriously
  ** wrong.  We might as well give up.
  */
  if( clipwin==NULL ){ TestPoint(0); goto earlyOut; }

  /* Resize, reposition and map the clipping window, if necessary */
  insetX = htmlPtr->padx + htmlPtr->inset;
  insetY = htmlPtr->pady + htmlPtr->inset;
  if( htmlPtr->flags & RESIZE_CLIPWIN ){
    int h, w;
    Tk_MoveResizeWindow(clipwin, insetX, insetY, 
       htmlPtr->realWidth - 2*insetX,
       htmlPtr->realHeight - 2*insetY);
    if( !Tk_IsMapped(clipwin) ){
      Tk_MapWindow(clipwin);
      TestPoint(0);
    }else{
      TestPoint(0);
    }
    h = htmlPtr->realHeight - 2*insetY;
    if( htmlPtr->yOffset + h > htmlPtr->maxY ){
      htmlPtr->yOffset = htmlPtr->maxY - h;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
    if( htmlPtr->yOffset < 0 ){
      htmlPtr->yOffset = 0;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
    w = htmlPtr->realWidth - 2*insetX;
    if( htmlPtr->xOffset + h > htmlPtr->maxX ){
      htmlPtr->xOffset = htmlPtr->maxX - w;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
    if( htmlPtr->xOffset < 0 ){
      htmlPtr->xOffset = 0;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
    htmlPtr->flags &= ~RESIZE_CLIPWIN;
  }

  /*
  ** Compute the virtual canvas coordinates corresponding to the
  ** dirty region of the clipping window.
  */
  clipwinW = Tk_Width(clipwin);
  clipwinH = Tk_Height(clipwin);
  if( htmlPtr->flags & REDRAW_TEXT ){
    w = clipwinW;
    h = clipwinH;
    x = htmlPtr->xOffset;
    y = htmlPtr->yOffset;
    htmlPtr->dirtyLeft = 0;
    htmlPtr->dirtyTop = 0;
    htmlPtr->flags &= ~REDRAW_TEXT;
    TestPoint(0);
  }else{
    if( htmlPtr->dirtyLeft < 0 ){
      htmlPtr->dirtyLeft = 0;
      TestPoint(0);
    }
    if( htmlPtr->dirtyRight > clipwinW  ){
      htmlPtr->dirtyRight = clipwinW;
      TestPoint(0);
    }
    if( htmlPtr->dirtyTop < 0 ){
      htmlPtr->dirtyTop = 0;
      TestPoint(0);
    }
    if( htmlPtr->dirtyBottom > clipwinH ){
      htmlPtr->dirtyBottom = clipwinH;
      TestPoint(0);
    }
    w = htmlPtr->dirtyRight - htmlPtr->dirtyLeft;
    h = htmlPtr->dirtyBottom - htmlPtr->dirtyTop;
    x = htmlPtr->xOffset + htmlPtr->dirtyLeft;
    y = htmlPtr->yOffset + htmlPtr->dirtyTop;
  }

  /* Skip the rest of the drawing process if the area to be refreshed is
  ** less than zero */
  if( w>0 && h>0 ){
    HtmlMapControls(htmlPtr);

    /* Allocate and clear a pixmap upon which to draw */
    pixmap = Tk_GetPixmap(htmlPtr->display, Tk_WindowId(clipwin), 
                          w, h, Tk_Depth(clipwin));
    Tk_Fill3DRectangle(clipwin, pixmap, htmlPtr->border, 
                       0, 0, w, h, 0, TK_RELIEF_RAISED);
                       
    /* Render all visible HTML onto the pixmap */
    for(pBlock=htmlPtr->firstBlock; pBlock; pBlock=pBlock->pNext){
      if( pBlock->top <= y+h && pBlock->bottom >= y 
      && pBlock->left <= x+w && pBlock->right >= x ){
        HtmlBlockDraw(htmlPtr,pBlock,pixmap,x,y,w,h);
        TestPoint(0);
      }else{
        TestPoint(0);
      }
    }
     
    /* Finally, copy the pixmap onto the window and delete the pixmap */
    gc = HtmlGetAnyGC(htmlPtr);
    XCopyArea(htmlPtr->display, pixmap, Tk_WindowId(clipwin),
              gc, 0, 0, w, h, htmlPtr->dirtyLeft, htmlPtr->dirtyTop);
    Tk_FreePixmap(htmlPtr->display, pixmap);
  }

  /* Redraw images, if requested */
  if( htmlPtr->flags & REDRAW_IMAGES ){
    HtmlImage *pImage;
    HtmlElement *pElem;
    int top, bottom, left, right;     /* Coordinates of the clipping window */
    int imageTop;                     /* Top edge of image */

    top = htmlPtr->yOffset;
    bottom = top + HtmlUsableHeight(htmlPtr);
    left = htmlPtr->xOffset;
    right = left + HtmlUsableWidth(htmlPtr);
    for(pImage = htmlPtr->imageList; pImage; pImage=pImage->pNext){
      for(pElem = pImage->pList; pElem; pElem=pElem->image.pNext){
        if( pElem->image.redrawNeeded==0 ) continue;
        imageTop = pElem->image.y - pElem->image.ascent;
        if( imageTop > bottom
         || imageTop + pElem->image.h < top
         || pElem->image.x > right
         || pElem->image.x + pElem->image.w < left ){ 
            TestPoint(0); 
            continue; 
        }
        HtmlDrawImage(pElem, Tk_WindowId(htmlPtr->clipwin),
                      left, top, right, bottom);
      }
    }
    htmlPtr->flags &= ~REDRAW_IMAGES;
  }

  /* Set the dirty region to the empty set. */
  earlyOut:
  htmlPtr->dirtyTop = LARGE_NUMBER;
  htmlPtr->dirtyLeft = LARGE_NUMBER;
  htmlPtr->dirtyBottom = 0;
  htmlPtr->dirtyRight = 0;
  TestPoint(0);
}

/*
** Make sure that a call to the HtmlRedrawCallback() routine has been
** queued.
*/
void HtmlScheduleRedraw(HtmlWidget *htmlPtr){
  if( (htmlPtr->flags & REDRAW_PENDING)==0
    && htmlPtr->tkwin!=0 
    && Tk_IsMapped(htmlPtr->tkwin)
  ){
    Tcl_DoWhenIdle(HtmlRedrawCallback, (ClientData)htmlPtr);
    htmlPtr->flags |= REDRAW_PENDING;
    TestPoint(0);
  }else{
    TestPoint(0);
  }
}

/*
** If any part of the screen needs to be redrawn, Then call this routine
** with the values of a box (in window coordinates) that needs to be 
** redrawn.  This routine will make sure an idle callback is scheduled
** to do the redraw.
**
** The box coordinates are relative to the clipping window (clipwin),
** not the main window (tkwin).  
*/
void HtmlRedrawArea(
  HtmlWidget *htmlPtr,      /* The widget to be redrawn */
  int left, int top,        /* Top left corner of area to redraw */
  int right, int bottom     /* bottom right corner of area to redraw */
){
  if( bottom < 0 ){ TestPoint(0); return; }
  if( top > htmlPtr->realHeight ){ TestPoint(0); return; }
  if( right < 0 ){ TestPoint(0); return; }
  if( left > htmlPtr->realWidth ){ TestPoint(0); return; }
  if( htmlPtr->dirtyTop > top ){ htmlPtr->dirtyTop = top; TestPoint(0);}
  if( htmlPtr->dirtyLeft > left ){ htmlPtr->dirtyLeft = left; TestPoint(0);}
  if( htmlPtr->dirtyBottom < bottom ){ 
    htmlPtr->dirtyBottom = bottom;
    TestPoint(0);
  }
  if( htmlPtr->dirtyRight < right ){ htmlPtr->dirtyRight = right; TestPoint(0);}
  HtmlScheduleRedraw(htmlPtr);
  TestPoint(0);
}

/* Redraw the HtmlBlock given.
*/
void HtmlRedrawBlock(HtmlWidget *htmlPtr, HtmlBlock *p){
  if( p ){
    HtmlRedrawArea(htmlPtr,
      p->left - htmlPtr->xOffset,
      p->top - htmlPtr->yOffset,
      p->right - htmlPtr->xOffset + 1,
      p->bottom - htmlPtr->yOffset
    );
    TestPoint(0);
  }else{
    TestPoint(0);
  }
}

/*
** Call this routine to force the entire widget to be redrawn.
*/
void HtmlRedrawEverything(HtmlWidget *htmlPtr){
  htmlPtr->flags |= REDRAW_FOCUS | REDRAW_TEXT | REDRAW_BORDER;
  HtmlScheduleRedraw(htmlPtr);
  TestPoint(0);
}

/*
** Do the redrawing right now.  Don't wait.
*/
#if 0  /* NOT_USED */
static void HtmlRedrawPush(HtmlWidget *htmlPtr){
  if( htmlPtr->flags & REDRAW_PENDING ){
    Tcl_CancelIdleCall(HtmlRedrawCallback, (ClientData)htmlPtr);
    TestPoint(0);
  }else{
    TestPoint(0);
  }
  HtmlRedrawCallback( (ClientData)htmlPtr );
}
#endif

/*
** Call this routine to cause all of the rendered HTML at the
** virtual canvas coordinate of Y and beyond to be redrawn.
*/
void HtmlRedrawText(HtmlWidget *htmlPtr, int y){
  int yOffset;        /* Top-most visible canvas coordinate */
  int clipHeight;     /* Height of the clipping window */
  
  yOffset = htmlPtr->yOffset;
  clipHeight = HtmlUsableHeight(htmlPtr);
  y -= yOffset;
  if( y < clipHeight ){
    HtmlRedrawArea(htmlPtr, 0, y, LARGE_NUMBER, clipHeight);
    TestPoint(0);
  }else{
    TestPoint(0);
  }
}

/*
** Recalculate the preferred size of the html widget and pass this
** along to the geometry manager.
*/
static void HtmlRecomputeGeometry(HtmlWidget *htmlPtr){
  int w, h;      /* Total width and height of the widget */

  htmlPtr->inset = htmlPtr->highlightWidth + htmlPtr->borderWidth;
  w = htmlPtr->width + 2*(htmlPtr->padx + htmlPtr->inset);
  h = htmlPtr->height + 2*(htmlPtr->pady + htmlPtr->inset);
  Tk_GeometryRequest(htmlPtr->tkwin, w, h);
  Tk_SetInternalBorder(htmlPtr->tkwin, htmlPtr->inset);
  TestPoint(0);
}

/*
** Make sure that the given font string has no more than one %d and
** no other instances of the % character.
*/
static void SanitizeFontString(char *z){
  int seen = 0;
  if( z==0 ) return;
  while( *z ){
    if( *z=='%' ){
      if( z[1]=='%' ){
        z++;
        TestPoint(0);
      }else if( z[1]=='d' && !seen ){
        seen = 1;
        TestPoint(0);
      }else{
        *z = '#';
        TestPoint(0);
      }
    }else{
      TestPoint(0);
    }
    z++;
  }
}

/*
** This routine is called in order to process a "configure" subcommand
** on the given html widget.
*/
int ConfigureHtmlWidget(
  Tcl_Interp *interp,      /* Write error message to this interpreter */
  HtmlWidget *htmlPtr,     /* The Html widget to be configured */
  int argc,                /* Number of configuration arguments */
  char **argv,             /* Text of configuration arguments */
  int flags                /* Configuration flags */
){
  int rc;
  rc = Tk_ConfigureWidget(interp, htmlPtr->tkwin, configSpecs, argc, argv,
                         (char *) htmlPtr, flags);
  if( rc!=TCL_OK ){ TestPoint(0); return rc; }
  htmlPtr->fontValid = 0;
  htmlPtr->apColor[COLOR_Normal] = htmlPtr->fgColor;
  htmlPtr->apColor[COLOR_Visited] = htmlPtr->oldLinkColor;
  htmlPtr->apColor[COLOR_Unvisited] = htmlPtr->newLinkColor;
  htmlPtr->apColor[COLOR_Selection] = htmlPtr->selectionColor;
  SanitizeFontString(htmlPtr->normalFontFamily);
  SanitizeFontString(htmlPtr->boldFontFamily);
  SanitizeFontString(htmlPtr->italicFontFamily);
  SanitizeFontString(htmlPtr->cwFontFamily);
  Tk_SetBackgroundFromBorder(htmlPtr->tkwin, htmlPtr->border);
  if( htmlPtr->highlightWidth < 0 ){ htmlPtr->highlightWidth = 0; TestPoint(0);}
  if (htmlPtr->padx < 0) { htmlPtr->padx = 0; TestPoint(0);}
  if (htmlPtr->pady < 0) { htmlPtr->pady = 0; TestPoint(0);}
  if (htmlPtr->width < 100) { htmlPtr->width = 100; TestPoint(0);}
  if (htmlPtr->height < 100) { htmlPtr->height = 100; TestPoint(0);}
  if (htmlPtr->borderWidth < 0) {htmlPtr->borderWidth = 0; TestPoint(0);}
  htmlPtr->flags |= RESIZE_ELEMENTS | RELAYOUT | REDRAW_BORDER | RESIZE_CLIPWIN;
  HtmlRecomputeGeometry(htmlPtr);
  HtmlRedrawEverything(htmlPtr);
  ClearGcCache(htmlPtr);
  return rc;
}

/*
** Delete a single HtmlElement
*/
void HtmlDeleteElement(HtmlElement *p){
  switch( p->base.type ){
    case Html_Block:
      if( p->block.z ){
        ckfree(p->block.z);
        TestPoint(0);
      }else{
        TestPoint(0);
      }
      break;
    default:
      TestPoint(0);
      break;
  }
  ckfree((char*)p);
}

/*
** Erase all data from the HTML widget.  Bring it back to an
** empty screen.
**
** This happens (for example) when the "clear" method is invoked
** on the widget, or just before the widget is deleted.
*/
void HtmlClear(HtmlWidget *htmlPtr){
  int i;
  HtmlElement *p, *pNext;

  HtmlDeleteControls(htmlPtr);
  for(p=htmlPtr->pFirst; p; p=pNext){
    pNext = p->pNext;
    HtmlDeleteElement(p);
    TestPoint(0);
  }
  htmlPtr->pFirst = 0;
  htmlPtr->pLast = 0;
  htmlPtr->nToken = 0;
  if( htmlPtr->zText ){
    ckfree(htmlPtr->zText);
    TestPoint(0);
  }else{
    TestPoint(0);
  }
  htmlPtr->zText = 0;
  htmlPtr->nText = 0;
  htmlPtr->nAlloc = 0;
  htmlPtr->nComplete = 0;
  htmlPtr->iPlaintext = 0;
  for(i=N_PREDEFINED_COLOR; i<N_COLOR; i++){
    if( htmlPtr->apColor[i] != 0 ){
      Tk_FreeColor(htmlPtr->apColor[i]);
      htmlPtr->apColor[i] = 0;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
  }
  htmlPtr->colorUsed = 0;
  while( htmlPtr->imageList ){
    HtmlImage *p = htmlPtr->imageList;
    htmlPtr->imageList = p->pNext;
    Tk_FreeImage(p->image);
    ckfree((char*)p);
    TestPoint(0);
  }
  HtmlPopStyleStack(htmlPtr, -1);
  ClearGcCache(htmlPtr);
  ResetLayoutContext(htmlPtr);
  htmlPtr->lastSized = 0;
  htmlPtr->nextPlaced = 0;
  htmlPtr->firstBlock = 0;
  htmlPtr->lastBlock = 0;
  htmlPtr->nInput = 0;
  htmlPtr->varId = 0;
  htmlPtr->paraAlignment = ALIGN_None;
  htmlPtr->rowAlignment = ALIGN_None;
  htmlPtr->anchorFlags = 0;
  htmlPtr->inDt = 0;
  htmlPtr->anchorStart = 0;
  htmlPtr->formStart = 0;
  htmlPtr->innerList = 0;
  htmlPtr->maxX = 0;
  htmlPtr->maxY = 0;
  htmlPtr->xOffset = 0;
  htmlPtr->yOffset = 0;
  htmlPtr->pInsBlock = 0;
  htmlPtr->ins.p = 0;
  htmlPtr->selBegin.p = 0;
  htmlPtr->selEnd.p = 0;
  htmlPtr->pSelStartBlock = 0;
  htmlPtr->pSelEndBlock = 0;
}

/*
** This routine is called by Tcl_EventuallyFree or Tcl_Release to clean
** up the internal structure of an HtmlWidget when the widget is deleted.
*/
static void DestroyHtmlWidget(char *memPtr){
  HtmlWidget *htmlPtr = (HtmlWidget*) memPtr;
  int i;

  HtmlClear(htmlPtr);
  Tk_FreeOptions(configSpecs, (char*) htmlPtr, htmlPtr->display, 0);
  for(i=0; i<N_FONT; i++){
    if( htmlPtr->aFont[i] != 0 ){
      Tk_FreeFont(htmlPtr->aFont[i]);
      htmlPtr->aFont[i] = 0;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
  }
  for(i=0; i<Html_TypeCount; i++){
    if( htmlPtr->zHandler[i] ){
      ckfree(htmlPtr->zHandler[i]);
      htmlPtr->zHandler[i] = 0;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
  }
  HtmlClearUrl(htmlPtr);
  if( htmlPtr->insTimer ){
    Tcl_DeleteTimerHandler(htmlPtr->insTimer);
    htmlPtr->insTimer = 0;
    TestPoint(0);
  }else{
    TestPoint(0);
  }
  ckfree(htmlPtr->zClipwin);
  ckfree((char*) htmlPtr);
}

/*
** Flash the insertion cursor.
*/
void HtmlFlashCursor(ClientData clientData){
  HtmlWidget *htmlPtr = (HtmlWidget*)clientData;
  if( htmlPtr->pInsBlock==0 || htmlPtr->insOnTime<=0
      || htmlPtr->insOffTime<=0 ){
    htmlPtr->insTimer = 0;
    TestPoint(0);
    return;
  }
  HtmlRedrawBlock(htmlPtr, htmlPtr->pInsBlock);
  if( (htmlPtr->flags & GOT_FOCUS)==0 ){
    htmlPtr->insStatus = 0;
    htmlPtr->insTimer = 0;
    TestPoint(0);
  }else if( htmlPtr->insStatus ){
    htmlPtr->insTimer = Tcl_CreateTimerHandler(htmlPtr->insOffTime,
                                               HtmlFlashCursor, clientData);
    htmlPtr->insStatus = 0;
    TestPoint(0);
  }else{
    htmlPtr->insTimer = Tcl_CreateTimerHandler(htmlPtr->insOnTime,
                                               HtmlFlashCursor, clientData);
    htmlPtr->insStatus = 1;
    TestPoint(0);
  }
}

/*
** Return a GC from the cache.  As many as N_CACHE_GCs are kept valid
** at any one time.  They are replaced using an LRU algorithm.
**
** A value of FONT_Any (-1) for the font means "don't care".
*/
GC HtmlGetGC(HtmlWidget *htmlPtr, int color, int font){
  int i, j;
  GcCache *p = htmlPtr->aGcCache;
  XGCValues gcValues;
  int mask;

  /* 
  ** Check for an existing GC.
  */
  if( color < 0 || color >= N_COLOR ){ color = 0; TestPoint(0); }
  if( font < FONT_Any || font >= N_FONT ){ font = FONT_Default; TestPoint(0); }
  for(i=0; i<N_CACHE_GC; i++, p++){
    if( p->index==0 ){ TestPoint(0); continue; }
    if( (font<0 || p->font==font) && p->color==color ){
      if( p->index>1 ){
        for(j=0; j<N_CACHE_GC; j++){
          if( htmlPtr->aGcCache[j].index 
          && htmlPtr->aGcCache[j].index < p->index ){
            htmlPtr->aGcCache[j].index++;
            TestPoint(0);
          }else{
            TestPoint(0);
          }
        }
        p->index = 1;
      }else{
        TestPoint(0);
      }
      return htmlPtr->aGcCache[i].gc;
    }else{
      TestPoint(0);
    }
  }

  /*
  ** No GC matches.  Find a place to allocate a new GC.
  */
  p = htmlPtr->aGcCache;
  for(i=0; i<N_CACHE_GC; i++, p++){
    if( p->index==0 || p->index==N_CACHE_GC ){ TestPoint(0); break; }
  }
  if( p->index ){
    Tk_FreeGC(htmlPtr->display, p->gc);
    TestPoint(0);
  }else{
    TestPoint(0);
  }
  gcValues.foreground = htmlPtr->apColor[color]->pixel;
  gcValues.graphics_exposures = False;
  if( font<0 ){ font = FONT_Default; TestPoint(0); }
  gcValues.font = Tk_FontId( HtmlGetFont(htmlPtr, font) );
  mask = GCForeground | GCGraphicsExposures | GCFont;
  p->gc = Tk_GetGC(htmlPtr->tkwin, mask, &gcValues);
  if( p->index==0 ){ p->index = N_CACHE_GC + 1; TestPoint(0); }
  for(j=0; j<N_CACHE_GC; j++){
    if( htmlPtr->aGcCache[j].index && htmlPtr->aGcCache[j].index < p->index ){
      htmlPtr->aGcCache[j].index++;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
  }
  p->index = 1;
  p->font = font;
  p->color = color;
  TestPoint(0);
  return p->gc;
}

/*
** Retrieve any valid GC.  The font and color don't matter since the
** GC will only be used for copying.
*/
GC HtmlGetAnyGC(HtmlWidget *htmlPtr){
  int i;
  GcCache *p = htmlPtr->aGcCache;

  for(i=0; i<N_CACHE_GC; i++, p++){
    if( p->index ){ TestPoint(0); return p->gc; }
  }
  TestPoint(0);
  return HtmlGetGC(htmlPtr, COLOR_Normal, FONT_Default);
}

/*
** All window events (for both tkwin and clipwin) are
** sent to this routine.
*/
static void HtmlEventProc(ClientData clientData, XEvent *eventPtr){
  HtmlWidget *htmlPtr = (HtmlWidget*) clientData;
  int redraw_needed = 0;
  XConfigureRequestEvent *p;

  switch( eventPtr->type ){
    case Expose:
      if( htmlPtr->tkwin==0 ){
        /* The widget is being deleted.  Do nothing */
        TestPoint(0);
      }else if( eventPtr->xexpose.window!=Tk_WindowId(htmlPtr->tkwin) ){
        /* Exposure in the clipping window */
        HtmlRedrawArea(htmlPtr, eventPtr->xexpose.x - 1, 
                   eventPtr->xexpose.y - 1,
                   eventPtr->xexpose.x + eventPtr->xexpose.width + 1,
                   eventPtr->xexpose.y + eventPtr->xexpose.height + 1);
        TestPoint(0);
      }else{
        /* Exposure in the main window */
        htmlPtr->flags |= REDRAW_BORDER;
        HtmlScheduleRedraw(htmlPtr);
        TestPoint(0);
      }
      break;
    case DestroyNotify:
      if( (htmlPtr->flags & REDRAW_PENDING) ){
        Tcl_CancelIdleCall(HtmlRedrawCallback, (ClientData)htmlPtr);
        htmlPtr->flags &= ~REDRAW_PENDING;
        TestPoint(0);
      }else{
        TestPoint(0);
      }
      if( htmlPtr->tkwin != 0 ){
        if( eventPtr->xany.window!=Tk_WindowId(htmlPtr->tkwin) ){
          Tk_DestroyWindow(htmlPtr->tkwin);
          htmlPtr->clipwin = 0;
          TestPoint(0);
          break;
        }
        htmlPtr->tkwin = 0;
        Tcl_DeleteCommand(htmlPtr->interp,
                Tcl_GetCommandName(htmlPtr->interp, htmlPtr->widgetCmd));
        TestPoint(0);
      }else{
        TestPoint(0);
      }
      Tcl_EventuallyFree((ClientData)htmlPtr, DestroyHtmlWidget);
      break;
    case ConfigureNotify:
      if( htmlPtr->tkwin!=0
       && eventPtr->xconfigure.window==Tk_WindowId(htmlPtr->tkwin) 
      ){
        p = (XConfigureRequestEvent*)eventPtr;
        if( p->width != htmlPtr->realWidth ){
          redraw_needed = 1;
          htmlPtr->realWidth = p->width;
          TestPoint(0);
        }else{
          TestPoint(0);
        }
        if( p->height != htmlPtr->realHeight ){
          redraw_needed = 1;
          htmlPtr->realHeight = p->height;
          TestPoint(0);
        }else{
          TestPoint(0);
        }
        if( redraw_needed ){
          htmlPtr->flags |= RELAYOUT | VSCROLL | HSCROLL | RESIZE_CLIPWIN;
          HtmlRedrawEverything(htmlPtr);
          TestPoint(0);
        }else{
          TestPoint(0);
        }
      }
      break;
    case FocusIn:
      if( htmlPtr->tkwin!=0
       && eventPtr->xfocus.window==Tk_WindowId(htmlPtr->tkwin)
       && eventPtr->xfocus.detail != NotifyInferior
      ){
        htmlPtr->flags |= GOT_FOCUS | REDRAW_FOCUS;
        HtmlScheduleRedraw(htmlPtr);
        HtmlUpdateInsert(htmlPtr);
        TestPoint(0);
      }else{
        TestPoint(0);
      }
      break;
    case FocusOut:
      if( htmlPtr->tkwin!=0
       && eventPtr->xfocus.window==Tk_WindowId(htmlPtr->tkwin)
       && eventPtr->xfocus.detail != NotifyInferior
      ){
        htmlPtr->flags &= ~GOT_FOCUS;
        htmlPtr->flags |= REDRAW_FOCUS;
        HtmlScheduleRedraw(htmlPtr);
        TestPoint(0);
      }else{
        TestPoint(0);
      }
      break;
  }
}


/*
** The rendering and layout routines should call this routine in order to get
** a font structure.  The iFont parameter specifies which of the N_FONT
** fonts should be obtained.  The font is allocated if necessary.
*/
Tk_Font HtmlGetFont(
  HtmlWidget *htmlPtr,        /* The HTML widget to which the font applies */
  int iFont                   /* Which font to obtain */
){
  Tk_Font toFree = 0;

  if( iFont<0 ){ iFont = 0; TestPoint(0); }
  if( iFont>=N_FONT ){ iFont = N_FONT - 1; CANT_HAPPEN; }

  /*
  ** If the font has previously been allocated, but the "fontValid" bitmap
  ** shows it is no longer valid, then mark it for freeing later.  We use
  ** a policy of allocate-before-free because Tk's font cache operates
  ** much more efficiently that way.
  */
  if( ((1<<iFont) & htmlPtr->fontValid)==0 && htmlPtr->aFont[iFont]!=0 ){
    toFree = htmlPtr->aFont[iFont];
    htmlPtr->aFont[iFont] = 0;
    TestPoint(0);
  }

  /*
  ** If we need to allocate a font, first construct the font name then
  ** allocate it.
  */
  if( htmlPtr->aFont[iFont]==0 ){
    char *name;
    char *familyStr = "";
    char *sizeStr = "";
    int iFamily;
    int iSize;
    int size;

    iFamily = iFont / N_FONT_SIZE;
    iSize = iFont % N_FONT_SIZE;
    switch( iFamily ){
      case 0:    
        familyStr = htmlPtr->normalFontFamily;
        sizeStr = htmlPtr->normalFontSize;
        TestPoint(0);
        break;
      case 1:    
        familyStr = htmlPtr->boldFontFamily;
        sizeStr = htmlPtr->boldFontSize;
        TestPoint(0);
        break;
      case 2:    
        familyStr = htmlPtr->italicFontFamily;
        sizeStr = htmlPtr->italicFontSize;
        TestPoint(0);
        break;
      case 3:    
        familyStr = htmlPtr->cwFontFamily;
        sizeStr = htmlPtr->cwFontSize;
        TestPoint(0);
        break;
    }
    name = ckalloc( strlen(familyStr) + 200 );
    size = 12;
    while( sizeStr ){
      while( *sizeStr && !isdigit(*sizeStr) ){ sizeStr++; }
      if( iSize==0 ){
        size = atoi(sizeStr);
        TestPoint(0);
        break;
      }
      iSize--;
      while( isdigit(*sizeStr) ){ sizeStr++; TestPoint(0); }
    }
    sprintf(name,familyStr,size);
    htmlPtr->aFont[iFont] = 
      Tk_GetFont(htmlPtr->interp, htmlPtr->tkwin, name);
    ckfree( name );
    htmlPtr->fontValid |= (1<<iFont);
    TestPoint(0);
  }

  /*
  ** Free the expired font, if any.
  */
  if( toFree!=0 ){
    Tk_FreeFont(toFree);
    TestPoint(0);
  }else{
    TestPoint(0);
  }
  return htmlPtr->aFont[iFont];
}

/*
** Compute the squared distance between two colors
*/
static float colorDistance(XColor *pA, XColor *pB){
  float x, y, z;

  x = 0.30 * (pA->red - pB->red);
  y = 0.61 * (pA->green - pB->green);
  z = 0.11 * (pA->blue - pB->blue);
  TestPoint(0);
  return x*x + y*y + z*z;
}

/*
** This routine returns an index between 0 and N_COLOR-1 which indicates
** which XColor structure in the apColor[] array of htmlPtr should be
** used to describe the color specified by the given name.
*/
int HtmlGetColorByName(HtmlWidget *htmlPtr, char *zColor){
  XColor *pNew;
  int i;
  float dist;
  float closestDist;
  int closest;
  Tk_Uid name = Tk_GetUid(zColor);  /**** This is a memory leak ****/

  pNew = Tk_GetColor(htmlPtr->interp, htmlPtr->clipwin, name);
  if( pNew==0 ){
    UNTESTED;
    return 0;      /* Color 0 is always the default */
  }

  /* Search for an exact match */
  for(i=0; i<N_COLOR; i++){
    XColor *p = htmlPtr->apColor[i];
    if( p && p->red==pNew->red && p->green==pNew->green 
     && p->blue==pNew->blue ){
      htmlPtr->colorUsed |= (1<<i);
      Tk_FreeColor(pNew);
      TestPoint(0);
      return i;
    }
  }

  /* No exact matches.  Look for a completely unused slot */
  for(i=N_PREDEFINED_COLOR; i<N_COLOR; i++){
    if( htmlPtr->apColor[i]==0 ){
      htmlPtr->apColor[i] = pNew;
      htmlPtr->colorUsed |= (1<<i);
      TestPoint(0);
      return i;
    }
  }

  /* No empty slots.  Look for a slot that contains a color that
  ** isn't currently in use. */
  for(i=N_PREDEFINED_COLOR; i<N_COLOR; i++){
    if( ((htmlPtr->colorUsed >> i) & 1) == 0 ){
      Tk_FreeColor(htmlPtr->apColor[i]);
      htmlPtr->apColor[i] = pNew;
      htmlPtr->colorUsed |= (1<<i);
      TestPoint(0);
      return i;
    }
  }

  /* Ok, find the existing color that is closest to the color requested
  ** and use it. */
  closest = 0;
  closestDist = colorDistance(pNew, htmlPtr->apColor[0]);
  for(i=1; i<N_COLOR; i++){
    dist = colorDistance(pNew, htmlPtr->apColor[i]);
    if( dist < closestDist ){
      closestDist = dist;
      closest = i;
      TestPoint(0);
    }else{
      TestPoint(0);
    }
  }
  Tk_FreeColor(pNew);
  TestPoint(0);
  return i;
}

/*
** This routine searchs for a hyperlink beneath the coordinates x,y
** and returns a pointer to the HREF for that hyperlink.  The text
** is held one of the markup.argv[] fields of the <a> markup.
*/
char *HtmlGetHref(HtmlWidget *htmlPtr, int x, int y){
  HtmlBlock *pBlock;
  HtmlElement *pElem;

  for(pBlock=htmlPtr->firstBlock; pBlock; pBlock=pBlock->pNext){
    if( pBlock->top > y || pBlock->bottom < y
     || pBlock->left > x || pBlock->right < x
    ){
      TestPoint(0);
      continue;
    }
    pElem = pBlock->base.pNext;
    if( (pElem->base.style.flags & STY_Anchor)==0 ){ TestPoint(0); continue; }
    switch( pElem->base.type ){
      case Html_Text:
      case Html_Space:
      case Html_IMG:
        while( pElem && pElem->base.type!=Html_EndA ){
          pElem = pElem->pNext;
          TestPoint(0);
        }
        if( pElem==0 || pElem->base.type!=Html_EndA ){ TestPoint(0); break; }
        pElem = pElem->ref.pOther;
        if( pElem==0 || pElem->base.type!=Html_A ){ TestPoint(0); break; }
        TestPoint(0);
        return HtmlMarkupArg(pElem,"href", 0);
      default:
        TestPoint(0);
        break;
    }
  }
  TestPoint(0);
  return 0;
}

/*
** Change the "yOffset" field from its current value to the value given.
** This has the effect of scrolling the widget vertically.
*/
void HtmlVerticalScroll(HtmlWidget *htmlPtr, int yOffset){
  int inset;  /* The 3D border plus the pady */
  int h;      /* Height of the clipping window */
  int diff;   /* Difference between old and new offset */
  GC gc;      /* Graphics context used for copying */
  int w;      /* Width of text area */

  if( yOffset==htmlPtr->yOffset ){ TestPoint(0); return; }
  inset = htmlPtr->pady + htmlPtr->inset;
  h = htmlPtr->realHeight - 2*inset;
  if( (htmlPtr->flags & REDRAW_TEXT)!=0
   || (htmlPtr->dirtyTop < h && htmlPtr->dirtyBottom > 0)
   || htmlPtr->yOffset > yOffset + (h - 30)
   || htmlPtr->yOffset < yOffset - (h - 30)
  ){
    htmlPtr->yOffset = yOffset;
    htmlPtr->flags |= VSCROLL | REDRAW_TEXT;
    HtmlScheduleRedraw(htmlPtr);
    TestPoint(0);
    return;
  }
  diff = htmlPtr->yOffset - yOffset;
  gc = HtmlGetAnyGC(htmlPtr);
  w = htmlPtr->realWidth - 2*(htmlPtr->inset + htmlPtr->padx);
  htmlPtr->flags |= VSCROLL;
  /* HtmlUnmapControls(htmlPtr); */
  htmlPtr->yOffset = yOffset;
  if( diff < 0 ){
    XCopyArea(htmlPtr->display, 
            Tk_WindowId(htmlPtr->clipwin),    /* source */
            Tk_WindowId(htmlPtr->clipwin),    /* destination */
            gc, 
            0, -diff,                         /* source X, Y */
            w, h + diff,                      /* Width and height */
            0, 0);                            /* Destination X, Y */
    HtmlRedrawArea(htmlPtr, 0, h + diff, w, h);
    TestPoint(0);
  }else{
    XCopyArea(htmlPtr->display, 
            Tk_WindowId(htmlPtr->clipwin),    /* source */
            Tk_WindowId(htmlPtr->clipwin),    /* destination */
            gc, 
            0, 0,                             /* source X, Y */
            w, h - diff,                      /* Width and height */
            0, diff);                         /* Destination X, Y */
    HtmlRedrawArea(htmlPtr, 0, 0, w, diff);
    TestPoint(0);
  }
  HtmlMapControls(htmlPtr);
}

/*
** Change the "xOffset" field from its current value to the value given.
** This has the effect of scrolling the widget horizontally.
*/
void HtmlHorizontalScroll(HtmlWidget *htmlPtr, int xOffset){
  if( xOffset==htmlPtr->xOffset ){ TestPoint(0); return; }
  htmlPtr->xOffset = xOffset;
  HtmlMapControls(htmlPtr);
  htmlPtr->flags |= HSCROLL | REDRAW_TEXT;
  HtmlScheduleRedraw(htmlPtr);
  TestPoint(0);
}

/*
** The following array defines all possible widget command.  The main
** widget command function just parses up the command line, then vectors
** control to one of the command service routines defined in the 
** following array:
*/
static struct HtmlSubcommand {
  char *zCmd1;           /* First-level subcommand.  Required */
  char *zCmd2;           /* Second-level subcommand.  May be NULL */
  int minArgc;           /* Minimum number of arguments */
  int maxArgc;           /* Maximum number of arguments */
  char *zHelp;           /* Help string if wrong number of arguments */
  int (*xFunc)(HtmlWidget*,Tcl_Interp*,int,char**);  /* Cmd service routine */
} aSubcommand[] = {
  { "base",      0,         2, 3, "?URL?",               HtmlBaseCmd },
  { "cget",      0,         3, 3, "CONFIG-OPTION",       HtmlCgetCmd },
  { "clear",     0,         2, 2, 0,                     HtmlClearCmd },
  { "configure", 0,         2, 0, "?ARGS...?",           HtmlConfigCmd },
  { "href",      0,         4, 4, "X Y",                 HtmlHrefCmd },
  { "index",     0,         3, 3, "INDEX",               HtmlIndexCmd },
  { "insert",    0,         3, 3, "INDEX",               HtmlInsertCmd },
  { "names",     0,         2, 2, 0,                     HtmlNamesCmd },
  { "parse",     0,         3, 3, "HTML-TEXT",           HtmlParseCmd },
  { "selection", "clear",   3, 3, 0,                     HtmlSelectionClearCmd},
  { 0,           "set",     5, 5, "START END",           HtmlSelectionSetCmd },
  { "text",      "ascii",   5, 5, "START END",           0 },
  { 0,           "delete",  5, 5, "START END",           0 },
  { 0,           "html",    5, 5, "START END",           0 },
  { 0,           "insert",  5, 5, "INDEX TEXT",          0 },
  { "token",     "append",  5, 5, "TAG ARGUMENTS",       0 },
  { 0,           "delete",  4, 5, "INDEX ?INDEX?",       0 },
  { 0,           "find",    4, 6, "TAG ?before|after INDEX?", 0 },
  { 0,           "get",     4, 5, "INDEX ?INDEX?",       0 },
  { 0,           "handler", 4, 5, "TAG ?SCRIPT?",        HtmlTokenHandlerCmd },
  { 0,           "insert",  6, 6, "INDEX TAG ARGUMENTS", 0 },
  { "xview",     0,         2, 5, "OPTIONS...",          HtmlXviewCmd },
  { "yview",     0,         2, 5, "OPTIONS...",          HtmlYviewCmd },
  { "_su",       0,         3, 3, 0,                     HtmlPrivateSuCmd },
  { "_re",       0,         3, 3, 0,                     HtmlPrivateReCmd },
  { "_ff",       0,         3, 3, 0,                     HtmlPrivateFfCmd },
  { "_fb",       0,         3, 3, 0,                     HtmlPrivateFbCmd },
#ifdef DEBUG
  { "debug",     "dump",    5, 5, "START END",           HtmlDebugDumpCmd },
  { 0,           "testpt",  4, 4, "FILENAME",            HtmlDebugTestPtCmd },
#endif
};
#define nSubcommand (sizeof(aSubcommand)/sizeof(aSubcommand[0]))

/*
** This routine implements the command used by individual HTML widgets.
*/
static int HtmlWidgetCommand(
  ClientData clientData,	/* The HTML widget data structure */
  Tcl_Interp *interp,		/* Current interpreter. */
  int argc,			/* Number of arguments. */
  char **argv			/* Argument strings. */
){
  HtmlWidget *htmlPtr = (HtmlWidget*) clientData;
  size_t length;
  int c;
  int i;
  struct HtmlSubcommand *pCmd;

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
        " option ?arg arg ...?\"", 0);
    TestPoint(0);
    return TCL_ERROR;
  }
  c = argv[1][0];
  length = strlen(argv[1]);
  for(i=0, pCmd=aSubcommand; i<nSubcommand; i++, pCmd++){
    if( pCmd->zCmd1==0 || c!=pCmd->zCmd1[0] 
    || strncmp(pCmd->zCmd1,argv[1],length)!=0 ){
      TestPoint(0);
      continue;
    }
    if( pCmd->zCmd2 ){
      int length2;
      int j;
      if( argc<3 ){
        Tcl_AppendResult(interp, "wrong # args: should be \"",
          argv[0], " ", pCmd->zCmd1, " SUBCOMMAND ?OPTIONS...?", 0);
        TestPoint(0);
        return TCL_ERROR;
      }
      length2 = strlen(argv[2]);
      for(j=i; j<nSubcommand && (j==i || pCmd->zCmd1==0); j++, pCmd++){
        if( strncmp(pCmd->zCmd2,argv[2],length2)==0 ){
          TestPoint(0);
          break;
        }
      }
      if( j>=nSubcommand || (j!=i && aSubcommand[j].zCmd1!=0) ){
        Tcl_AppendResult(interp,"unknown subcommand \"", argv[2],
          "\" -- should be one of:", 0);
        for(j=i; j<nSubcommand && (j==i || aSubcommand[j].zCmd1==0); j++){
          Tcl_AppendResult(interp, " ", aSubcommand[j].zCmd2, 0);
          TestPoint(0);
        }
        return TCL_ERROR;
      }
    }
    if( argc<pCmd->minArgc || (argc>pCmd->maxArgc && pCmd->maxArgc>0) ){
      Tcl_AppendResult(interp,"wrong # args: should be \"", argv[0],
         " ", pCmd->zCmd1, 0);
      if( pCmd->zCmd2 ){
        Tcl_AppendResult(interp, " ", pCmd->zCmd2, 0);
        TestPoint(0);
      }
      if( pCmd->zHelp ){
        Tcl_AppendResult(interp, " ", pCmd->zHelp, 0);
        TestPoint(0);
      }
      Tcl_AppendResult(interp, "\"", 0);
      TestPoint(0);
      return TCL_ERROR;
    }
    if( pCmd->xFunc==0 ){
      Tcl_AppendResult(interp,"command not yet implemented", 0);
      TestPoint(0);
      return TCL_ERROR;
    }
    TestPoint(0);
    return (*pCmd->xFunc)(htmlPtr, interp, argc, argv);
  }
  Tcl_AppendResult(interp,"unknown command \"", argv[1], "\" -- should be "
    "one of:", 0);
  for(i=0; i<nSubcommand; i++){
    if( aSubcommand[i].zCmd1==0 || aSubcommand[i].zCmd1[0]=='_' ){ 
      TestPoint(0); 
      continue;
    }
    Tcl_AppendResult(interp, " ", aSubcommand[i].zCmd1, 0);
    TestPoint(0);
  }
  TestPoint(0);
  return TCL_ERROR;
}

/*
** The following routine implements the Tcl "html" command.  This command
** is used to create new HTML widgets only.  After the widget has been
** created, it is manipulated using the widget command defined above.
*/
static int HtmlCommand(
  ClientData clientData,	/* Main window */
  Tcl_Interp *interp,		/* Current interpreter. */
  int argc,			/* Number of arguments. */
  char **argv			/* Argument strings. */
){
  HtmlWidget *htmlPtr;
  Tk_Window new;
  Tk_Window clipwin;
  char *zClipwin;
  Tk_Window tkwin = (Tk_Window)clientData;
  static int varId = 1;        /* Used to construct unique names */

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
         argv[0], " pathName ?options?\"", (char *) NULL);
    TestPoint(0);
    return TCL_ERROR;
  }
  new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
  if (new == NULL) {
     TestPoint(0);
     return TCL_ERROR;
  }
  zClipwin = ckalloc( strlen(argv[1]) + 3 );
  if( zClipwin==0 ){
    Tk_DestroyWindow(new);
    TestPoint(0);
    return TCL_ERROR;
  }
  sprintf(zClipwin,"%s.x",argv[1]);
  clipwin = Tk_CreateWindowFromPath(interp, new, zClipwin, 0);
  if( clipwin==0 ){
    Tk_DestroyWindow(new);
    ckfree(zClipwin);
    TestPoint(0);
    return TCL_ERROR;
  }

  htmlPtr = (HtmlWidget*) ckalloc(sizeof(HtmlWidget));
  memset(htmlPtr, 0, sizeof(HtmlWidget));
  htmlPtr->tkwin = new;
  htmlPtr->clipwin = clipwin;
  htmlPtr->zClipwin = zClipwin;
  htmlPtr->display = Tk_Display(new);
  htmlPtr->interp = interp;
  htmlPtr->widgetCmd = Tcl_CreateCommand(interp, Tk_PathName(new),
      HtmlWidgetCommand, (ClientData) htmlPtr, HtmlCmdDeletedProc);
  htmlPtr->relief = TK_RELIEF_FLAT;
  htmlPtr->dirtyLeft = LARGE_NUMBER;
  htmlPtr->dirtyTop = LARGE_NUMBER;
  htmlPtr->flags = RESIZE_CLIPWIN;
  htmlPtr->varId = varId++;
  
  Tk_SetClass(new,"Html");
  Tk_SetClass(clipwin,"HtmlClip");
  Tk_CreateEventHandler(htmlPtr->tkwin,
       ExposureMask|StructureNotifyMask|FocusChangeMask,
       HtmlEventProc, (ClientData) htmlPtr);
  Tk_CreateEventHandler(htmlPtr->clipwin,
       ExposureMask|StructureNotifyMask,
       HtmlEventProc, (ClientData) htmlPtr);
  if (ConfigureHtmlWidget(interp, htmlPtr, argc-2, argv+2, 0) != TCL_OK) {
     TestPoint(0);
     goto error;
  }
  interp->result = Tk_PathName(htmlPtr->tkwin);
  TestPoint(0);
  return TCL_OK;

  error:
  Tk_DestroyWindow(htmlPtr->tkwin);
  TestPoint(0);
  return TCL_ERROR;
}

/*
** This routine is used to register the "html" command with the
** Tcl interpreter.  This is the only routine in this file with
** external linkage.
*/
int Tkhtml_Init(Tcl_Interp *interp){
  Tcl_CreateCommand(interp,"html", HtmlCommand, 
      Tk_MainWindow(interp), 0);
  Tcl_GlobalEval(interp,HtmlLib);
#ifdef DEBUG
  Tcl_LinkVar(interp, "HtmlTraceMask", (char*)&HtmlTraceMask, TCL_LINK_INT);
#endif
  TestPoint(0);
  return TCL_OK;
}
