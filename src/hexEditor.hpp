/* $Id: hexEditor.hpp,v 1.7 2006-11-05 04:42:43 ganzhorn Exp $
 * This file is part of lfhex.
 * Copyright (C) 2006 Salem Ganzhorn <eyekode@yahoo.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef _HEX_EDITOR_
#define _HEX_EDITOR_

#include <QMouseEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QWidget>

// STL headers
#include <vector>

// System headers
#include <sys/types.h>

// Local headers
#include "local.h"
#include "reader.hpp"
#include "delta.hpp"
#include "cursor.hpp"
#include "save.hpp"

class HexEditor : public QWidget {
  Q_OBJECT
public:
  HexEditor( QWidget * parent =0 );
  ~HexEditor();

  // note: closeFile returns QMessageBox::{Yes,No,Cancel}
  int closeFile(bool force = false );
  bool open(const QString& filename);
  // becomes "save as" if filename != ""
  bool save(QString filename = "");
  bool isModified() const;
  QString filename() const;
  Reader * reader();
  off_t offset() const;
  

  // returns the word (string representation) that the cursor is in
  //  QString currentWord() const;
  //
  // cursor movement related fn's
  //
  void seeCursor();
  void cursorLeft();
  void cursorRight();
  void cursorUp();
  void cursorDown();
  int  wordWidth() const;
  int  wordSpacing() const;
  void setCursorFromXY(int x,int y);

public:
  enum SelectionPos_e {
    SelectionStart,
    SelectionEnd,
  };
  void setSelection(SelectionPos_e pos,off_t byte_offset);
  off_t selectionStart() const; 
  off_t selectionEnd() const;
  // get the char offset from topleft of the point (x,y)
  // Note: this wont work for large files, off_t is 2*size() at eof for hex
  //off_t charOffsetAtXY( off_t x, off_t y );
  int   offsetToPercent( off_t offset );

  off_t localByteOffsetAtXY( off_t x, off_t y );
protected:
  // accessors for local offset's
  // note: returned offset's are not necessarily visible
  off_t   localByteOffset() const;
  off_t   localWordOffset() const;
  off_t   localCharOffset() const;
  off_t   localLineOffset() const;
 
signals:
  // attach to know when line ranges change (for a scrollbar)
  void rangeChanged(off_t low, off_t high);
  void offsetChanged(off_t byte);
  void topLeftChanged(off_t offset);
  void selectionChanged(const QString& selection);

public slots:
  bool browseLoadFile();
  void setOffset(off_t offset);     // sets cursor offset
  void setTopLeftToPercent( int percent ); // for setting pos from scroll
  //
  // slots for setting the number of bytes per column
  // I really should change all references of "bytesPerWord" to "bytesPerCol"
  // sorry for the confusion -Salem
  //
  void setBytesPerWord( int i );
  void set1BPC() { setBytesPerWord(1); };
  void set2BPC() { setBytesPerWord(2); };
  void set4BPC() { setBytesPerWord(4); };
  void set8BPC() { setBytesPerWord(8); };
  void nextLine();
  void prevLine();
  void nextPage();
  void prevPage();
  void setFont(const QFont& font);
  void undo();
  void redo();
  void search(const QString& hexText, bool forwards);
  void setBase(int base); // only 2 8 16 acceptableO
  void setBaseHex();
  void setBaseOctal();
  void setBaseBinary();
  void setBaseASCII();

protected:
  //  void setOffsetLabels( off_t topLeft );
  void setTopLeft( off_t offset );

protected:
  bool wordModified( off_t widx ) const;
  // template setWord so I can call it with different types of string data
  template <class _vectType>
  void setWord( off_t wordIdx, const _vectType& str );
  // calculate the rectangle which bounds the word at _offset+idx
  QRect  charBBox( off_t charIdx ) const;
  QRect  byteBBox( off_t byteIdx ) const;

  // translate widget coord to word index
  int pointToWord(const QPoint& pt);
  QChar& pointToChar(const QPoint& pt);
  // inlined access fn's
protected:
  void calculateFontMetrics();
  int charsPerByte() const;
  int charsPerWord() const;
  int charsPerLine() const;
  int bytesPerPage() const;
  int bytesPerWord() const;
  int bytesPerLine() const;
  int wordsPerLine() const;
  int wordsPerPage() const;
  int linesPerPage() const;

  int lineSpacing()  const;
  int fontHeight()   const;
  int fontMaxWidth() const;
  int topMargin() const;
  int leftMargin() const;

  // translate local offset to global offset
  off_t globalOffset( off_t local ) const;
  // translate global offset to local offset.
  // Note: globalOffset may not exist in local viewport.
  off_t localOffset ( off_t global) const;

protected:
  void showMatch( off_t pos, int len );
  // drawing utilities
protected:
    bool getDisplayText( QString& text );
    void drawCursor( QPainter& p );
    void drawSelection( QPainter& p );
    void drawTextRegion( QPainter& p, const QString& text,
			 int row_start, int row_end,
			 int col_start, int col_end );
  // event handlers
protected:
  void resizeEvent      ( QResizeEvent *e );
  // generates update() events for the bbox for the word given byh wordIdx
  // note: this is range safe, no need to do range checking before passing in
  void updateWord       ( off_t wordIdx );
  void paintEvent       ( QPaintEvent*e );
  void paintLabels      ( QPainter*painter);
  void focusInEvent     ( QFocusEvent*e );
  void focusOutEvent    ( QFocusEvent*e );
  void keyPressEvent    ( QKeyEvent *e );
  void mousePressEvent  ( QMouseEvent * e );
  void mouseReleaseEvent( QMouseEvent *e );
  void mouseMoveEvent   ( QMouseEvent *e );

protected:
  Reader              _reader;

  int                 _wordSpacing;
  int                 _wordWidth;
  int                 _lineSpacing;
  int                 _fontHeight;
  int                 _bytesPerWord;
  int                 _charsPerByte;
  off_t               _topLeft;

  //
  // current screen data info              
  //
  vector<uchar>       _data;
  int                 _base; // either 2 8 or 16

  off_t               _lastValidWord;

  // data used to optimize drawing.
  vector<QRect>       _lineBBox;
  vector<QRect>       _wordBBox;
  QRect               _labelBBox;

  int                 _linspacing;
  int                 _fontMaxWidth;

  int                 _topMargin,_leftMargin;
  int                 _offsetLabelBytes;

  int                 _cols;
  int                 _rows;
  int                 _width;
  Cursor              _cursor; 

  //
  // when refering to the offset of a word, I always mean the offset of the 
  // first byte of the word
  // 

  // this class keeps track of modifications
  DeltaMap            _delta;
  
  // selection:
  off_t               _selection[2];
};

#endif
