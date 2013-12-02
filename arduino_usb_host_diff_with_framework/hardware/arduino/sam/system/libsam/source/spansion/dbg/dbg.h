/*******************************************************************************
* Copyright (C) 2013 Spansion LLC. All Rights Reserved. 
*
* This software is owned and published by: 
* Spansion LLC, 915 DeGuigne Dr. Sunnyvale, CA  94088-3453 ("Spansion").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND 
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with Spansion 
* components. This software is licensed by Spansion to be adapted only 
* for use in systems utilizing Spansion components. Spansion shall not be 
* responsible for misuse or illegal use of this software for devices not 
* supported herein.  Spansion is providing this software "AS IS" and will 
* not be responsible for issues arising from incorrect user implementation 
* of the software.  
*
* SPANSION MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS), 
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING, 
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED 
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED 
* WARRANTY OF NONINFRINGEMENT.  
* SPANSION SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT, 
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT 
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, 
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR 
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA, 
* SAVINGS OR PROFITS, 
* EVEN IF SPANSION HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED 
* FROM, THE SOFTWARE.  
*
* This software may be replicated in part or whole for the licensed use, 
* with the restriction that this Disclaimer and Copyright notice must be 
* included with each copy of this software, whether used in part or whole, 
* at all times.  
*/

/************************************************************************/
/** \file dbg.h
 **
 ** - debug routines for use with uart module  
 **
 ** History:
 **   - 2010-03-30    1.2  MSc  First version  (works with 16FX)
 *****************************************************************************/

#ifndef _DBG_H
    #define _DBG_H
    #define puts0(x)  puts(x)
  
    #define debug_enabled    0
    #define use_printbuffer 0
  
  
    #if (debug_enabled)
      #define DEBUGLEVEL 15
      #define DBGOUT(dbglevel,x)   if (dbglevel <= DEBUGLEVEL) { x; }
      #define dbg(x)    puts(x)
      #define dbgsect(x,y)    dbg(x);dbg(y)
      #define dbgval(x,y)  dbg(x);dbghex(y);dbg("\n")
      #define dbgsectval(x,y,z)    dbg(x);dbgval(y,z)
      #define dbghex(x)    _dbghex((uint32_t)x)
      
      void _dbghex(uint32_t n);
      #if (use_printbuffer)
          void bufferprintout(uint8_t* buffer, uint32_t size);
          #define printBuffer(x,y)    bufferprintout(x,y);
      #else
          #define printBuffer(x,y) ;
      #endif
    #else
        #define dbg(x)   
        #define dbghex(x) 
        #define printBuffer(x,y)
        #define dbgval(x,y)
        #define DEBUGLEVEL
        #define DBGOUT(dbglevel,x)
    #endif
#endif
